#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

struct Server {
  char ip[255];
  int port;
};

struct ThreadArgs {
  struct Server server;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  uint64_t* result;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

struct Server* read_servers(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open servers file");
        return NULL;
    }
    
    char buffer[255];
    *count = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        (*count)++;
    }
    
    rewind(file);
    
    struct Server* servers = malloc(*count * sizeof(struct Server));
    
    for (int i = 0; i < *count; i++) {
        fgets(buffer, sizeof(buffer), file);
        char* colon = strchr(buffer, ':');
        if (colon) {
            *colon = '\0';
            strncpy(servers[i].ip, buffer, sizeof(servers[i].ip) - 1);
            servers[i].port = atoi(colon + 1);
        }
    }
    
    fclose(file);
    return servers;
}

void* server_worker(void* args) {
    struct ThreadArgs* targs = (struct ThreadArgs*)args;
    
    struct hostent *hostname = gethostbyname(targs->server.ip);
    if (hostname == NULL) {
        fprintf(stderr, "gethostbyname failed with %s\n", targs->server.ip);
        *(targs->result) = 1;
        return NULL;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(targs->server.port);
    server_addr.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
        fprintf(stderr, "Socket creation failed!\n");
        *(targs->result) = 1;
        return NULL;
    }

    if (connect(sck, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Connection to %s:%d failed\n", targs->server.ip, targs->server.port);
        close(sck);
        *(targs->result) = 1;
        return NULL;
    }

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &targs->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &targs->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &targs->mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
        fprintf(stderr, "Send failed to %s:%d\n", targs->server.ip, targs->server.port);
        close(sck);
        *(targs->result) = 1;
        return NULL;
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
        fprintf(stderr, "Receive failed from %s:%d\n", targs->server.ip, targs->server.port);
        close(sck);
        *(targs->result) = 1;
        return NULL;
    }

    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
    *(targs->result) = answer;

    close(sck);
    return NULL;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers_file[255] = {'\0'};

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        memcpy(servers_file, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers_file)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n", argv[0]);
    return 1;
  }

  int servers_num = 0;
  struct Server* servers = read_servers(servers_file, &servers_num);
  if (servers == NULL || servers_num == 0) {
    fprintf(stderr, "No servers found in file %s\n", servers_file);
    return 1;
  }

  pthread_t threads[servers_num];
  struct ThreadArgs thread_args[servers_num];
  uint64_t results[servers_num];

  uint64_t chunk_size = k / servers_num;
  for (int i = 0; i < servers_num; i++) {
    thread_args[i].server = servers[i];
    thread_args[i].begin = i * chunk_size + 1;
    thread_args[i].end = (i == servers_num - 1) ? k : (i + 1) * chunk_size;
    thread_args[i].mod = mod;
    thread_args[i].result = &results[i];
    
    if (pthread_create(&threads[i], NULL, server_worker, (void*)&thread_args[i])) {
      fprintf(stderr, "Error creating thread for server %d\n", i);
      return 1;
    }
  }

  uint64_t total = 1;
  for (int i = 0; i < servers_num; i++) {
    pthread_join(threads[i], NULL);
    total = MultModulo(total, results[i], mod);
  }

  printf("Final result: %llu! mod %llu = %llu\n", k, mod, total);

  free(servers);
  return 0;
}
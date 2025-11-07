#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int start;
    int end;
    long long mod;
    long long partial_result;
} thread_data_t;

long long global_result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int k = 0;
int pnum = 1;
long long mod = 0;

void parse_arguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-k") == 0) {
        k = atoi(argv[i + 1]);
        i++;
    } else if (strcmp(argv[i], "--pnum") == 0) {
        pnum = atoi(argv[i + 1]);
        i++;
    } else if (strcmp(argv[i], "--mod") == 0) {
        mod = atoi(argv[i + 1]);
        i++;
    }
    }   
    
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Usage: %s -k <number> --pnum <threads> --mod <modulus>\n", argv[0]);
        fprintf(stderr, "All parameters must be positive integers\n");
        exit(1);
    }
}


void* calculate_partial_factorial(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    data->partial_result = 1;
    
    printf("Thread: calculating from %d to %d\n", data->start, data->end);
    
    for (int i = data->start; i <= data->end; i++) {
        data->partial_result = (data->partial_result * i) % data->mod;
    }
    
    pthread_mutex_lock(&mutex);
    global_result = (global_result * data->partial_result) % mod;
    printf("Thread: partial result = %lld, global result = %lld\n", 
           data->partial_result, global_result);
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);
    
    printf("Calculating %d! mod %lld using %d threads\n", k, mod, pnum);
    
    if (k == 0 || k == 1) {
        printf("Result: 1\n");
        return 0;
    }
    
    pthread_t threads[pnum];
    thread_data_t thread_data[pnum];
    
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    for (int i = 0; i < pnum; i++) {
        int numbers_for_this_thread = numbers_per_thread;
        if (i < remainder) {
            numbers_for_this_thread++;
        }
        
        thread_data[i].start = current_start;
        thread_data[i].end = current_start + numbers_for_this_thread - 1;
        thread_data[i].mod = mod;
        thread_data[i].partial_result = 1;
        
        current_start += numbers_for_this_thread;
        
        if (pthread_create(&threads[i], NULL, calculate_partial_factorial, &thread_data[i]) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
    
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            exit(1);
        }
    }
    
    printf("\nFinal result: %d! mod %lld = %lld\n", k, mod, global_result);
    
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
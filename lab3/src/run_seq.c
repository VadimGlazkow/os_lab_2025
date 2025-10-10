#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        char *seed = argv[1];
        char *array_size = argv[2];
        char *args[] = {"sequential_min_max", seed, array_size, NULL};
        
        execv("./sequential_min_max", args);
        perror("execv failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
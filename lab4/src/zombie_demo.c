#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Родительский процесс
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        printf("Child became zombie for 10 seconds...\n");
        sleep(10);  // родитель спит, не делает wait()
        printf("Parent exits, zombie cleaned by init.\n");
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Child process exiting now...\n");
        exit(0); // завершается сразу
    } else {
        perror("fork failed");
        return 1;
    }

    return 0;
}

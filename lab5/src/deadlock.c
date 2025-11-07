#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_function(void* arg) {
    printf("Thread 1: пытается захватить mutex1\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 1: захватил mutex1\n");
    
    sleep(1);
    
    printf("Thread 1: пытается захватить mutex2 (будет deadlock!)\n");
    pthread_mutex_lock(&mutex2);  // DEADLOCK!
    printf("Thread 1: захватил mutex2\n");
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
}

void* thread2_function(void* arg) {
    printf("Thread 2: пытается захватить mutex2\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 2: захватил mutex2\n");
    
    sleep(1);
    
    printf("Thread 2: пытается захватить mutex1 (будет deadlock!)\n");
    pthread_mutex_lock(&mutex1);  // DEADLOCK!
    printf("Thread 2: захватил mutex1\n");
    
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}

int main() {
    pthread_t thread1, thread2;
    
    printf("=== Демонстрация Deadlock ===\n");
    
    pthread_create(&thread1, NULL, thread1_function, NULL);
    pthread_create(&thread2, NULL, thread2_function, NULL);
    
    // Даем время потокам войти в deadlock
    sleep(3);
    
    printf("\n=== Состояние после 3 секунд ===\n");
    
    // Эти вызовы заблокируются навсегда из-за deadlock
    printf("Ждем завершения thread1 (заблокируется)...\n");
    pthread_join(thread1, NULL);
    
    printf("Ждем завершение thread2 (эта строка не выполнится)...\n");
    pthread_join(thread2, NULL);
    
    return 0;
}
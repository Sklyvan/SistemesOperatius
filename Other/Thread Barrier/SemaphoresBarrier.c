#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NTHREADS 100
#define BARRIER_SIZE 25 // Número necesarios de threads esperando para levantar la barrera.

int K = 0;
sem_t Semaphore;

void* doSomething(){
    sem_wait(&Semaphore);
    K += 1; // Critical Region.
    sem_post(&Semaphore);

    return NULL;
}

int main() {
    pthread_t threadsID[NTHREADS];

    sem_init(&Semaphore, 0, BARRIER_SIZE); // Dirección de Memoria, 0 indica uso en en threads y el valor del semáforo.

    for (int i=0; i<NTHREADS; i++) { pthread_create(&threadsID[i], NULL, doSomething, NULL); }

    for (int i=0; i<NTHREADS; i++){ pthread_join(threadsID[i], NULL); }

    sem_destroy(&Semaphore);

    return 0;
}

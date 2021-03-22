#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 100
#define BARRIER_SIZE 25 // Número necesarios de threads esperando para levantar la barrera.

int K = 0;
int readyThreads = 0;

pthread_cond_t isReady;
pthread_mutex_t Lock;

void* doSomething(){
    pthread_mutex_lock(&Lock);
    while(readyThreads < BARRIER_SIZE) pthread_cond_wait(&isReady, &Lock);
    K += 1; // Critical Region.
    pthread_cond_broadcast(&isReady);
    pthread_mutex_unlock(&Lock);

    return NULL;
}

int main() {
    pthread_t threadsID[NTHREADS];
    pthread_mutex_init(&Lock, NULL);
    pthread_cond_init(&isReady, NULL);

    for (int i=0; i<NTHREADS; i++) {
        readyThreads++;
        pthread_create(&threadsID[i], NULL, doSomething, NULL);
    }

    for (int i=0; i<NTHREADS; i++){ pthread_join(threadsID[i], NULL); }

    return 0;
}

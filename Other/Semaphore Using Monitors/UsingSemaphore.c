#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <stdbool.h>

#define SEMAPHORE_VALUE 5
#define NTHREADS 10000

int K = 0; // Nuestros threads van a operar sobre esta variable global, la usaremos para ver si hay Race Condition.
pthread_t threadsID[NTHREADS]; // Crearemos varios threads para ver que no hay Race Condition ni Deadlock.
sem_t Semaphore; pthread_mutex_t Lock;
int CurrentThreads = 0;

int Factorial(int N){
    if (N <= 1) return N;
    else return N*Factorial(N-1);
}

void* ThreadFunction(){
    sem_wait(&Semaphore);
    pthread_mutex_lock(&Lock); CurrentThreads++; pthread_mutex_unlock(&Lock);
    printf("Current Number of Threads: %d \n", CurrentThreads);
    int N1, N2, N3, N4, N5;
    N1 = 100%(K+1) + 1; N2 = 100%(K+2) + 1; N3 = 100%(K+3) + 1; N4 = 100%(K+4) + 1; N5 = 100%(K+5) + 1;
    N1 = Factorial(N1); N2 = Factorial(N2); N3 = Factorial(N3); N4 = Factorial(N4); N5 = Factorial(N5);
    int N[5] = {N1, N2, N3, N4, N5};
    K += N[rand()%5];
    pthread_mutex_lock(&Lock); CurrentThreads--; pthread_mutex_unlock(&Lock);
    sem_post(&Semaphore);
    return NULL;
}

int main() {
    sem_init(&Semaphore, 0, SEMAPHORE_VALUE); pthread_mutex_init(&Lock, NULL);

    for (int i=0; i<NTHREADS; i++) pthread_create(&threadsID[i], NULL, ThreadFunction, NULL);
    for (int i=0; i<NTHREADS; i++) pthread_join(threadsID[i], NULL);

    printf("--------> END OF THREADS <--------\n");

    if (K == NTHREADS) printf("No Race Condition!\n");
    else printf("Race Condition! (K = %d)\n", K);

    return 0;
}

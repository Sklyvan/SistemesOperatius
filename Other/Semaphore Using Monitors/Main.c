#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define SEMAPHORE_VALUE 5
#define NTHREADS 10000

int K = 0;
pthread_t threadsID[NTHREADS];

typedef struct SemStruct{
  int AvaliableThreads;
  pthread_mutex_t Lock; pthread_cond_t Condition;
} Semaphore;

void InitSemaphore(Semaphore* CurrentSemaphore, int n){
    CurrentSemaphore->AvaliableThreads = n;
    pthread_mutex_init(&CurrentSemaphore->Lock, NULL);
    pthread_cond_init(&CurrentSemaphore->Condition, NULL);
}

void Wait(Semaphore* CurrentSemaphore){
    pthread_mutex_lock(&CurrentSemaphore->Lock);
    while(CurrentSemaphore->AvaliableThreads == 0) pthread_cond_wait(&CurrentSemaphore->Condition, &CurrentSemaphore->Lock);
    CurrentSemaphore->AvaliableThreads--;
    pthread_mutex_unlock(&CurrentSemaphore->Lock);
}

void Signal(Semaphore* CurrentSemaphore){
    pthread_mutex_lock(&CurrentSemaphore->Lock);
    pthread_cond_signal(&CurrentSemaphore->Condition);
    CurrentSemaphore->AvaliableThreads++;
    pthread_mutex_unlock(&CurrentSemaphore->Lock);
}

int Factorial(int N){
    if (N <= 1) return N;
    else return N*Factorial(N-1);
}

Semaphore* MySemaphore;

void* ThreadFunction(){
    Wait(MySemaphore);

    printf("Current Number of Threads: %d \n", SEMAPHORE_VALUE - MySemaphore->AvaliableThreads);
    int N1, N2, N3, N4, N5;
    N1 = 100%(K+1) + 1; N2 = 100%(K+2) + 1; N3 = 100%(K+3) + 1; N4 = 100%(K+4) + 1; N5 = 100%(K+5) + 1;
    N1 = Factorial(N1); N2 = Factorial(N2); N3 = Factorial(N3); N4 = Factorial(N4); N5 = Factorial(N5);
    int N[5] = {N1, N2, N3, N4, N5};
    K += N[rand()%5];

    Signal(MySemaphore);
    return NULL;
}

int main() {
    MySemaphore = malloc(sizeof(Semaphore));
    InitSemaphore(MySemaphore, SEMAPHORE_VALUE);

    for (int i=0; i<NTHREADS; i++) pthread_create(&threadsID[i], NULL, ThreadFunction, NULL);
    for (int i=0; i<NTHREADS; i++) pthread_join(threadsID[i], NULL);

    printf("--------> END OF THREADS <--------\n");

    return 0;
}

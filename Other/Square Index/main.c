#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/shm.h>

#define NTHREADS 2
#define ARRAY_SIZE 100

pthread_t threadsID[NTHREADS];
pthread_mutex_t arrayLocks[ARRAY_SIZE];
int Array[ARRAY_SIZE];

void* Thread1(){
    for(int i=0; i<ARRAY_SIZE; i++){
        pthread_mutex_lock(&arrayLocks[i]);
        Array[i] = Array[i] * i;
        pthread_mutex_unlock(&arrayLocks[i]);
    }
    return NULL;
}

void* Thread2(){
    for(int i=ARRAY_SIZE-1; i>=0; i--){
        pthread_mutex_lock(&arrayLocks[i]);
        Array[i] = Array[i] * i;
        pthread_mutex_unlock(&arrayLocks[i]);
    }
    return NULL;
}

int main() {
    for (int i=0; i<ARRAY_SIZE; i++) {Array[i] = 1; pthread_mutex_init(&arrayLocks[i], NULL); }

    pthread_create(&threadsID[0], NULL, Thread1, NULL);
    pthread_create(&threadsID[1], NULL, Thread2, NULL);

    for (int i=0; i<NTHREADS; i++) pthread_join(threadsID[i], NULL);

    for (int i=0; i<ARRAY_SIZE; i++) if (Array[i] != i*i) printf("ERROR! ---> Array[%d] = %d | %d² = %d\n", i, Array[i], i, i*i);

    return 0;
}

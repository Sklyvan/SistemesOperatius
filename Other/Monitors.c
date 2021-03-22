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
#include <stdbool.h>

#define NTHREADS 50
#define NSUMS 100

int SUM[2] = {0, 0};
pthread_t threadsID[NTHREADS];

bool isBusy1, isBusy2;
pthread_mutex_t Lock1, Lock2;
pthread_cond_t Condition1, Condition2;

void* ThreadFunction1(){
    for (int i=0; i<NSUMS; i++)
    {
        pthread_mutex_lock(&Lock1);
        while(isBusy1) pthread_cond_wait(&Condition1, &Lock1);
        isBusy1 = true;
        SUM[0]++;
        isBusy1 = false;
        pthread_cond_signal(&Condition1);
        pthread_mutex_unlock(&Lock1);
    }
    return NULL;
}

void* ThreadFunction2(){
    for (int i=0; i<NSUMS; i++)
    {
        pthread_mutex_lock(&Lock2);
        while(isBusy2) pthread_cond_wait(&Condition2, &Lock2);
        isBusy2 = true;
        SUM[1]++;
        isBusy2 = false;
        pthread_cond_signal(&Condition2);
        pthread_mutex_unlock(&Lock2);
    }
    return NULL;
}

int Main3()
{
    pthread_mutex_init(&Lock1, NULL); pthread_mutex_init(&Lock2, NULL);
    pthread_cond_init(&Condition1, NULL); pthread_cond_init(&Condition2, NULL);
    isBusy1 = false; isBusy2 = false;
    for (int i=0; i<NTHREADS; i++){
        if (i%2 == 0) pthread_create(&threadsID[i], NULL, ThreadFunction1,NULL);
        else pthread_create(&threadsID[i], NULL, ThreadFunction2, NULL);
    }

    for (int i=0; i<NTHREADS; i++) pthread_join(threadsID[i], NULL);

    printf("SUM = {%d, %d}\n", SUM[0], SUM[1]);
    printf("Total Sum = %d\n", SUM[0]+SUM[1]);

    return 0;
}
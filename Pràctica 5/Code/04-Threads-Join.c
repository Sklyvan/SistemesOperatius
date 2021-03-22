#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NTHREADS 10


void* fsleep(void* arg) {
    usleep(1);
    int id = (long) arg;
    printf("fsleep thread function ID: %d\n", id);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tids[NTHREADS];
    srand(time(NULL));

    for(int i=0; i<NTHREADS; i++) {
        printf("Creating thread %d\n", i);
        pthread_create(&tids[i], NULL, fsleep, (void*) (long) i);
    }

    for(int i=0; i<NTHREADS; i++) pthread_join(tids[i], NULL);

}

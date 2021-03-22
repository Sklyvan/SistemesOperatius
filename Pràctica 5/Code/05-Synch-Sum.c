#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep function : waits for seconds
#include <time.h>    // for usleep : waits for microseconds
#include <sys/time.h>
struct timeval start;
void startTimer();
long endTimer();


#define NTHREADS 10
#define NSUMS 4

int sums[NSUMS];
pthread_mutex_t  locks[NSUMS]; // Creamos un array de locks, para hacer el lock de cada posición.

void* fsum(void * arg) {
    for(int i=0;i<500;i++) {
        pthread_mutex_lock(&locks[i%NSUMS]); // Hacemos lock de esa posicón determinada del array.
        sums[i%NSUMS] = sums[i%NSUMS] + 1;
        usleep(100+rand()%100);
        pthread_mutex_unlock(&locks[i%NSUMS]); // Ya podemos desbloquear la posición del array.
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tids[NTHREADS];

    startTimer();

    // Inicializamos el array de locks.
    for(int i=0; i<NSUMS; i++) { sums[i] = 0; pthread_mutex_init(&locks[i], NULL); }


    for(int i=0; i<NTHREADS; i++) {
        printf("Creating thread %d\n", i);
        pthread_create(&tids[i], NULL, fsum, NULL);
    }

    for(int i=0; i<NTHREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    printf("Sums are: ");
    for(int i=0; i<NSUMS; i++)  printf("%d ", sums[i]);
    printf("    computed in %ld millis\n", endTimer());

}


void startTimer() {
    gettimeofday(&start, NULL);
}

long endTimer() {
    long mtime, seconds, useconds;
    struct timeval end;
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

    return mtime;
}

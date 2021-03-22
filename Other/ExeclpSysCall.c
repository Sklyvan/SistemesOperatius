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

int main()
{

    int FD = open("Data.dat", O_CREAT | O_RDWR);
    for (int i=0; i<100; i++){
        srand(time(NULL)); // Hacemos la seed de nuestra función rand, se el tiempo UNIX en ese momento.
        int K = rand()%50; // Rand(0, 50)
        lseek(FD, K*sizeof(int), SEEK_SET);
        write(FD, &K, sizeof(int));
    }


    return 0;
}
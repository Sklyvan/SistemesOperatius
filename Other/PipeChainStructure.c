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

#define NPROCESSES 10

int Main2(){
    srand(time(NULL)); // Hacemos la seed de nuestra función rand, se el tiempo UNIX en ese momento.
    int K = rand()%50; // Rand(0, 50)
    printf("Random Number: %d \n", K);
    // Pipe[0]: Read    Pipe[1]: Write
    int Pipe1[2]; pipe(Pipe1); // Creamos nuestra Pipe1.
    int Pipe2[2]; pipe(Pipe2); // Creamos nuestra Pipe2.

    for (int i=0; i<NPROCESSES; i++)
    {
        if (fork() == 0)
        {
            if (i%2 == 0)
            {
                close(Pipe1[1]); close(Pipe2[0]);
                read(Pipe1[0], &K, sizeof(K));
                K++;
                write(Pipe2[1], &K, sizeof(K));
            }
            else
            {
                close(Pipe1[0]); close(Pipe2[1]);
                read(Pipe2[0], &K, sizeof(K));
                K++;
                write(Pipe1[1], &K, sizeof(K));
            }
            printf("Proces %d | K = %d \n", i, K);
            exit(0);
        }
    }

    close(Pipe1[0]); // Cerramos el canal de lectura.
    write(Pipe1[1], &K, sizeof(K)); // Escribimos el número aleatorio en la Pipe.

    for (int i=0; i<NPROCESSES; i++) wait(NULL); // Esperamos que todos los 10 procesos terminen.

    return 0;
}
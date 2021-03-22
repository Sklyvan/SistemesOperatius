#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 10 // Número de threads y tamaño del array.
#define N 5 // Este es el número que añadimos a nuestro array.

/*
 *
Sistemas Operativos: S202
Joan Gracia Taribó: 219176
Josep Tuñí Domínguez: 216737
 *
*/

int myArray[NTHREADS+1];
pthread_mutex_t Lock;
pthread_cond_t isReady;

void printArray(){ // Simplemente recorremos el array y lo imprimimos al estilo Python.
    printf("[");
    for (int i=1; i<NTHREADS+1; i++){
        if (i == NTHREADS) printf("%d", myArray[i]);
        else printf("%d, ", myArray[i]);
    }
    printf("]\n");
}

void* InsertIntoArray(void* toIndex){ // Inserta el número N en la posición toIndex.

    pthread_mutex_lock(&Lock);
    while(myArray[(long) toIndex - 1] != N) pthread_cond_wait(&isReady, &Lock); // Mientras el thread anterior no haya terminado, esperamos.

    myArray[(long)toIndex] = myArray[(long) toIndex - 1]; // Insertamos el número.
    printArray();

    pthread_cond_broadcast(&isReady); // Avisamos de que ese thread ya ha terminado.
    pthread_mutex_unlock(&Lock);
    return NULL;
}

int Main1() {
    pthread_mutex_init(&Lock, NULL);
    pthread_cond_init(&isReady, NULL);
    pthread_t threadsID[NTHREADS];

    for (int i=0; i<NTHREADS+1; i++){ // Iniciamos el array, el único elemento que no es cero, es el primero que lo usamos de forma auxiliar.
        if (i==0) myArray[i] = N;
        else myArray[i] = 0;
    }

    for (long i=0; i<NTHREADS; i++){ // Creamos los threads.
        pthread_create(&threadsID[i], NULL, InsertIntoArray, (void*) i+1);
    }

    for(int i=0; i<NTHREADS; i++) { pthread_join(threadsID[i], NULL); } // Esperamos a que terminen.

    printArray();

    return 0;
}

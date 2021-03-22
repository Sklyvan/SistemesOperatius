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

#define NPROCESSES 2
#define ARRAY_SIZE 1000

int main() {
    int KEY = 12345;
    int SHDM_ID = shmget(KEY, sizeof(int)*ARRAY_SIZE, 0666 | IPC_CREAT); // Obtenemos el ID de nuestra Shared Memory region. (Espacio para 100 enteros)
    int* SHDM_P = shmat(SHDM_ID, NULL, 0); int* Array = SHDM_P; // Obtenemos el puntero de la primera posición del Array de 100 enteros.
    sem_t* NamedMutex = sem_open("/NamedMutex", O_CREAT | O_EXCL, 0600, 1);

    for (int i=0; i<NPROCESSES; i++){
        int ID = fork(); // Creamos un proceso hijo.
        if (ID == 0 && i == 0){ // Child 1
            SHDM_P = shmat(SHDM_ID, NULL, 0); // Obtenemos el puntero a la primera posición de nuestro array de 100 enteros.
            sem_wait(NamedMutex); // Hasta que no se haya iniciado el array completamente a 1, no se puede hacer nada que no sea eso.
            for (int j=0; j<ARRAY_SIZE; j++) SHDM_P[j] = 1; // Initialize Array to 1.
            sem_post(NamedMutex); // Ya hemos inicado el array a 1, ya podemos hacer otras cositas.

            for (int j=0; j<ARRAY_SIZE; j++) {
                sem_wait(NamedMutex);
                SHDM_P[j] = SHDM_P[j]*j; // Multiply entry by index.
                sem_post(NamedMutex);
            }

            exit(0);
        }
        else if (ID == 0 && i == 1){ // Child 2
            SHDM_P = shmat(SHDM_ID, NULL, 0); // Obtenemos el puntero a la primera posición de nuestro array de 100 enteros.

            for (int j=0; j<ARRAY_SIZE; j++) {
                sem_wait(NamedMutex);
                SHDM_P[j] = SHDM_P[j]*j; // Multiply entry by index.
                sem_post(NamedMutex);
            }

            exit(0);
        }

    }

    while (wait(NULL) > 0); // Esperamos a que terminen los dos procesos.
    sem_close(NamedMutex); sem_unlink("NamedMutex");

    for (int i=0; i<ARRAY_SIZE; i++) if (Array[i] != i*i) printf("ERROR! ---> Array[%d] = %d | %d² = %d\n", i, Array[i], i, i*i);



    return 0;
}

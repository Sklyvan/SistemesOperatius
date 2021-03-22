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

bool isPrime(int n)
{
    for (int i=2; i<n; i++){ if (n%i == 0) return false; }
    return true;
}

int main()
{
    int Pipe1[2]; pipe(Pipe1);
    int Pipe2[2]; pipe(Pipe2);
    int K = 10;
    printf("Number: %d \n", K);

    for (int i=0; i<2; i++) // Creamos dos procesos.
    {
        if (fork() == 0 && i == 0) // Child Process A
        {
            waitpid(getppid(), NULL, 0); // Esperamos a que el padre genere un número primo y nos lo mande.

            close(Pipe1[1]); // Cerramos el canal de escritura.
            close(Pipe2[0]); // Cerramos el canal de lectura.

            int K1;
            read(Pipe1[0], &K1, sizeof(int)); // Leemos el número que ha generado el Proceso Padre.
            write(Pipe2[1], &K1, sizeof(int)); // Escribimos el número que nos ha mandado el Proceso Padre, al Proceso B.

            if(isPrime(K1))
            {
                printf("Process A: %d Is Prime.\n", K1);
            }
            else
            {
                printf("Process A: %d Not Prime. \n", K1);
            }

            exit(0);
        }
        else if (fork() == 0 && i == 1) // Child Process B
        {
            waitpid(0, NULL, 0);
            close(Pipe2[1]); // Cerramos el canal de escritura.
            int K2; read(Pipe2[0], &K2, sizeof(int)); // Leemos el número que nos ha mandado el Proceso A.

            if(isPrime(K2))
            {
                printf("Process B: %d Is Prime.\n", K2);
            }
            else
            {
                printf("Process B: %d Not Prime. \n", K2);
            }

            exit(0);
        }
    }

    close(Pipe1[0]); // Cerramos el canal de lectura.
    write(Pipe1[1], &K, sizeof(int)); // Mandamos un número a la Pipe1.

    for (int i=0; i<2; i++) wait(NULL);

    return 0;
}

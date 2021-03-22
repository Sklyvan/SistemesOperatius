#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define NPROCESS 10
#define NNUMBERS 10

/*
 *
Sistemas Operativos: S202
Joan Gracia Taribó: 219176
Josep Tuñí Domínguez: 216737
 *
*/

int main() {
    int fd[2]; int num; // Declaramos el Pipe y la variable donde iremos extrayendo el número de nuestro PIPE.
    pipe(fd); // Creamos el PIPE.
    for (int i=0; i<NPROCESS; i++){ // Vamos a crear 10 procesos.
        int ID = fork();
        if (ID == 0){ // Si estamos en un proceso hijo.
            close(fd[0]); // Cerramos el canal de lectura.
            for (int j=0; j<NNUMBERS; j++) {
                int n = i+1; // El proceso 0 escribe 10 veces el número 1.
                write(fd[1], &n, sizeof(n)); // Cada proceso escribe 10 veces su n.
            }
            exit(0); // Matamos al proceso hijo.
        }
    }
    // Esto ya lo ejecuta el proceso padre.

    for (int i=0; i<NPROCESS; i++) wait(NULL); // Esperamos a todos los procesos a que terminen.

    close(fd[1]); // Cerramos el canal de escritura.
    int totalSum = 0; // Aquí guardaremos la suma total.
    for (int i=0; i<NPROCESS*NNUMBERS; i++){ // Leemos el contenido del PIPE.
        read(fd[0], &num, sizeof(num)); // Leemos y guardamos en la variable num.
        totalSum += num; // Añadimos a la suma total.
    }
    /*
    Como cada proceso añade su número 10 veces, el proceso 1 añade 10*1 = 10.
    El proceo 2 añade 10*2 = 10 y así hasta el proceso 10 que añade 10*9 = 90.
    Finalmente la suma es: 10 + 20 + 30 + 40 + 50 + 60 + 70 + 80 + 90 + 100 = 550.
    */
    printf("Final Sum: %d\n", totalSum);
    return 0;
}
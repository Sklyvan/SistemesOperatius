#include <stdio.h>       /* printf, sprintf */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */
#include <sys/types.h>


int main(int argc, char *argv[]){
    int fd[2]; int num; // Declaramos el Pipe y la variable donde iremos extrayendo el número de nuestro PIPE.
    pipe(fd); // Creamos el PIPE.

    for (int i=0; i<10; i++) {
        write(fd[1], &i, sizeof(i)); // Escribimos el ID en el Pipe.
        if(fork() == 0) {
            close(fd[1]); // En cada hijo, cerramos el canal de escritura.
            usleep(10000+rand()%10000);
            read(fd[0], &num, sizeof(num)); // Leemos el ID del Pipe.
            printf("I'm process %d\n", num);
            _exit(0);
        }
    }
    close(fd[0]); // En el padre, cerramos el canal de lectura.

    while (wait(NULL) > 0);
    return 0;
}


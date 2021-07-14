#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <pthread.h>

#define N 10

// Joan Gracia Taribó - 219176

int SOLUTION = 0;
int nTimes;
int a[N]; 
int times[N];

int mult2(int num) {return 2*num; }

int main(int argc, char *argv[])
{
	if(argc > 1) SOLUTION = atoi(argv[1]);
  
  if (SOLUTION == 0) return -1;

  else if (SOLUTION == 1) {
    nTimes = 1;
    for (int i=0; i<N; i++) { a[i] = 1; times[i] = 0; }
    if (fork() == 0){
      for (int i=0; i<N; i++){
          for (int j=0; j<nTimes; j++) {
            a[i] = mult2(a[i]);
            times[i]++;
        }
      }
      for (int i=0; i<N; i++) printf("Array[%d] = %d \n", i, a[i]);
      exit(0); 
    }
    wait(NULL);
    return 0;
  }

  else if (SOLUTION == 2) {
    for (int i=0; i<N; i++) { a[i] = 1; times[i] = 0; }
    int Pipe1[2]; int Pipe2[2];
    pipe(Pipe1); pipe(Pipe2);

    if (fork() == 0){
      close(Pipe1[1]); // Cerramos el canal de escritura de la Pipe1.
      close(Pipe2[0]); // Cerramos el canal de lectura de la Pipe2.
      for (int i=0; i<N; i++){
          int CurrentElement; int n = 1;
          read(Pipe1[0], &CurrentElement, sizeof(CurrentElement)); // Leemos el elemento de la Pipe1.
          CurrentElement = mult2(CurrentElement);
          write(Pipe2[1], &CurrentElement, sizeof(CurrentElement)); // Escribimos el elemento procesado en la Pipe2.
          write(Pipe2[1], &n, sizeof(n)); // Escribimos las veces que se ha procesado en la Pipe2.
      }
      exit(0); 
    }

    close(Pipe1[0]); // Cerramos el canal de lectura.
    for (int i=0; i<N; i++){
      int K = a[i];
      write(Pipe1[1], &K, sizeof(K));
    }

    wait(NULL); // Esperamos al hijo.

    close(Pipe2[1]); // Cerramos el canal de escritura.
    for (int i=0; i<N; i++){
      int K1; int K2;
      read(Pipe2[0], &K1, sizeof(K1)); read(Pipe2[0], &K2, sizeof(K2));
      a[i] = K1; times[i] = K2;
      printf("a[%d] = %d | ", i, a[i]);
      printf("times[%d] = %d \n", i, times[i]);
    }
    return 0;
  }

  else if (SOLUTION == 3){
    printf("No me ha dado tiempo :( \n");
    return 0;
  }

}

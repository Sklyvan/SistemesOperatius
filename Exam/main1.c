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

// Joan Gracia Taribó: 219176

int SOLUTION = 0;
int nTimes;
int a[N]; 
int times[N];

int mult2(int num) {return 2*num; }

void* ThreadFunction(void* toIndex){
  int i = (long) toIndex;
  for (int k=0; k<nTimes; k++) {
    a[i] = mult2(a[i]);
    times[i]++;
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  if(argc > 1) SOLUTION = atoi(argv[1]);
  
  if (SOLUTION == 0) return -1;
  
  else if (SOLUTION == 1){
    // En este caso, no será necesaria la sincronización, ya que cada thread accede a una
    // región de memoria distina, por lo que no tendremos Race Condition.
    pthread_t threadsID[N];
    nTimes = 1;
    for (int i=0; i<N; i++) { a[i] = 1; times[i] = 0; }
    for (long i=0; i<N; i++) { pthread_create(&threadsID[i], NULL, ThreadFunction, (void*) i); }

    for (int i=0; i<N; i++) pthread_join(threadsID[i], NULL);

    for (int i=0; i<N; i++) printf("Array[%d] = %d \n", i, a[i]);
    
    return 0;
  }

    else if (SOLUTION == 2){
    // En este caso, no será necesaria la sincronización, ya que cada thread accede a una
    // región de memoria distina, por lo que no tendremos Race Condition.
    pthread_t threadsID[N];
    nTimes = 2;
    for (int i=0; i<N; i++) { a[i] = 1; times[i] = 0; }
    for (long i=0; i<N; i++) { pthread_create(&threadsID[i], NULL, ThreadFunction, (void*) i); }

    for (int i=0; i<N; i++) pthread_join(threadsID[i], NULL);

    for (int i=0; i<N; i++) printf("Array[%d] = %d \n", i, a[i]);
    
    return 0;
  }

    else if (SOLUTION == 3){
    // En este caso, si que será necesaria la sincronización, ya que a cada posición del array
    // van a acceder dos threads distintos, y podríamos tener Race Condition.
    pthread_t threadsID[2*N];
    nTimes = 1;
    for (int i=0; i<N; i++) { a[i] = 1; times[i] = 0; }
    for (long i=0; i<2*N; i++) { pthread_create(&threadsID[i], NULL, ThreadFunction, (void*) (i%N)); }

    for (int i=0; i<2*N; i++) pthread_join(threadsID[i], NULL);

    for (int i=0; i<N; i++) printf("Array[%d] = %d \n", i, a[i]);
    
    return 0;
  }

    else if (SOLUTION == 4){
    // En este caso, si que será necesaria la sincronización, ya que a cada posición del array
    // van a acceder dos threads distintos, y podríamos tener Race Condition.
    pthread_t threadsID[2*N];
    nTimes = 3;
    for (int i=0; i<N; i++) { a[i] = 1; times[i] = 0; }
    for (long i=0; i<2*N; i++) { 
      if (i >= N) nTimes = 2;
      pthread_create(&threadsID[i], NULL, ThreadFunction, (void*) (i%N)); 
      }

    for (int i=0; i<2*N; i++) pthread_join(threadsID[i], NULL);

    for (int i=0; i<N; i++) printf("Array[%d] = %d \n", i, a[i]);
    
    return 0;
  }


}

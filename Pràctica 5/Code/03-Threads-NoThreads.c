#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep function : waits for seconds
#include <time.h>    // for usleep : waits for microseconds

#define NTHREADS 10


void* fsleep(void * arg) {
  usleep(1);
  int id = * (int*) arg;
  printf("fsleep thread function ID: %d\n", id);
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t tids[NTHREADS];
  srand(time(NULL));

  for(int i=0; i<NTHREADS; i++) {
    printf("Creating thread %d\n", i);
    fsleep(&i);
  }

}

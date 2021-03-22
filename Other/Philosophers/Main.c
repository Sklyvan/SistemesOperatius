#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <stdbool.h>

#define NPHILOSOPHERS 5
#define NFORKS 5

int Eating = 0;
sem_t Forks[NFORKS];
char* PhilosophersNames[NPHILOSOPHERS] = {"Plato", "Nietzsche", "Aristotle", "Kant", "Voltaire"};

void Think(int ID) { printf("Philosopher %s Thinking!\n", PhilosophersNames[ID]); sleep(1); }
void Eat(int ID) { printf("Philosopher %s Eating!\n", PhilosophersNames[ID]); sleep(1); }

void* ThreadFunction(void* ID){
    int PhilosopherID = (long) ID;
    while(true){
        Think(PhilosopherID);
        if (PhilosopherID+1 < 5){
            sem_wait(&Forks[PhilosopherID]);
            sem_wait(&Forks[PhilosopherID+1]);
        }
        else{
            sem_wait(&Forks[(PhilosopherID+1)%5]);
            sem_wait(&Forks[PhilosopherID]);
        }

        Eat(PhilosopherID);

        if (PhilosopherID+1 < 5){
            sem_post(&Forks[PhilosopherID]);
            sem_post(&Forks[PhilosopherID+1]);
        }
        else{
            sem_post(&Forks[(PhilosopherID+1)%5]);
            sem_post(&Forks[PhilosopherID]);
        }
    }
    return NULL;
}

int main() {
    for (int i=0; i<NFORKS; i++) sem_init(&Forks[i], 0, 1);
    pthread_t threadsID[NPHILOSOPHERS];

    for (long i=0; i<NPHILOSOPHERS; i++) pthread_create(&threadsID[i], NULL, ThreadFunction, (void*) i);
    for (int i=0; i<NPHILOSOPHERS; i++) pthread_join(threadsID[i], NULL);

    return 0;
}

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 2
#define N 25 // Capacidad de nuestro buffer.

int BUFFER[25]; // Un 1 representa posición llena y un 0 representa que está vacía.
pthread_mutex_t Lock;
pthread_cond_t emptySpace; // Condición que indica si tenemos algun espacio libre que llenar.
pthread_cond_t thereIsItem; // Condición que indica si tenemos algun elemento que consumir.
int emptyElements = N; // Número actual de posiciones llenas.

void printBuffer(){ // Esto imprime nuestro buffer al estilo Python.
    printf("[");
    for (int i=0; i<N; i++){
        if (i == N-1) printf("%d", BUFFER[i]);
        else printf("%d, ", BUFFER[i]);
    }
    printf("]\n");
}

void* CreateItem(){ // Busca la primera posición vacía y le añade un elemento.
    for (int i=0; i<N; i++){
        if (BUFFER[i] == 0) {
            BUFFER[i] = 1; emptyElements--;
            return NULL;
        }
    }
    write(2, "System Overload! \n", sizeof("System Overload! \n")); // Si no ha encontrado ninguna posición vacía da error.
    return NULL;
}

void* ConsumeItem(){ // Busca si hay alguna posición llena y la consume.
    for (int i=0; i<N; i++){
        if (BUFFER[i] == 1){
            BUFFER[i] = 0; emptyElements++;
            return NULL;
        }
    }
    write(2, "Empty Space! \n", sizeof("Empty Space! \n")); // Si no ha encontrado ninguna posición llena da error.
    return NULL;
}

void* ProducerFunction(){
    for (int n=0; n<4*N; n++){
        for (int i=0; i<N; i++){
            pthread_mutex_lock(&Lock);
            while(emptyElements == 0) pthread_cond_wait(&emptySpace, &Lock); // Mientras el buffer se encuentre lleno, esperamos a que haya algun espacio.
            CreateItem(); // Ahora que hay un espacio, ya podemos añadir un elemento.
            pthread_cond_broadcast(&thereIsItem); // Avisamos de que ya hay un elemento.
            pthread_mutex_unlock(&Lock);
        }
    }
    return NULL;
}
void* ConsumerFunction(){
    for (int n=0; n<4*N; n++){
        for (int i=0; i<N; i++){
            pthread_mutex_lock(&Lock);
            while(emptyElements == N) pthread_cond_wait(&thereIsItem, &Lock); // Mientras se encuentre vacío, esperamos que se genere algún elemento.
            ConsumeItem(); // Ahora que ya hay almenos un elemento, lo vamos a consumir.
            pthread_cond_signal(&emptySpace); // Avisamos de que ya hay un espacio libre.
            pthread_mutex_unlock(&Lock);
        }
    }
    return NULL;
}

int main() {
    pthread_t threadsID[NTHREADS];
    for (int i=0; i<N; i++) BUFFER[i] = 0;
    pthread_mutex_init(&Lock, NULL);
    pthread_cond_init(&thereIsItem, NULL); pthread_cond_init(&emptySpace, NULL);
    printf("Initial Buffer ---> "); printBuffer();

    // Creamos al Producer y al Consumer.
    pthread_create(&threadsID[0], NULL, ProducerFunction, NULL);
    pthread_create(&threadsID[1], NULL, ConsumerFunction, NULL);

    // Esperamos que terminen los dos threads.
    pthread_join(threadsID[0], NULL);
    pthread_join(threadsID[1], NULL);

    printf("Final Buffer ---> "); printBuffer();
    return 0;
}

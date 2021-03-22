#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/*
 *
 * Sistemas Operativos: S202
Joan Gracia Taribó: 219176
Josep Tuñí Domínguez: 216737
 *
*/

int arraySize = 100; // Tamaño del array.
int Array[100];
pthread_mutex_t lock; // Esta variable global nos va a servir para hacer un lock de nuestro thread antes de operar sobre el array.
void* multiplyByIndex(void* i) { // La función debe retornar un puntero a void y recibir como argumento un puntero a void.
    pthread_mutex_lock(&lock); // Bloquamos el thread para evitar problemas de Race Condition.
    Array[(long)i] = Array[(long)i] * (long)i; // Como realmente el puntero que recibe apunta a un entero, podemos hacer un cast para usar su valor.
    pthread_mutex_unlock(&lock); // Ya hemos hecho la operación que podia causar problemas, desbloqueamos el thread.
    return NULL; // Es necesario esto ya que hemos dicho que retorna un puntero a void.
}

int main() {
    for (int i=0; i < arraySize; i++) Array[i] = 1; // Inicializamos el array con todos sus elementos igual 1.
    pthread_t threadsID[2*arraySize]; // Lista de los IDs de nuestro threads, en total son 200.
    for(long i=0; i<2*arraySize; i++){ // Vamos a recorrer cada uno de nuestro threads.
        // Como tenemos el doble de threads que posiciones del array, a partir de la i=100, el módulo hará que el valor máximo sea 99.
        // En un caso donde la i=100, nuestro Array no tiene posición 100, pero como se hace i%arraySize, se irá a la posición 0 del array.
        // De esta forma, se vuelve a  recorrer el array una segunda vez.
        pthread_create(&threadsID[i], NULL, multiplyByIndex, (void *) (i%arraySize)); // Creamos el thread correspondiente.
    }
    for (int i=0; i < arraySize; i++){ pthread_join(threadsID[i], NULL); } // Esperamos a cada thread.

    for (int i=0; i<arraySize; i++){ // Esto simplemente lo usamos para ver que nuestro programa funciona y que no existen casos de Array[i] != i².
        if (Array[i] != i*i) printf("i = %d, A[%d] = %d\n", i, i, Array[i]);
    }

    return 0;
}
/*
Hemos necesitado una sola variable global para gestionar nuestros locks. Esto es necesario debido a que la instrucción Array[i] = Array[i] * i no es atómica.
Su traducción en ensamblador, implica distintas operaciones que pueden ser interrumpidas si a nuestro thread se le acaba el tiempo asignado por el Sistema Operativo.
Esto puede generar que otro thread acceda a esa misma posicón del array donde otro thread ha sido interrumpido y que no tengamos i² en esa posición.
Para evitar esto, hemos usado el lock, que va a hacer que un thread no se pueda ejecutar hasta que el anterior haya terminado y desbloquee el aceso.
Al iniciar el acceso al array, el thread activa el lock y al terminar lo desbloquea.
*/

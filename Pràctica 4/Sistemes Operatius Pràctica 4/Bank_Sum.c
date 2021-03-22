#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "myutils.h"

#define FILENAME "Bank.dat"
#define NACCOUNTS 10
int SOLUTION = 0;
sem_t* NamedMutex;

typedef struct account { int ID; double Balance; } Account;

off_t fsize(const char *filename) {
    // Esta función la usamos para abrir el archivo en función de tu tamaño,
    // ya que en los casos donde teníamos Race Condition, ocurría que podíamos escribir más de NACCOUNTS cuentas.
    // De esta forma leemos el archivo entero.
    struct stat st;
    if (stat(filename, &st) == 0) return st.st_size; // Tamaño del archivo.
    return -1; // El archivo no existe.
}

bool checkSum(char* fileName){
    int fbank = open(fileName,  O_RDONLY, 0640); // Abrimos el archivo en modo lectura.
    // Si estamos en la Solución 1, hacemos lock de el archivo entero, ya que vamos a leerlo entero.
    if (SOLUTION == 1) file_lock(fbank, 0, sizeof(Account)*NACCOUNTS);
    else if (SOLUTION == 2) sem_wait(NamedMutex); // En la Solución 2, usámos un semáforo.
    float totalSum = 0;
    Account CurrentAccount;
    for (int i=0; i<fsize(FILENAME)/sizeof(Account); i++){ // Recorremos el archivo cuenta a cuenta.
        read(fbank, &CurrentAccount, sizeof(Account));
        totalSum += CurrentAccount.Balance; // Acumulamos la cantidad de esa cuenta.
    }
    if (SOLUTION == 1) file_unlock(fbank, 0, sizeof(Account)*NACCOUNTS); // Desbloqueamos el archivo entero.
    else if (SOLUTION == 2) sem_post(NamedMutex); // Volvemos a dejar pasar en nuestro semáforo.
    printf("CheckSum: %d | ", (int) totalSum);
    return totalSum == 1000; // Esto nos retorna si hay algun error.
}

int main(int argc, char* argv[]){
    if (argc > 1) SOLUTION = atoi(argv[1]);
    if (SOLUTION == 1 || SOLUTION == 2) { // Si es una solución posible.
        if (SOLUTION == 2) NamedMutex = sem_open("/NamedMutex", O_CREAT, 0644, 1); // El flag de O_CREAT, crea el semáforo solo si no existe.
        while (true) { // Ejecutamos de forma infinita este bucle.
            if (checkSum(FILENAME)) {
                printf("No Errors.\n");
            } else {
                printf("Error!\n");
            }
            usleep(500000); // Esperamos 500000 microsegundos.
        }
        return 0; // En teoria el bucle solo acaba si el usuario mata el proceso.
    }
    return -1; // Si se llama al programa sin solución o una solución que no sea la 1 o la 2.
}
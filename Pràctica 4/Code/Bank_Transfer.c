#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "myutils.h"

#define NOPERATIONS 50
#define FILENAME "Bank.dat"
#define NACCOUNTS 10
#define NPROCESS 200
#define INITIAL_ACCOUNT_MONEY 100

int SOLUTION = 0; // La Solution 1 es usando FileLocks y la Solution 2 es con Semáforos.
int fbank; // Archivo Bank.dat

typedef struct account { int ID; double Balance; } Account; // Estructura de nuestra cuenta bancaria.

sem_t* NamedMutex;

void doSomething() { for (int i=0; i < 100000; i++); }

void bankInit(){ // Esta función crea NACCOUNTS cuentas y las escribe en nuestro archivo Bank.dat
    for(int i=0; i<NACCOUNTS; i++) {
        Account CurrentAccount = {.ID = i, .Balance = INITIAL_ACCOUNT_MONEY}; // Iniciamos la cuenta.
        write(fbank, &CurrentAccount, sizeof(Account)); // Escribimos nuestra cuenta en el Bank.dat
    }
}

void fileWithdraw(int accountID, double amount){
    // Esta función retira la cantidad a la cuenta con ese ID.
    Account tempAccount; // Aquí guardaremos la cuenta temporalmente mientras se encuentre fuera de nuestro archivo y la manipulemos.

    // Movemos el puntero al inicio de nuestra cuenta que hace el pago, y leemos la información de la cuenta en el archivo.
    lseek(fbank, accountID * sizeof(Account), SEEK_SET); read(fbank, &tempAccount, sizeof(Account));

    tempAccount.Balance -= amount; // Extraemos esa cantidad de la cuenta.

    // Movemos el puntero a la zona donde vamos a escribir la cuenta, y escribimos la cuenta con los nuevos cambios.
    lseek(fbank, accountID * sizeof(Account), SEEK_SET); write(fbank, &tempAccount, sizeof(Account));
}

void fileDeposit(int accountID, double amount){
    // Esta función añade la cantidad a la cuenta con ese ID.
    Account tempAccount; // Aquí guardaremos la cuenta temporalmente mientras se encuentre fuera de nuestro archivo y la manipulemos.

    // Movemos el puntero al inicio de nuestra cuenta que recibe el pago, y leemos la información de la cuenta en el archivo.
    lseek(fbank, accountID * sizeof(Account), SEEK_SET); read(fbank, &tempAccount, sizeof(Account));

    tempAccount.Balance += amount; // Añadimos esa cantidad de la cuenta.

    // Movemos el puntero a la zona donde vamos a escribir la cuenta, y escribimos la cuenta con los nuevos cambios.
    lseek(fbank, accountID * sizeof(Account), SEEK_SET); write(fbank, &tempAccount, sizeof(Account));
}

bool check(Account* acc, double amount) { return acc->Balance - amount >= 0; } // Se comprueba si la cuenta tiene dinero suficiente.

bool fileCheck(int accountID, double amount){
    // Va al archivo, lee la cuenta y nos dice si esa cuenta tiene sufiente dinero.
    Account tempAccount;
    lseek(fbank, accountID * sizeof(Account), SEEK_SET); read(fbank, &tempAccount, sizeof(Account));
    return check(&tempAccount, amount);
}

bool withdraw(int accountID, double amount) {
    // Añade dinero a una cuenta.
    if(fileCheck(accountID, amount))  {
        fileWithdraw(accountID, amount);
        doSomething();
        return true;
    }
    return false;
}

void deposit(int accountID, double amount) {
    // Retira dinero a una cuenta.
    fileDeposit(accountID, amount);
    doSomething();
}

bool transfer1(int fromID, int toID, double amount){
    // Solución usando FileLocks.

    int Lock1, Lock2; // Con esto vamos a evitar posibles Deadlocks.
    if (fromID > toID) {Lock1 = toID * sizeof(Account); Lock2 = fromID * sizeof(Account); }
    else{ Lock1 = fromID * sizeof(Account), Lock2 = toID * sizeof(Account); }

    file_lock(fbank, Lock1, sizeof(Account)); file_lock(fbank, Lock2, sizeof(Account));

    // En este momento, ya hemos hecho el lock de la zona determinada de nuestro archivo.
    // Así que ya podemos hacer la operación de sumar y restar dinero.
    bool isDone = withdraw(fromID, amount);
    if (isDone) { deposit(toID, amount); }

    file_unlock(fbank, Lock1, sizeof(Account)); file_unlock(fbank, Lock2, sizeof(Account));

    return isDone;
}

bool transfer2(int fromID, int toID, double amount){
    // Solución usando semáforos.

    sem_wait(NamedMutex);

    bool isDone = withdraw(fromID, amount);
    if (isDone) { deposit(toID, amount); }

    sem_post(NamedMutex);

    return isDone;s
}

void tranferProcess(){
    fbank = open("Bank.dat",  O_RDWR, 0600); // Cada hijo debe volver a abrir el archivo.
    int iter = NOPERATIONS/NACCOUNTS;
    int fromID = -1, toID = -1;
    while(iter--) {
        fromID = -1; toID = -1;
        while(fromID == toID) { // Si hemos seleccionado aleatoriamente las mismas cuentas, repetimos el proceso.
            fromID = rand()%NACCOUNTS; toID = rand()%NACCOUNTS;
        }

        double randomAmount = rand()%50+1;
        printf("ID: %d --- %d€ ---> ID: %d \n", fromID, (int) randomAmount, toID);
        // Dependiendo de la solución, hacemos la transferencia de una forma u otra.
        if (SOLUTION == 1) transfer1(fromID, toID, randomAmount);
        else if (SOLUTION == 2) transfer2(fromID, toID, randomAmount);

    }
}

int main(int argc, char* argv[]){
    if (argc > 1) SOLUTION = atoi(argv[1]); // Extraemos nuestra solución, la transformamos de char a entero.
    startTimer(0); // Iniciamos el cronómetro para saber el tiempo de nuestra ejecución.
    if (SOLUTION == 1 || SOLUTION == 2){ // Si se ha introducido una solución correcta.
        // Eliminamos si hay una copia antigua del archivo, ya que podria contener información de una ejecución con Race Condition.
        remove(FILENAME); fbank = open(FILENAME, O_CREAT | O_RDWR, 0600); // Creamos y Abrimos el archivo.
        if (SOLUTION == 2) NamedMutex = sem_open("/NamedMutex", O_CREAT, 0644, 1); // Si la solución es la de los Semáforos, iniciamos nuestro semáforo.
        bankInit(); // Iniciamos las cuentas del banco.
        printf("----> Initial Bank Total Money: %d€ <----\n", INITIAL_ACCOUNT_MONEY); // Imprimimos el dinero total del banco.

        for (int i=0; i < NPROCESS; i++){ // Creamos los procesos.
            int PID = fork();
            if (PID == 0){ // Esto solo lo ejecutan los hijos.
                tranferProcess(); // Hacemos las transferencias.
                exit(0); // Matamos al proceso para evitar que siga su ejecución.
            }
        }
        while(wait(NULL) > 0); // Desde el padre, esperamos a que terminen todos los hijos.

        if (SOLUTION == 2) sem_close(NamedMutex); // Cerramos el semáforo si ya hemos terminado y estamos en la solución que lo ha iniciado.
        close(fbank); // Cerramos el archivo.

        printf("Time: %ld ms\n", endTimer(0)); // Imprimimos el tiempo total.
        return 0;
    }
    else return -1; // Si se ha producido algún error.
}

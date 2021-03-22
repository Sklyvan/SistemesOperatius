#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "myutils.h"

int file; //con esta variable accederemos al archivo bank.dat

int SOLUTION = 0;//Variable para cambiar el tipo de sincronización de procesos

char* str_id[3] = {"NO_SYNCH", "FILE_LOCK", "NAMED_SEMAPHORE_LOCK"};

#define N_PROCESS 8 //Num of processes (Tiene que ser potencia de 2)
#define N 10 //Numero de cuentas en el banco

//NAMED SEMAPHORES//
sem_t* named_mutex;//definición del semáforo


typedef struct struct_account { //estructura de la cuenta
    int id;
    int account_type;
    double balance;
} account;

typedef struct struct_sync_account { //estructura de la sincronización de la cuenta
    account a;
    monitor_lock m;
} account_sync;

account_sync bank[N]; //Array de cuentas del banco

account auxBank;


void do_something() {
    int n = 20;
    int sum = 0;
}

void withdrawInFile(int acc, double amount) {

    int point = acc * sizeof(account);
    lseek(file, point, SEEK_SET);
    read(file, &auxBank, sizeof(account));

    auxBank.balance-=amount;

    lseek(file, point, SEEK_SET);
    write(file, &auxBank, sizeof(account));

}

void depositInFile(int acc, double amount) {

    int point = acc * sizeof(account);
    lseek(file, point, SEEK_SET);
    read(file, &auxBank, sizeof(account));

    auxBank.balance += amount;

    lseek(file, point, SEEK_SET);
    write(file, &auxBank, sizeof(account));

}


bool check(int acc, double amount) { //chequeo de tener dinero en la cuenta
    int point = acc * sizeof(account);
    lseek(file, point, SEEK_SET);
    read(file, &auxBank, sizeof(account));
    //printf("%.2f - %.2f = %d\n", auxBank.balance, amount, auxBank.balance-amount >= 0);
    return auxBank.balance - amount >= 0;
}

bool withdraw(int acc, double amount) { //intercambio de dinero si hay el suficiente
    if(check(acc,amount))  {
        withdrawInFile(acc, amount);
        do_something();
        return true;
    }
    return false;
}

void deposit(int acc, double amount) { //depósito de dinero
    depositInFile(acc, amount);
    do_something();
}

bool transfer(int from, int to, double amount) {
    int aux, p1, p2;

//se hace la transferencia

    //Bloqueamos las zonas del fichero que contienen las cuentas implicadas en la operación
    p1 = from * sizeof(account);
    p2 = to * sizeof(account);

    if (SOLUTION == 1){ //SINCRONIZACIÓN CON FILE_LOCKS
        if(from > to){p1 = to * sizeof(account); p2 = from * sizeof(account);}
        while(file_lock(file, p1, sizeof(account)) < 0);
        while(file_lock(file, p2, sizeof(account)) < 0);
    }else if (SOLUTION == 2){
        if(from > to){p1 = to * sizeof(account); p2 = from * sizeof(account);}
        sem_wait(named_mutex);
    }

    //Ejecutamos la operación de extracción de dinero
    bool bDone = withdraw(from, amount);

    //Si la operación withdraw ha sido exitosa, tendremos un TRUE (1) en bDone por lo que se efectuará el ingreso en la cuenta de destino
    if(bDone){
        deposit(to,amount);
        //printf("%d Transfered %.2f from %d to %d\n", getpid(), amount, from, to);
    }

    if (SOLUTION == 1){
        file_unlock(file, p1, sizeof(account));
        file_unlock(file, p2, sizeof(account));
    }else if (SOLUTION == 2){
        sem_post(named_mutex);

    }
    // end of transfer operation
    // ******************************************

    return bDone;
}


void transferProcess() { //hace la transferencia

    file = open("bank.dat",  O_RDWR, 0600); //abrimos el archivo bank.dat

    int from = -1;
    int to = -1;
    int iter = 10000/N_PROCESS; //10.000 transacciones entre el número de procesos

    srand(time(NULL));
    while(iter--) {
        from = -1;
        to = -1;
        while(from == to) {
            from = rand()%N;
            to = rand()%N;
        }
        //printf("%d Transfer from %d to %d\n", getpid(), from, to);

        transfer(from, to, rand()%50+1);

    }

    close(file);
}


int main(int argc, char *argv[]) {
    int pid, point, father;
    double sum = 0;

    if(argc > 1) SOLUTION = atoi(argv[1]); //pasamos de string a int el argumento dado

    if(SOLUTION == 1){
        file = open("bank.dat",  O_RDWR, 0600); //abrimos el archivo bank.dat

        pid = fork();
        startTimer(0);
        //El hijo generará los procesos para ejecutar las transferencias
        if(pid == 0){
            //For que genera N_PROCESS subprocesos hijos que llaman a la función transferProcess, por lo que habrán N_PROCESS subprocesos generando transferencias
            for(int i = 0; i < N_PROCESS; i++){ //Creamos tantos procesos como N_PROCESS
                pid = fork();
                if(pid == 0){
                    transferProcess();
                    _exit(0);
                }
            }
            //Una vez generados los subprocesos los esperamos
            while (wait(NULL) > 0);
            _exit(0);//Finalizamos el proceso que genera los subprocesos
        }

        //El primer proceso padre espera al proceso generador de subprocesos
        int stat;
        while (wait(&stat) > 0){
            //printf("Exit status: %d\n", WEXITSTATUS(stat));
        }

        point = 0;
        while(file_lock(file, point, sizeof(file)) < 0);
        for(int i=0;i<N;i++) {
            point = i * sizeof(account);
            lseek(file, point, SEEK_SET);
            read(file, &auxBank, sizeof(account));

            sum += auxBank.balance;
            printf("%d Account %d %.2f\n", getpid(), auxBank.id, auxBank.balance);

            file_unlock(file, point, sizeof(file));
        }
        printf("Done: %ld ms\n",endTimer(0));


        close(file);
    }

    else if(SOLUTION == 2){

        named_mutex = sem_open("/mutex", O_CREAT, 0644, 1); //Creamos el semaforo

        pid = fork();
        startTimer(0);
        if(pid == 0){

            for(int i = 0; i < N_PROCESS; i++){ //Creamos tantos procesos como N_PROCESS
                pid = fork();
                if(pid == 0){
                    transferProcess();
                    _exit(0);
                }
            }
            while (wait(NULL) > 0);
            _exit(0);//Finalizamos el proceso que genera los subprocesos
        }

        int stat;
        while (wait(&stat) > 0){
            printf("Exit status: %d\n", WEXITSTATUS(stat));
        }

        file = open("bank.dat",  O_RDWR, 0600); //abrimos el archivo bank.dat

        point = 0;


        for(int i=0;i<N;i++) {

            point = i * sizeof(account);
            lseek(file, point, SEEK_SET);
            read(file, &auxBank, sizeof(account));
            sum += auxBank.balance;
            printf("%d Account %d %.2f\n", getpid(), auxBank.id, auxBank.balance);


        }
        printf("Done: %ld ms\n",endTimer(0));
        sem_close(named_mutex);
    }

    if(sum != 1000) { printf("ERROR : ******** CORRUPT BANK!!!!!!  sum: %.2lf *******\n",sum);
    }else{ printf("Final bank sum: %f\n",sum);}

    close(file);
    return 0;
}
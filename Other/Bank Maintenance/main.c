#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <stdbool.h>

#define NACCOUNTS 100
#define NOPERATIONS 1000
#define INITIAL_ACCOUNT_MONEY 1000
#define MAINTENANCE_AMOUNT 1
#define NTHREADS 100

typedef struct BankAccount{ int ID; int Balance; bool isBusy; pthread_mutex_t Lock; } Account;
typedef struct BankStruct{ int TotalBalance; Account* Accounts[NACCOUNTS]; } Bank;
pthread_t threadsID[NTHREADS];
Bank MyBank = { .TotalBalance = 0 };
bool inMaintenance; pthread_mutex_t GlobalLock;

int InitializeAccount(Account* CurrentAccount, int toID){
    CurrentAccount->ID = toID;
    CurrentAccount->Balance = INITIAL_ACCOUNT_MONEY;
    CurrentAccount->isBusy = false;
    pthread_mutex_init(&CurrentAccount->Lock, NULL);
    return CurrentAccount->Balance;
}

int InitializeBank(Bank* CurrentBank){
    pthread_mutex_init(&GlobalLock, NULL); // Iniciamos el Global Lock para el mantenimiento.
    for (int i=0; i<NACCOUNTS; i++){
        Account* CurrentAccount = malloc(sizeof(Account)); // La verdad es que uso el malloc demasiado, pero me gusta jeje.
        CurrentBank->TotalBalance += InitializeAccount(CurrentAccount, i);
        CurrentBank->Accounts[i] = CurrentAccount;
    }
    return CurrentBank->TotalBalance;
}

void AddMoney(Account* CurrentAccount, int Amount) { CurrentAccount->Balance += Amount; }
void SubtractMoney(Account* CurrentAccount, int Amount){ CurrentAccount->Balance -= Amount; }

bool Withdraw(Account* fromAccount, Account* toAccount, int Amount){
    if (fromAccount->ID < toAccount->ID){ pthread_mutex_lock(&fromAccount->Lock); pthread_mutex_lock(&toAccount->Lock); }
    else { pthread_mutex_lock(&toAccount->Lock); pthread_mutex_lock(&fromAccount->Lock); }

    if (fromAccount->Balance >= Amount){
        AddMoney(toAccount, Amount);
        SubtractMoney(fromAccount, Amount);

        if (fromAccount->ID < toAccount->ID){ pthread_mutex_unlock(&fromAccount->Lock); pthread_mutex_unlock(&toAccount->Lock); }
        else { pthread_mutex_unlock(&toAccount->Lock); pthread_mutex_unlock(&fromAccount->Lock); }

        return true;
    }
    else {
        if (fromAccount->ID < toAccount->ID){ pthread_mutex_unlock(&fromAccount->Lock); pthread_mutex_unlock(&toAccount->Lock); }
        else { pthread_mutex_unlock(&toAccount->Lock); pthread_mutex_unlock(&fromAccount->Lock); }

        return false;
    }
}

void* ThreadOperations(){
    int fromID = -1, toID = -1;
    int randomEuros;
    for (int i=0; i<NOPERATIONS; i++){
        while(fromID == toID) { fromID = rand()%(NACCOUNTS-1); toID = rand()&(NACCOUNTS-1); } // Generamos dos cuentas aleatoriamente, no paramos hasta que sean distintas.
        randomEuros = rand()%100;

        while(inMaintenance){ printf("Waiting... \n"); }
        Withdraw(MyBank.Accounts[fromID], MyBank.Accounts[toID], randomEuros);

        fromID = -1; toID = -1;
    }
    return NULL;
}

void* Maintenance(){
    inMaintenance = true;

    for (int i=0; i<NACCOUNTS; i++) MyBank.Accounts[i]->Balance -= MAINTENANCE_AMOUNT;

    inMaintenance = false;

    return NULL;
}

int main2() {
    InitializeBank(&MyBank);
    printf("Initial Bank Money: %d€ | ", MyBank.TotalBalance);

    for (int i=0; i<NTHREADS; i++) {
        if (i != 50) pthread_create(&threadsID[i], NULL, ThreadOperations, NULL);
        else pthread_create(&threadsID[i], NULL, Maintenance, NULL);
    }
    for (int i=0; i<NTHREADS; i++) pthread_join(threadsID[i], NULL);

    printf("Final Bank Money: %d€ \n", MyBank.TotalBalance);


    return 0;
}

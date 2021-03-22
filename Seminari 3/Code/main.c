# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>
# define NACCOUNTS 10 // Número de cuentas que contiene nuestro banco.

/*
 *
 * Sistemas Operativos: S202
Joan Gracia Taribó: 219176
Josep Tuñí Domínguez: 216737
 *
*/

pthread_mutex_t GlobalLock; pthread_cond_t CondFree;
bool isBusy = false;

typedef struct Account{
    int ID; float AccountBalance; bool isBusy;
    pthread_mutex_t accountLock; pthread_cond_t accountCond;
} Account;

typedef struct args{ Account* CurrentAccount; float Amount; bool isMaintainance; } args; // Usada para las llamadas con threads.

void initBankAccounts(Account* AccountsList){
    for (int i=0; i<NACCOUNTS; i++){ // Para cada cuenta, inicializamos sus valores.
       Account CurrentAccount = {.ID = i+1, .isBusy = false, .AccountBalance = 100 + abs(rand()/(RAND_MAX/100))};
       pthread_mutex_init(&CurrentAccount.accountLock, NULL);
       AccountsList[i] = CurrentAccount;
    }
}

void MaintainanceLock(){ // Esto va a tener que bloquear todas las cuentas.
    pthread_mutex_lock(&GlobalLock); // Hacemos el lock de nuestro mutex global, ya que hemos entrado en una zona crítica.
    while (isBusy) pthread_cond_wait(&CondFree, &GlobalLock); // Mientras el booleano sea true, hacemos el wait en el monitor.
    isBusy = true; // Cuando salgamos del bucle, es que ya no estamos ocupados, así que podemos estar ocupados con el mantenimiento.
    pthread_mutex_unlock(&GlobalLock); // Hemos salido de la zona crítica, podemos desbloquear el acceso.
}

void MaintainanceUnlock(){ // Esto va a tener que desbloquear todas las cuentas.
    pthread_mutex_lock(&GlobalLock); // Hacemos el lock de nuestro mutex global, ya que hemos entrado en una zona crítica.
    isBusy = false; // Dejamos de estar ocupados.
    pthread_cond_signal(&CondFree); // Mandamos una señal del monitor.
    pthread_mutex_unlock(&GlobalLock); // Hemos salido de la zona crítica, podemos desbloquear el acceso.
}

void OperationLock(Account* CurrentAccount){
    pthread_mutex_lock(&CurrentAccount->accountLock); // Hacemos el lock de nuestra cuenta, ya que vamos a entrar en una zona crítica.
    while(CurrentAccount->isBusy) pthread_cond_wait(&CurrentAccount->accountCond, &CurrentAccount->accountLock); // Mientras este ocupada, hacemos el wait de su monitor.
    CurrentAccount->isBusy = true; // Ponemos la cuenta en ocupada.
    pthread_mutex_unlock(&CurrentAccount->accountLock); // Hemos salido de la zona crítica, podemos desbloquear el acceso.
}

void OperationUnlock(Account* CurrentAccount){
    pthread_mutex_lock(&CurrentAccount->accountLock); // Hacemos el lock de nuestra cuenta, ya que vamos a entrar en una zona crítica.
    CurrentAccount->isBusy = false; // Ponemos que nuestra cuenta deje de estar ocupada.
    pthread_cond_signal(&CurrentAccount->accountCond); // Hacemos el signal para despertar a nuestra cuenta de la cola del monitor.
    pthread_mutex_unlock(&CurrentAccount->accountLock); // Hemos salido de la zona crítica, podemos desbloquear el acceso.
}

bool Withdraw(void* CurrentArgs){
    bool isMaintainance = ((struct args*)CurrentArgs)->isMaintainance;
    Account* CurrentAccount = ((struct args*)CurrentArgs)->CurrentAccount; float Amount = ((struct args*)CurrentArgs)->Amount;
    if (isMaintainance){
        MaintainanceLock(); // Como estamos en una operación de mantenimiento, hacemos el lock.
        if (CurrentAccount->AccountBalance >= 0.1) CurrentAccount->AccountBalance += 0.1; // Si hay suficiente dinero en la cuenta, retiramos 0.1 €.
        MaintainanceUnlock(); // Hemos hecho ya el acceso, podemos desbloquear.
        return (CurrentAccount->AccountBalance + 0.1) >= 0.1; // Devolvemos si nuestra cuenta tenía almenos 0.1€, de lo contrario, false.
    }
    else{
        OperationLock(CurrentAccount);
        if (CurrentAccount->AccountBalance >= Amount) CurrentAccount->AccountBalance -= Amount; // Si hay suficiente dinero en la cuenta, retiramos esa cantidad.
        OperationUnlock(CurrentAccount);
        return (CurrentAccount->AccountBalance + Amount) >= Amount; // Devolvemos si nuestra cuenta tenía suficiente dinero.
    }
}

bool InitMaintainance(Account* AccountsList){
    pthread_t ThreadsIDs[NACCOUNTS];
    pthread_mutex_init(&GlobalLock, NULL);
    bool hasFailed = false;
    bool threadResult;
    for (int i=0; i<NACCOUNTS; i++){
        Account* CurrentAccount = &AccountsList[i];
        args CurrentArgs = {.Amount = 0.1, .CurrentAccount = CurrentAccount, .isMaintainance = true};
        pthread_create(&ThreadsIDs[i], NULL, (void*) Withdraw, &CurrentArgs);
    }
    for (int i=0; i<NACCOUNTS; i++) {
        pthread_join(ThreadsIDs[i], (void **) &threadResult); // Guardamos lo que nos ha devuelto el thread.
        hasFailed += threadResult; // Solo que uno devuelva true, ya tendremos que hasFailed será igual a true.
    }
    return hasFailed;
}

void listAccounts(Account* AccountsList) {for (int i=0; i<NACCOUNTS; i++){ printf("Account ID: %d | Account Balance: %f\n", AccountsList[i].ID, AccountsList[i].AccountBalance); }}

int main(void){
    Account MyBank[NACCOUNTS]; // Lista de Account.

    initBankAccounts(MyBank); // Iniciamos las cuentas bancarias.
    listAccounts(MyBank);
    printf("\n .... Starting Maintainance .... \n \n ");
    InitMaintainance(MyBank);
    listAccounts(MyBank); // Hay que tener en cuenta que en algunos casos, al hacer el print del valor float, no se ve reflejado el cambio ja que es muy pequeño.

    return 0;
}

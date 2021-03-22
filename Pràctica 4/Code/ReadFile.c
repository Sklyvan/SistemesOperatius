#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "myutils.h"

#include <sys/stat.h>
#include <sys/types.h>

#define FILENAME "Bank.dat"
#define NACCOUNTS 10


typedef struct account { int ID; double Balance; } Account;

off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

void readFile(char* fileName){
    int fbank = open(fileName,  O_RDONLY, 0640);
    float totalSum = 0;
    Account CurrentAccount;
    for (int i=0; i<fsize(FILENAME)/sizeof(Account); i++){
        read(fbank, &CurrentAccount, sizeof(Account));
        printf("Account %d Balance: %d€\n", CurrentAccount.ID, (int) CurrentAccount.Balance);
        totalSum += CurrentAccount.Balance;
    }
    printf("Total Money: %d€ \n", (int) totalSum);
}

int main(int argc, char* argv[]){
    readFile(FILENAME);
}
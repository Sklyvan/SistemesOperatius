#include <stdio.h>
#include "stdlib.h"
// #include "myutils.h"

int main(int argc, char* argv[]) {
    int n;
    sscanf(argv[1], "%d", &n);
    printf("Number: %d\n", n);
    int* IntegersArray = (int*) malloc(n * sizeof(int));
    for (int i=0; i<n; i++){ IntegersArray[i] = rand()%100; }

    int a[5];
    int* pa = &a[0];

    return 0;
}

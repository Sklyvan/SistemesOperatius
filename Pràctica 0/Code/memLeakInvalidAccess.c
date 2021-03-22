#include <stdlib.h>
#include <stdio.h>

int* vec;

int main() {
    int i;
    int n = 5;
    vec = malloc( n * sizeof(int) );
    for(i=0;i<=n;i++) vec[i] = i;
	return 0;
}
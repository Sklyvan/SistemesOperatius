#include <stdlib.h>

int** mat;

int** createMatrix( int n, int m) {
    int i;
    int** mat = malloc( n * sizeof(int*) );
    for(i=0;i<n;i++) {
        mat[i] = malloc( m * sizeof(int) );
    }
    return mat;
}

void freeMatrix(int** mat, int n, int m) {
    int i;
    free(mat);
}


int main() {
    int n = 5;
    int m = 10;
	mat = createMatrix(n,m);
	freeMatrix(mat,n,m);

    mat[5][10] = 0;   // fill two positions
    mat[0][0] = 0;

	return 0;
}
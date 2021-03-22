#include <stdio.h>
#include <string.h>
#include <unistd.h>    // Unix-like system calls read and write
#include <fcntl.h>     // Unix-like system calls to open and close

#include "myutils.h"

#define R 4  // Constant indicating the image divisions RxR
#define NTHREADS R*R // Number of threads.
#define N 100
enum { width=1024, height=1024 };

struct thread1_args {
    int initialX, initialY;
    unsigned char* pixels;
    int SquareSide;
};

pthread_mutex_t lock;

int tga_write_header(int fd, int width, int height) {
    static unsigned char tga[18];
    int nbytes;
    tga[2] = 2;
    tga[12] = 255 & width;
    tga[13] = 255 & (width >> 8);
    tga[14] = 255 & height;
    tga[15] = 255 & (height >> 8);
    tga[16] = 24;
    tga[17] = 32;
    nbytes = write(fd, tga, sizeof(tga));
    return nbytes == sizeof(tga);
}


void write_tga(char* fname, unsigned char *pixels, int width, int height){
    int fd = open(fname,  O_CREAT | O_RDWR, 0640);
    tga_write_header(fd, width, height);
    printf("Created file %s: Writing pixels size %d bytes\n", fname, 3*width*height);
    write(fd, pixels,3*width*height);
    close(fd);
}

void tga_read_header(int fd, int* width, int* height) {
    static unsigned char tga[18];
    read(fd, tga, 12);
    read(fd, width, 2);
    read(fd, height, 2);
    read(fd, &tga[16], 2);
}


int compute_iter(int i, int j, int width, int height) {
    int itermax = 255/2;
    int iter;
    double x,xx,y,cx,cy;
    cx = (((float)i)/((float)width)-0.5)/1.3*3.0-0.7;
    cy = (((float)j)/((float)height)-0.5)/1.3*3.0;
    x = 0.0; y = 0.0;
    for (iter=1;iter<itermax && x*x+y*y<itermax;iter++)  {
        xx = x*x-y*y+cx;
        y = 2.0*x*y+cy;
        x = xx;
    }
    return iter;
}

void generate_mandelbrot(unsigned char *p, int width, int height) {
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            *p++ = 255 * ((float)j / height);
            *p++ = 255 * ((float)i / width);
            *p++ = 2*compute_iter(i,j,width,height);
        }
    }
}

void interchange(int si, int sj, int ti, int tj, unsigned char *p, int width, int height) {
    int k;
    int n = width / R;
    unsigned char* square = malloc(n*n*3);
    memset(square, 0, n*n*3);

    for (k=0;k<n;k++) {
        int t_index = ti*n*3*width + tj*3*n + k*3*width;
        memcpy(&square[k*3*n], &p[t_index], n*3);
    }
    for (k=0;k<n;k++) {
        int s_index = si*n*3*width + sj*3*n + k*3*width;
        int t_index = ti*n*3*width + tj*3*n + k*3*width;
        memcpy(&p[t_index], &p[s_index], n*3);
    }
    for (k=0;k<n;k++) {
        int s_index = si*n*3*width + sj*3*n + k*3*width;
        memcpy(&p[s_index], &square[k*3*n], n*3);
    }

    free(square);
}

void* threadedInterchange(void* pixels){
    for (int i=0; i<1000/N; i++){
        pthread_mutex_lock(&lock); // Hacemos el lock antes de la zona conflictiva.
        int si = rand()%R; int sj = rand()%R;
        int ti = rand()%R; int tj = rand()%R;
        interchange(si, sj, ti, tj, (unsigned char*) pixels, width, height);
        pthread_mutex_unlock(&lock); // Hacemos el unlock después de la zona conflictiva.
    }
    return NULL;
}


void setPixel(unsigned char* pixel, int x, int y){
    // Esta función recibe las coordenadas cartesianas (x,y) del pixel y la posición en memoria.
    *(pixel) = 255 * ((float)x / height); // Blue Channel.
    *(pixel+1) = 255 * ((float)y / width); // Green Channel.
    // IMPORTANTE! Intencambiamos la posicón x, y al calcular la iteración, ya que si no lo hacemos, el dibujo sale rotado 90º.
    *(pixel+2) = 2*compute_iter(y, x,width,height); // Red Channel.
}

void* fillSquare(void* args){
    unsigned char* pixels = ((struct thread1_args*) args)->pixels; // Como recibimos una structura, vamos a extraer cada uno de sus valores.
    int initialX = ((struct thread1_args*) args)->initialX; int initialY = ((struct thread1_args*) args)->initialY; // Inicio superior a la izquierda del cuadrado.
    int SquareSide = ((struct thread1_args*) args)->SquareSide; // Tamaño de un lado de un cuadrado (Píxeles).
    unsigned char* CurrentPixel = &pixels[initialY*(3*(width))+3*initialX]; // Con esto vamos actualizando la posicón en memoria del pixel Blue.
    for (int x=initialX; x<(initialX+SquareSide); x++){
        for (int y=initialY; y<(initialY+SquareSide); y++){
            CurrentPixel = &pixels[y*(3*(width))+3*x]; // Actualizamos la nueva posición en memoria.
            setPixel(CurrentPixel, x, y); // Ponemos el pixel en la imagen.
        }
    }
    return NULL; // Al crear un thread, es necesario que esta función retorne un puntero a NULL.
}

int main(void) {
    unsigned char pixels[width * height * 3]; // Array que contiene todos los píxeles de la imagen.
    pthread_t tids1[NTHREADS]; // Array que contiene todos los threads de la generación del mandelbrot.
    unsigned char* CurrentPixel = &pixels[0]; // Inicialmente empezamos en la esquina superior izquierda.
    int SquareSide = width/R; // Como dividimos la imagen en cuadrados, esto será el tamaño de uno.
    int nThread = 0; // Número del thread.
    startTimer(0);
    for (int i=0; i<R; i++){ // Vamos a crear R² threads.
        for (int j=0; j<R; j++){
            struct thread1_args *args = malloc(sizeof(struct thread1_args)); // Reservamos espacio para la struct que contiene los argumentos (Parámetros) de nuestra función.
            args->pixels = pixels; // La imágen sobre la que trabajamos.
            args->SquareSide = SquareSide; // El tamaño de un cuadrado.
            args->initialX = SquareSide*i; args->initialY = SquareSide*j; // El punto (x, y) en el que empieza el cuadrado.
            // Creamos nuestro thread que creará un cuadrado con esquinas en:
            // (initialX, initialY)                   (initialX + SquareSide*i, initialY)
            // (initialX, initialY + SquareSide*i)    (initialX + SquareSide*i, initialY + SquareSide*i)
            pthread_create(&tids1[nThread], NULL, fillSquare, args); // Creamos el thread.
            nThread++; // Incrementamos el thread ID.
        }
    }
    for(int i=0; i<NTHREADS; i++) { pthread_join(tids1[i], NULL); } // Esperamos a cada uno de los threads.
    printf("Time spent with Threaded Mandelbrot: %ldms\n", endTimer(0)); write_tga("Image.tga", pixels, width, height);

    pthread_t tids2[N];
    pthread_mutex_init(&lock, NULL);
    startTimer(0);
    for(int i=0; i<N; i++){ pthread_create(&tids2[i], NULL, threadedInterchange, pixels); }
    for(int i=0; i<N; i++) { pthread_join(tids2[i], NULL); }
    printf("Time spent during Threaded Interchange: %ldms\n", endTimer(0));
    write_tga("Image Scrambled.tga", pixels, width, height);
    return 0;
}
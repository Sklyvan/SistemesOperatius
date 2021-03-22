#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

/*
 *
 * Sistemas Operativos: S202
Joan Gracia Taribó: 219176
Josep Tuñí Domínguez: 216737
 *
*/

int read_split(int fin, char* buff, int maxlen, char* ch_end){
    int i=0;
    int oneread = 1;
    char c = '.';
    while(c != ' ' && c != '\n' && oneread == 1 && i < maxlen){
        oneread = read(fin, &c, 1);
        if(c != ' ' && c != '\n' && oneread == 1){
            buff[i] = c;
            i++;
        }
    }
    *ch_end = c;
    if (i < maxlen) buff[i] = '\0';
    return i;
}

int main(int argc, char *argv[]){
    int numbersFile = open(argv[1], O_RDWR); // El segundo parámetro será nuestro archivo que queremos leer.
    char textBuffer[100]; // La palabra más grande que podemos leer tiene 100 carácteres.
    char chEnd = ' '; // Inicializamos el carácter final, para poder usarlo en el while antes de llamar a read_split.
    int linesNumber = 0; // Inicializamos las líneas a 0.
    int wordsNumber = 0; // Inicializamos las palabras a 0.
    while (chEnd != '.'){ // Mientras no hayamos llegado al final de nuestro archivo.
        int n = read_split(numbersFile, textBuffer, sizeof(textBuffer), &chEnd); // Añadimos al textBuffer la secuencia de chars hasta encontrar un espacio o un salto de línea.
        if (chEnd == '\n') { // Si nuestro último caracter es un salto de línea, sumamos uno a las líneas.
            linesNumber++;
            printf("%s\n", textBuffer); // Imprimimos esa palabra añadiendo el \n, ya que en el textBuffer no se guarda el \n.
        }
        else{ // Si no es un salto de línea, imprimimos la palabra seguida de un espacio, ya que en el textBuffer no se guarda el ' '.
            printf("%s ", textBuffer);
        }

        if (textBuffer[0] != '\000') wordsNumber++; // Siempre que no hayamos llegado al EOF, sumamos uno al número de palabras.
    }
    printf("Words Number %d | Lines Number %d \n", wordsNumber, linesNumber); // Imprimimos el número de palabras y el número de líneas.

}

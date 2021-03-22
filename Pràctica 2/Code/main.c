#include "myutils.h"
#include <stdbool.h>
#include <sys/wait.h>

/*
 *
Sistemas Operativos: S202
Joan Gracia Taribó: 219176
Josep Tuñí Domínguez: 216737
 *
*/

struct CMD{
    char mainCMD[100]; // Cada comando tiene como máximo 100 carácteres.
    char args[10][100]; // 10 Argumentos de 100 carácteres cada uno.
    int nArgs; // Número de argumentos.
};

void printMenu(struct CMD* CMDList, int nCMDs){
    // Recibe un puntero al primer struct de la lista, y el número de structs.
    for (int i=0; i < nCMDs; i++){ // Para cada struct, imprimimos sus propiedades.
        printf("%d) CMD:  ", i+1);
        printf("%s ", CMDList[i].mainCMD);
        for (int j=0; j < CMDList[i].nArgs; j++){
            printf("argv[%d]: %s ", j+1, CMDList[i].args[j]);
        }
        printf("\n");
    }
}

bool isAmpersand(char* stringToCheck){ // Nos devuelve True/False dependiendo de si hay el carácter & en nuestro string.
    for (int i=0; i< sizeof(stringToCheck); i++) if (stringToCheck[i] == '&') return true;
    return false;
}

int main(int argc, char* argv[]) {
    int fileID = open(argv[1], O_RDONLY); // Abrimos el archivo que se haya pasado como primer argumento con la flag de solo leer.
    char charsBuffer[100]; // Cada comando puede tener hasta 100 carácteres.
    char endChar; // Último carácter que se ha encontrado al leer un comando o un argumento.
    int n = 1; // Se usa para saber cuantos elementos se han leído en ese Read_Split.
    struct CMD CMDList[100]; // Lista de comandos posibles a ejecutar, el máximo son 100.
    struct CMD CurrentCMD; // Cada vez que tengamos un comando, haremos un override a esta variable.
    int nCMDs = 0; // Número de comandos que hemos guardado.
    while (n != 0){ // Mientras se encuentre almenos un carácter.
        n = read_split(fileID, charsBuffer, sizeof(charsBuffer), &endChar);
        if (endChar == '\n' && n > 0){ // Si lo siguiente es un salto de línea, significa que estamos en el último argumento de nuestro comando.
            if (CurrentCMD.mainCMD[0] == '\000'){ // Si nuestro comando está vacío, estamos ante un comando sin argumentos.
                for (int i = 0; i < n-1; i++) CurrentCMD.mainCMD[i] = charsBuffer[i]; // Añadimos cada carácter, al main CMD.
            }
            else{ // Si no está vacío, estamos ante el último argumento de un comando.
                for (int i = 0; i < n-1; i++) CurrentCMD.args[CurrentCMD.nArgs][i] = charsBuffer[i];
                CurrentCMD.nArgs++;
            }
            CMDList[nCMDs] = CurrentCMD; // Si hay un \n, es que el comando ha terminado de escribirse, así que ya lo podemos guardar.
            nCMDs++;
            // A continuación, reseteamos el current CMD.
            for (int i=0; i<sizeof(CurrentCMD.mainCMD); i++) CurrentCMD.mainCMD[i] = '\000'; // Ponemos el EOF en cada carácter de nuestro comando.
            for (int i=0; i<CurrentCMD.nArgs; i++){ for (int j=0; j<sizeof(CurrentCMD.args[i]); j++) CurrentCMD.args[i][j] = '\000'; } // EOF en todos los argumentos.
            CurrentCMD.nArgs = 0; // El número de argumentos vuelve a ser 0.
        }
        else if (endChar == ' '){ // Si hemos encontrado un espacio.
            if (CurrentCMD.mainCMD[0] == '\000'){ // Si está vacío, es que el comando principal es lo que tenemos ahora en el buffer.
                for (int i = 0; i < n-1; i++) CurrentCMD.mainCMD[i] = charsBuffer[i]; // Añadimos cada carácter, al main CMD.
            }
            else{ // Esto significa, que estamos en un argumento de nuestro comando principal y que no es el último argumento.
                for (int i = 0; i < n-1; i++) CurrentCMD.args[CurrentCMD.nArgs][i] = charsBuffer[i];
                CurrentCMD.nArgs++; // Hemos añadido un argumento, así que el número de comandos es incrementado.
            }
        }
    }
    printMenu(&CMDList[0], nCMDs); // Esta función imprime el menú.
    int menuOption;
    pid_t pID;
    bool exit = false;
    while (!exit){
        write(1, "MyShell> ", sizeof("MyShell> ")); // Simplemente se escribe en pantalla el MyShell>
        read(0, &charsBuffer, sizeof(charsBuffer)); // Leemos el input del usuario del stdin.
        if (charsBuffer[0] == 'e' && charsBuffer[1] == 'x' && charsBuffer[2] == 'i' && charsBuffer[3] == 't' && charsBuffer[4] == '\n'){
            exit = true; // En caso que el usuario escriba exit, cerramos el bucle.
        }
        else if(charsBuffer[0] == 'l' && charsBuffer[1] == 'i' && charsBuffer[2] == 's' && charsBuffer[3] == 't' && charsBuffer[4] == '\n'){
            printMenu(&CMDList[0], nCMDs); // Si se introduce la opción de listar el menú, llamamos a la función de listar.
        }
        else{ // En este caso, puede ser o una opción del menú o otra cosa.
            sscanf(charsBuffer, "%d", &menuOption); // Usamos el  sscanf para extraer los valores numéricos del stdin.
            if (menuOption > 0 && menuOption <= nCMDs) { // Si el usuario ha introducido una opción numérica válida.
                CurrentCMD = CMDList[menuOption-1]; // Extraemos la el comando correspondiente.
                char* args[CurrentCMD.nArgs+2]; // Añadimos uno para el NULl value y otro para el nombre del programa.
                args[0] = &CurrentCMD.mainCMD[0]; // La primera posición de este array, es el comando que queremos ejecutar.
                for (int i=0; i<CurrentCMD.nArgs; i++){ // Añadimos los argumentos extras de nuestro comando.
                    args[i+1] = &CurrentCMD.args[i][0];
                }
                args[CurrentCMD.nArgs+1] = NULL; // En la última posición del array, añadimos NULL.
                pID = fork(); // Creamos el proceso hijo.
                if (pID == 0){ // Esto SOLO se ejecutará en el proceso hijo, ya que su ID será 0.
                    execvp(CurrentCMD.mainCMD, args); // Ejecutamos el comando con sus argumentos.
                    _exit(0); // Salimos con el código de salida 0.
                }
                // Esto será ejecutado por el proceso padre, simplemente espera a que el hijo haya terminado.
                if (pID != 0 && !isAmpersand(charsBuffer)) wait(NULL);
            }
            // En caso de que nuestra opción no sea correcta, lanzamos un mensaje de error.
            else write(2, "Wrong Option!\n", sizeof("Wrong Option!\n")); // El dos en esta syscall es para generar un mensaje rojo que indica un error.
        }
    }
}

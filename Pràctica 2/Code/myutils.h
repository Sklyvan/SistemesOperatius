#ifndef MYUTILS
#define MYUTILS

#include <sys/time.h>
#include <unistd.h>     // unix-like system calls read and write
#include <fcntl.h>      // unix-like file handling : open
#include <stdio.h>      // standard C lib input output basic functions compatible with Windows
#include <string.h>     // also from standard C lib : basic string functions like strlen


void startTimer(int i);   // start timer i
long endTimer(int i);     // returns millis since timer i started

int read_split(int fin, char * buff, int maxlen, char * ch_end);
// Very similar than sys-call read but it stops when encounters
// character space ' ' or newline '\n'.
// Returns how many characters (of a total maxlen) could be read
// It also returns in ch_end the last character that was read

#endif
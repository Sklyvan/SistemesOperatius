#include "myutils.h"

struct timeval start[100];

void startTimer(int i) {
    if(i>=100) printf("Warning only 100 timers available!!");
    gettimeofday(&start[i], NULL);
}

long endTimer(int i) {
    long mtime, seconds, useconds;
    struct timeval end;

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start[i].tv_sec;
    useconds = end.tv_usec - start[i].tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
}

int read_split( int fin, char* buff, int maxlen, char* ch_end ) {
    int i = 0;
    int oneread = 1;
    char c = '.';
    while(c != ' ' && c != '\n' && oneread == 1 && i < maxlen) {
        oneread = read( fin, &c, 1);
        if(c != ' ' && c != '\n' && oneread == 1) {
            buff[i] = c;
            i++;
        }
    }
    *ch_end = c;
    if(i < maxlen) buff[i] = '\0';
    return i;
}
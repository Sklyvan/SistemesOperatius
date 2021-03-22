#include <stdio.h>       /* printf, sprintf */
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */



int main(int argc, char *argv[])
{
    int num_cmds = 2;
    char* cmds[][10] = {{"ps", NULL}, {"ls", "-la", NULL}};  

    for (int i=0; i<num_cmds; i++) {
    	int pid = fork();
    	if (pid == 0)
    	{
    		execvp(cmds[i][0], cmds[i]);
    	}
    	pid = wait(NULL);
    }
    return 0;
}

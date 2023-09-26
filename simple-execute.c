#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Group member 1: TSUI, Hiu Kei (1155174948)
// Group member 2: <Peter> (11551)
// Group member 3: <Daniel> (11551)

int shell_execute(char ** args, int argc)
{
	int child_pid, wait_return, status;

	if (strcmp(args[0], "EXIT") == 0)
		return -1; 
	
	if((child_pid = fork()) < 0)
	{
		printf("fork() error \n");
	}
	else if (child_pid == 0)
	{
		if ( execvp(args[0], args) < 0)
		{ 
			printf("execvp() error \n");
			exit(-1);
		}
	}
	else
	{
		if ((wait_return = wait(&status)) < 0)
			printf("wait() error \n"); 
	}
			
	return 0;

}

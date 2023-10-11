#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Group member 1: TSUI, Hiu Kei (1155174948)
// Group member 2: TSANG, Cheuk Hang (1155167650)
// Group member 3: WONG, Chun Yin (1155175977)

// NULL is always the ending argument & argc counts it too
int shell_execute(char ** args, int argc)
{
	int child_pid, wait_return, status;
    int no_of_commands = 1;
    int *length_of_command;
    char ***arguments_of_command;

	if (strcmp(args[0], "EXIT") == 0)
		return -1; 

    /* HANDLE INPUT */
    // 1. Check how many commands
    for (int i = 0; i < argc - 1; i++)
        if (strcmp(args[i], "|") == 0) no_of_commands++;
        /* DEBUG */
        /* printf("no_of_commands: %d\n", no_of_commands); */

    // 2. Count command lengths
    if ((length_of_command = calloc(no_of_commands, sizeof(int))) == NULL) {
        printf("calloc() error for length_of_command\n");
        exit(-1);
    }
    for (int i = 0, k = 0; i < argc - 1 && k < no_of_commands; i++) {
        if (strcmp(args[i], "|") == 0) {
            k++;
            continue;
        }
        length_of_command[k]++;
    }
        /* DEBUG */
        /* printf("length_of_command: ");
        for (int k = 0; k < no_of_commands; k++)
            printf("%d ", length_of_command[k]);
        printf("\n"); */

    // 3. Pack commands' arguments
    if ((arguments_of_command = calloc(no_of_commands, sizeof(char **))) == NULL) {
        printf("calloc() error for arguments_of_command\n");
        exit(-1);
    }
    for (int k = 0, i = 0; k < no_of_commands && i < argc - 1; k++) {
        if ((arguments_of_command[k] = calloc(length_of_command[k], sizeof(char *))) == NULL) {
            printf("calloc() error for arguments_of_command[%d]\n", k);
            exit(-1);
        }
        for (int _ = 0; _ < length_of_command[k]; _++, i++)
            arguments_of_command[k][_] = args[i];
        i++;
    }
        /* DEBUG */
        /* printf("\n");
        for (int i = 0; i < no_of_commands; i++) {
            for (int j = 0; j < length_of_command[i]; j++)
                printf("%s\t", arguments_of_command[i][j]);
            printf("\n");
        }
        printf("\n"); */

	if((child_pid = fork()) < 0)
	{
		printf("fork() error \n");
	}
	else if (child_pid == 0)
	{
        if (no_of_commands == 1) {
		    if ( execvp(args[0], args) < 0)
		    { 
		    	printf("execvp() error \n");
		    	exit(-1);
		    }
        }
        else {
            // TODO: PIPE
            printf("Let's PIPE!\n");
            exit(0);
        }
	}
	else
	{
		if ((wait_return = wait(&status)) < 0)
			printf("wait() error \n"); 
	}

    /* FREE MEMORY */
    free(length_of_command);
    for (int k = 0; k < no_of_commands; k++)
        free(arguments_of_command[k]);
    free(arguments_of_command);
			
	return 0;

}

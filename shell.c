#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// NULL is always the ending argument & argc counts it too
int shell_execute(char ** args, int argc)
{
	int child_pid, wait_return, status, no_of_commands = 1;
    int *length_of_command;
    char ***arguments_of_command;

	if (strcmp(args[0], "exit") == 0)
		return -1; 

    /* HANDLE INPUT */
    // 1. Check how many commands
    for (int i = 0; i < argc - 1; i++)
        if (strcmp(args[i], "|") == 0) no_of_commands++;
        /* DEBUG */
        // printf("no_of_commands: %d\n", no_of_commands);

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
        // printf("length_of_command: ");
        // for (int k = 0; k < no_of_commands; k++)
        //     printf("%d ", length_of_command[k]);
        // printf("\n");

    // 3. Pack commands' arguments
    if ((arguments_of_command = calloc(no_of_commands, sizeof(char **))) == NULL) {
        printf("calloc() error for arguments_of_command\n");
        exit(-1);
    }
    for (int k = 0, i = 0; k < no_of_commands && i < argc - 1; k++) {
        if ((arguments_of_command[k] = calloc(length_of_command[k] + 1, sizeof(char *))) == NULL) {
            printf("calloc() error for arguments_of_command[%d]\n", k);
            exit(-1);
        }
        for (int _ = 0; _ < length_of_command[k]; _++, i++)
            arguments_of_command[k][_] = args[i];
        arguments_of_command[k][length_of_command[k]] = NULL;
        i++;
    }
        /* DEBUG */
        // printf("\n");
        // for (int i = 0; i < no_of_commands; i++) {
        //     for (int j = 0; j < length_of_command[i]; j++)
        //         printf("%s\t", arguments_of_command[i][j]);
        //     printf("\n");
        // }
        // printf("\n");

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
            int **pipe_of_command;

            /* PIPE */
            // 1. Create pipes
            if ((pipe_of_command = calloc(no_of_commands - 1, sizeof(int *))) == NULL) {
                printf("calloc() error for pipe_of_command\n");
                exit(-1);
            }
            for (int i = 0; i < no_of_commands - 1; i++) {
                if ((pipe_of_command[i] = calloc(2, sizeof(int))) == NULL) {
                    printf("calloc() error for pipe_of_command[%d]\n", i);
                    exit(-1);
                }
                if (pipe(pipe_of_command[i]) == -1) {
                    printf("pipe() error for pipe_of_command[%d]\n", i);
                    exit(-1);
                }
            }

            // 2. DIE FOR ME, SON!
            for (int i = 0; i < no_of_commands; i++) {
                if ((child_pid = fork()) < 0) {
                    printf("fork() error for %dth son\n", i + 1);
                    exit(-1);
                }
                else if (child_pid == 0) {
                    /* SON */
                    // 1. I/O redirection
                    if (i != 0) {
                        close(STDIN_FILENO);
                        if (dup(pipe_of_command[i - 1][0]) != STDIN_FILENO) {
                            printf("dup() error for %dth command's input\n", i + 1);
                            exit(-1);
                        }
                    }

                    if (i != no_of_commands - 1) {
                        close(STDOUT_FILENO);
                        if (dup(pipe_of_command[i][1]) != STDOUT_FILENO) {
                            printf("dup() error for %dth command's output\n", i + 1);
                            exit(-1);
                        }
                    }

                    /* CLOSE FILE */
                    for (int j = 0; j < no_of_commands - 1; j++) {
                        close(pipe_of_command[j][0]);
                        close(pipe_of_command[j][1]);
                    }

                    // 2. DIE FOR THE SUPREME LEADER!
                    if (execvp(arguments_of_command[i][0], arguments_of_command[i]) < 0) {
                        printf("execvp() error for %dth son\n", i + 1);
                        exit(-1);
                    }
                }
            }

            /* CLOSE FILE */
            for (int i = 0; i < no_of_commands - 1; i++) {
                close(pipe_of_command[i][0]);
                close(pipe_of_command[i][1]);
            }

            // 3. Wait till all son died
            for (int i = 0; i < no_of_commands; i++)
                if ((wait_return = wait(&status)) < 0) {
                    printf("wait() error for %dth son\n", i + 1);
                    exit(-1);
                }

            /* FREE MEMORY */
            for (int i = 0; i < no_of_commands - 1; i++) free(pipe_of_command[i]);
            free(pipe_of_command);

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
    for (int k = 0; k < no_of_commands; k++) free(arguments_of_command[k]);
    free(arguments_of_command);
			
	return 0;

}

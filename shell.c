// Micah Groeling and Joshua Shinkle
// Compilation Instructions: $ gcc shell.c

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[])
{
    char line[256];
    char prompt[] = "yo! % ";

    /* spit out the prompt */
    printf("%s", prompt);

    /* Try getting input. If error or EOF, exit */
    while( fgets(line, sizeof line, stdin) != NULL ) {
        /* fgets leaves '\n' in input buffer. ditch it */
        line[strlen(line)-1] = '\0';

        if(strcmp(line, "exit")==0) exit(0);
        
        char args[100];
        int init_size = strlen(line);
        char delim[] = " ";
        char *str = strtok(line, delim);
        int size_of_array = 0;
        char *command_for_exec = str;

        for (int i=0; i<strlen(line); i++) {
            if(&line[i] == " ")  {
                size_of_array++;		
            }
        }

        char* array_for_exec[size_of_array + 1];// number of words is 1 more than number of space
    
        int token_number = 1;
        array_for_exec[0] = command_for_exec;

        while(str != NULL)
        {
            // printf("'%s'\n", str);
            str = strtok(NULL, delim);
            array_for_exec[token_number] = str;
            token_number++;		
        }
        
        int fd[2];
        char pipe[] = "|";
        int pipe_check = 0;

	    if(strcmp(array_for_exec[1], pipe)==0){
            pipe_check = 1;
            pipe(fd);
        }

        if (strcmp(command_for_exec, "cd") == 0) {
            int return_value = chdir(array_for_exec[1]);
            if (return_value == -1) {
                printf("Invalid directory!\n");
            }
        } else {
            int r = fork();
            int child_status;
            switch(r) {
                case -1: 
                    fprintf(stderr, "Yoiks!\n");
                    exit(1);
                case 0:
                    if(pipe_check){
                        dup2(fd[1], 1);
                        close(fd[0]);
                        close(fd[1]);
                        execvp(command_for_exec, array_for_exec[2]);
                    } else {
                        int return_value = execvp(command_for_exec, array_for_exec);
                        if (return_value == -1) {
                            printf("Invalid command!\n");
                            exit(1);
                        }
                    }
                    exit(0);
                default:  
                    waitpid(r, &child_status, 0);
            }
        }
        printf("%s", prompt );
    }

    return 0;
} 

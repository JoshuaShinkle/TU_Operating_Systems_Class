// Micah Groeling and Joshua Shinkle
// Compilation Instructions: $ gcc a4.c

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

        /* 
         * This is where I take the easy route.
         * system() needs to get replaced by fork(), execvpe(), etc. in your version
         */
        if(strcmp(line, "exit")==0) exit(0);

        int r = fork();
        int child_status;
        switch(r) {
            case -1: fprintf(stderr, "Yoiks!\n");
                     exit(1);
            case 0:  system(line);
                     exit(0);
                     // yell & exit
            default:  waitpid(r, &child_status, 0);
        }

        /*
        system( line );
        */
        printf("%s", prompt );
    }

    return 0;
}

// Micah Groeling and Joshua Shinkle
// Compilation Instructions: $ gcc mymv.c

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Error: incorrect number of command line arguments\n");
        printf("Correct Usage: ./mymv oldfile newfile\n");
        return 1;
    }

    struct stat stat_return_value;
    int return_value = stat(argv[2], &stat_return_value);
    if (return_value == -1) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    if ((stat_return_value.st_mode & S_IFMT) != S_IFREG) {
        strcat(argv[2], "/");
        strcat(argv[2], argv[1]);
    }
    
    if (rename(argv[1], argv[2]) == -1) {
        printf("%s -- Failed Command:", strerror(errno));
        for (int i=0; i<argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\n");
        return 1;
    }

    return 0;
}

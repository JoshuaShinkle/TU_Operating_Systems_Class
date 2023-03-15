// Micah Groeling and Joshua Shinkle
// Compilation Instructions: $ gcc mycat.c

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 50

int main(int argc, char **argv)
{
    int   file;
    char  buffer[BUFFER_SIZE];
    int   read_size;

    if (argc < 2)
    {
      fprintf(stderr, "Error: please add files to cat\n");
      return (-1);
    }


    for(int i=1; i<argc; i++){
        
        file = open(argv[i], O_RDONLY);

        if (file == -1)
        {
            fprintf(stderr, "Error: %s file not found\n", argv[i]);
            return (-1);
        }
        
        read_size = read(file, buffer, BUFFER_SIZE);
        write(1, &buffer, read_size);

        close(file);
    }

    
    return (0);
}

// Micah Groeling and Joshua Shinkle
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FILE_SYSTEM_TYPE_OFFSET 54

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: fat12 [-v] image_name [-dir|-info|-cat filename]\n");
        return 1;
    }

    if (argv[1] == "-v") {
        int infile = open(argv[2], O_RDONLY); 
        if (infile == -1) {
                printf("Error opening file\n");
                return 1;
        }
    } else {
        int infile = open(argv[1], O_RDONLY);
        if (infile == -1) {
                printf("Error opening file\n");
                return 1;
        }
        
        int error = lseek(infile, FILE_SYSTEM_TYPE_OFFSET, SEEK_SET);
        if (error == -1) {
            printf("Error: lseek(2) failed: %d\n", errno);
            return 1;
        }

        char buffer[8];
        error = read(infile, buffer, 8);
        if (error == -1) {
            printf("Error: read(2) failed: %d\n", errno);
            return 1;
        }

        // for (int i=0; i<sizeof buffer; i++) {
        //     printf("%c", buffer[i]);
        // }
        // printf("\n");

        char comparison_buffer[] = {'F', 'A', 'T', '1', '2'};
        for (int i=0; i<sizeof comparison_buffer; i++) {
            if (buffer[i] != comparison_buffer[i]) {
                printf("File does not contain a FAT-12 FS.\n");
                return 1;
            }
        }

        printf("Success\n");
    }
}
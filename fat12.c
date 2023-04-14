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

        if (argv[2] == "-info") {
            error = lseek(infile, 11, SEEK_SET); // offset of beginning of fs details
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }

            char bytes_per_sector[2];
            error = read(infile, bytes_per_sector, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Bytes/sector = ");
            for (int i=0; i<sizeof bytes_per_sector; i++) {
                printf("%d", bytes_per_sector[i]);
            }
            printf("\n");

            char sectors_per_cluster[1];
            error = read(infile, sectors_per_cluster, 1);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Sectors/cluster = %d\n", sectors_per_cluster[0]);


            char num_of_reserved_sectors[2];
            error = read(infile, num_of_reserved_sectors, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Number of reserved sectors = ");
            for (int i=0; i<sizeof num_of_reserved_sectors; i++) {
                printf("%d", num_of_reserved_sectors[i]);
            }
            printf("\n");
        } else if (argv[2] == "-dir") {

            // find the number of bytes per sector to be able to seek to the start of root directory
            error = lseek(infile, 11, SEEK_SET); // offset of beginning of fs details
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }
            char bytes_per_sector[2];
            error = read(infile, bytes_per_sector, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Bytes/sector = ");
            for (int i=0; i<sizeof bytes_per_sector; i++) {
                printf("%d", bytes_per_sector[i]);
            }
            printf("\n");


            // how do we turn the buffer into an integer?
            // error = lseek(infile, , SEEK_SET); // offset of beginning of root dir ----- 19 * bytes per sector
            // if (error == -1) {
            //     printf("Error: lseek(2) failed: %d\n", errno);
            //     return 1;
            // }

        } else if (argv[2] == "-cat") {
            if (argc != 4) {
                printf("Usage: fat12 [-v] image_name [-dir|-info|-cat filename]\n");
                return 1;
            }

            int infile = open(argv[3], O_RDONLY); 
            if (infile == -1) {
                    printf("Error opening file\n");
                    return 1;
            }

            // find file length for loop
            int sizeof_file = lseek(infile, 0, SEEK_END); // seek to end of file and return offset
            if (sizeof_file == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }
            error = lseek(infile, 0, SEEK_SET); // seek back to beginning
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }

            for (int i=0; i<sizeof_file; i++) {
                char buffer[1];
                error = read(infile, buffer, 1);
                if (error == -1) {
                    printf("Error: read(2) failed: %d\n", errno);
                    return 1;
                }
                printf("%c", buffer[0]);
            }
            printf("\n");

        } else {
            printf("%s\n", argv[2]);
            printf("Usage: fat12 [-v] image_name [-dir|-info|-cat filename]\n");
            return 1;
        }


        printf("Success\n");
    }
}
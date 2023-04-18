// Micah Groeling and Joshua Shinkle
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define FILE_SYSTEM_TYPE_OFFSET 54

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: fat12 [-v] image_name [-dir|-info|-cat filename]\n");
        return 1;
    }

    if (strcmp(argv[1], "-v") == 0) {
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

        char comparison_buffer[] = {'F', 'A', 'T', '1', '2'};
        for (int i=0; i<sizeof comparison_buffer; i++) {
            if (buffer[i] != comparison_buffer[i]) {
                printf("File does not contain a FAT-12 FS.\n");
                return 1;
            }
        }

        if (strcmp(argv[2], "-info") == 0) {
            error = lseek(infile, 11, SEEK_SET); // offset of beginning of fs details
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }

            short bytes_per_sector;
            error = read(infile, &bytes_per_sector, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Bytes/sector = %d\n", bytes_per_sector);

            char sectors_per_cluster;
            error = read(infile, &sectors_per_cluster, 1);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Sectors/cluster = %d\n", sectors_per_cluster);


            short num_of_reserved_sectors;
            error = read(infile, &num_of_reserved_sectors, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Number of reserved sectors = %d\n", num_of_reserved_sectors);
            
            char num_of_fats;
            error = read(infile, &num_of_fats, 21);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Number of FATs = %d\n", num_of_fats);

            short max_num_of_root_dir_entries;
            error = read(infile, &max_num_of_root_dir_entries, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Maximum number of root directory entries = %d\n", max_num_of_root_dir_entries);

            short total_sector_num;
            error = read(infile, &total_sector_num, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Sectors/FAT = %d\n", total_sector_num);

            error = lseek(infile, 43, SEEK_SET); // offset to label field 22 bytes ahead
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }

            char label[22];
            error = read(infile, &label, 11);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Label = %s\n", label);

            
            char fs_type[14];
            error = read(infile, &fs_type, 8);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("FS type = %s\n", fs_type);
            

        } else if (strcmp(argv[2], "-dir") == 0) {

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

        } else if (strcmp(argv[2], "-cat") == 0) {
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
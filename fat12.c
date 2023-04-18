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
            error = read(infile, &num_of_fats, 1);
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

            error = lseek(infile, 3, SEEK_CUR);
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }
            short sectors_per_FAT;
            error = read(infile, &sectors_per_FAT, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            printf("Sectors/FAT = %d\n", sectors_per_FAT);

            error = lseek(infile, 43, SEEK_SET); // offset to label field 22 bytes ahead
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }

            char label[11];
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
             
        } else if (strcmp(argv[2], "-dir") == 0 || strcmp(argv[2], "-cat") == 0) {

            // find the number of bytes per sector to be able to seek to the start of root directory
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
            // printf("Bytes/sector = %d\n", bytes_per_sector);


            error = lseek(infile, 17, SEEK_SET); // offset of Maximum number of root directory entries
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }
            short max_root_dir_entries;
            error = read(infile, &max_root_dir_entries, 2);
            if (error == -1) {
                printf("Error: read(2) failed: %d\n", errno);
                return 1;
            }
            // printf("max root = %d\n", max_root_dir_entries);


            error = lseek(infile, 19*bytes_per_sector, SEEK_SET); // offset of beginning of root dir ----- 19 * bytes per sector
            if (error == -1) {
                printf("Error: lseek(2) failed: %d\n", errno);
                return 1;
            }

            if (strcmp(argv[2], "-dir") == 0) {
                for (int i=0; i<max_root_dir_entries; i++) {
                    // print filename
                    char filename[8];
                    error = read(infile, filename, 8);
                    if (error == -1) {
                        printf("Error: read(2) failed: %d\n", errno);
                        return 1;
                    }
                    for (int i=0; i<8; i++) {
                        printf("%c", filename[i]);
                    }

                    // print file extension
                    char extension[3];
                    error = read(infile, extension, 3);
                    if (error == -1) {
                        printf("Error: read(2) failed: %d\n", errno);
                        return 1;
                    }
                    printf("   .");
                    for (int i=0; i<3; i++) {
                        printf("%c", extension[i]);
                    }

                    // print first logical cluster
                    error = lseek(infile, 26-11, SEEK_CUR); // offset 26 for first logical cluster - 11 for the 8+3 bytes we read for filename and extension
                    if (error == -1) {
                        printf("Error: lseek(2) failed: %d\n", errno);
                        return 1;
                    }
                    short first_logical_cluster;
                    error = read(infile, &first_logical_cluster, 2);
                    if (error == -1) {
                        printf("Error: read(2) failed: %d\n", errno);
                        return 1;
                    }
                    printf(" first cluster=%d", first_logical_cluster);

                    // print file size
                    int file_size;
                    error = read(infile, &file_size, 4);
                    if (error == -1) {
                        printf("Error: read(2) failed: %d\n", errno);
                        return 1;
                    }
                    printf(" size=%d\n", file_size);
                }

            } else if (strcmp(argv[2], "-cat") == 0) {
                if (argc != 4) {
                    printf("Usage: fat12 [-v] image_name [-dir|-info|-cat filename]\n");
                    return 1;
                }

                for (int i=0; i<10; i++) {
                    // print filename
                    char filename[8];
                    memset(filename, 0, 8);
                    error = read(infile, filename, 8);
                    if (error == -1) {
                        printf("Error: read(2) failed: %d\n", errno);
                        return 1;
                    }
                    // for (int j=0; j<8; j++) {
                    //     printf("%c", filename[j]);
                    // }

                    // strips extension off of cli filename to properly compare to filename found in fs
                    char extension_stripped[8];
                    memset(extension_stripped, 0, 8);
                    for (int j=0; j<8; j++) {
                        if (j != 0 && argv[3][j] == '.') {
                            for (j=j; j<8; j++) {
                                extension_stripped[j] = ' ';
                            }
                        }
                        extension_stripped[j] = argv[3][j];
                    }
                    
                    for (int j=0; j<8; j++) {
                        if (extension_stripped[j] == filename[j]) {
                            if (j == 7) { 
                                // once you've found the correct file to cat then...
                                error = lseek(infile, 18, SEEK_CUR); // offset of First Logical Cluster
                                if (error == -1) {
                                    printf("Error: lseek(2) failed: %d\n", errno);
                                    return 1;
                                }

                                short first_logical_cluster;
                                error = read(infile, &first_logical_cluster, 2);
                                if (error == -1) {
                                    printf("Error: read(2) failed: %d\n", errno);
                                    return 1;
                                }
                                // printf("first cluster=%d\n", first_logical_cluster);

                                // print file size
                                int file_size;
                                error = read(infile, &file_size, 4);
                                if (error == -1) {
                                    printf("Error: read(2) failed: %d\n", errno);
                                    return 1;
                                }
                                // printf("size=%d\n", file_size);

                                // physical sector number = 33 + FAT entry number - 2
                                int physical_sector_number = 33 + first_logical_cluster - 2;
                                error = lseek(infile, physical_sector_number*bytes_per_sector, SEEK_SET); // offset of file ----- sector number * bytes per sector
                                if (error == -1) {
                                    printf("Error: lseek(2) failed: %d\n", errno);
                                    return 1;
                                }

                                for (int i=0; i<file_size; i++) {
                                    char buffer[1];
                                    error = read(infile, buffer, 1);
                                    if (error == -1) {
                                        printf("Error: read(2) failed: %d\n", errno);
                                        return 1;
                                    }
                                    printf("%c", buffer[0]);
                                }
                                printf("\n");
                            }
                        } else {
                            break;
                        }
                    }


                    error = lseek(infile, 24, SEEK_CUR);
                    if (error == -1) {
                        printf("Error: lseek(2) failed: %d\n", errno);
                        return 1;
                    }
                }
            }

        } else {
            printf("%s\n", argv[2]);
            printf("Usage: fat12 [-v] image_name [-dir|-info|-cat filename]\n");
            return 1;
        }


        printf("Success\n");
    }
}
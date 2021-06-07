#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PARAMS_ERROR 1
#define FILE_OPENING_ERROR 2
#define READING_ERROR 3
#define WRITING_ERROR 4
#define MMAP_ERROR 5

#define NO_ARGS 1

int main(int argc, char** argv) {
    // the first arg is the file name
    int ret_value = 0;
    int fd = -1;
    off_t file_size = -1;
    char* data = NULL;

    // verify arguments
    if (argc != NO_ARGS + 1) {
        printf("Usage: %s <file name>\n", argv[0]);
        ret_value = PARAMS_ERROR;
        goto cleanup;
    }

    // open file
    fd = open(argv[1], O_RDWR); //todo: access rights
    if (fd < 0) {
        printf("Error opening file\n");
        ret_value = FILE_OPENING_ERROR;
        goto cleanup;
    }

    // get file size
    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // map file into memory
    data = (char*) mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        printf("Error mapping file\n");
        ret_value = MMAP_ERROR;
        goto cleanup;
    }

    // reading and writing
    for (int i = 0; i < file_size; i++) {
        data[i]++;
    }

    cleanup:
    if (fd >= 0) close(fd);
    if (data != NULL && data != MAP_FAILED) munmap(data, file_size);
    return ret_value;
}
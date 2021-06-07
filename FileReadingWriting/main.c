#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PARAMS_ERROR 1
#define FILE_OPENING_ERROR 2
#define READING_ERROR 3
#define WRITING_ERROR 4

#define NO_ARGS 1

int main(int argc, char** argv) {
    // the first arg is the file name
    int ret_value = 0;
    int fd = -1;
    off_t file_size = -1;

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

    // writing
    char w = 'a';
    long no_written = write(fd, &w, 1);
    if (no_written < 1) { //todo: value
        printf("Writing error\n");
        ret_value = WRITING_ERROR;
        goto cleanup;
    }

    // reading
    lseek(fd, 0, SEEK_SET);
    char r;
    long no_read = read(fd, &r, 1);
    if (no_read < 1) { //todo: value
        printf("Reading error\n");
        ret_value = READING_ERROR;
        goto cleanup;
    }

    cleanup:
    if (fd >= 0) close(fd);
    return 0;
}

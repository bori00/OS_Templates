#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define PIPE_NAME "mypipe"

#define PIPE_CREATING_ERROR 1
#define PIPE_OPENING_ERROR 2
#define WRITING_ERROR 3
#define READING_ERROR 4

int main(int argc, char** argv) {
    int ret_value = 0;

    int fd[2];
    if (pipe(fd) < 0) {
        printf("Error at pipe creation");
        ret_value = PIPE_CREATING_ERROR;
        goto cleanup;
    }

    // read, write
    char c1, c2;
    c1 = 'a';
    if (write(fd[1], &c1, 1) < 1) {
        printf("Error at writing");
        ret_value = WRITING_ERROR;
        goto cleanup;
    }
    if (read(fd[0], &c2, 1) < 1) {
        printf("Error at reading");
        ret_value = READING_ERROR;
        goto cleanup;
    }
    printf("C2 = %c\n", c2);


    cleanup:
    unlink(PIPE_NAME);
    close(fd[1]);
    close(fd[0]);
    return ret_value;
}
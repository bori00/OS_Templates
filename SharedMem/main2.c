#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include "mem.h"

int main(int argc, char** argv) {
    // the first arg is the file name
    int ret_value = 0;
    int shm_fd = -1;
    char* data = NULL;
    int sem_id;

    // open shm
    shm_fd = shm_open(SHM_NAME, O_RDONLY, 0600);
    if (shm_fd < 0) {
        printf("Error opening shared mem\n");
        ret_value = SHM_OPENING_ERROR;
        goto cleanup;
    }

    // create semaphores
    sem_id = semget(SEM_NR, NO_SEMAPHORES, 0600);
    if (semget < 0) {
        printf("Error creating semaphores\n");
        ret_value = SEM_CREATE_ERROR;
        goto cleanup;
    }

    // map file into memory
    data = (char*) mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        printf("Error mapping file\n");
        ret_value = MMAP_ERROR;
        goto cleanup;
    }

    // reading and writing
    for (int i = 0; i < SHM_SIZE; i++) {
        P(sem_id, MUTEX_12);
        printf("%c ", data[i]);
        V(sem_id, MUTEX_21);
    }

    cleanup:
    if (shm_fd >= 0) close(shm_fd);
    if (data != NULL && data != MAP_FAILED) munmap(data, SHM_SIZE);
    return ret_value;
}
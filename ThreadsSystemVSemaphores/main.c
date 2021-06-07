#include <stdio.h>
#include <bits/pthreadtypes.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/** -lpthread */

#define NO_ARGS 1
#define NO_SEMAPHORES 2
#define MUTEX 0
#define SEM_NO_NON_FINISHED_THREADS 1

#define PARAMS_ERROR 1
#define THREAD_CREATE_ERROR 2
#define SEM_CREATE_ERROR 3

int shared_variable = 0;
int sem_id = -1;

void P(int semid, int sem_no)
{
    struct sembuf op = {sem_no, -1, 0};
    semop(semid, &op, 1);
}

void V(int semid, int sem_no)
{
    struct sembuf op = {sem_no, +1, 0};
    semop(semid, &op, 1);
}

void SEMOP(int semid, int sem_no, short value)
{
    struct sembuf op = {sem_no, value, 0};
    semop(semid, &op, 1);
}

void WAIT_FOR_0(int semid, int sem_no)
{
    struct sembuf op = {sem_no, 0, 0};
    semop(semid, &op, 1);
}

void* thread_function(void* arg) {
    int id = *((int*) arg);
    printf("Thread %d\n", id);
    P(sem_id, MUTEX);
    shared_variable++;
    printf("Thread %d set shared variable to %d\n", id, shared_variable);
    V(sem_id, MUTEX);
    P(sem_id, SEM_NO_NON_FINISHED_THREADS);
    int* res = (int*) malloc(sizeof(int));
    *res = id;
    return res;
}

int main(int argc, char** argv) {
    // the first arg is the  number of threads
    int ret_value = 0;
    pthread_t* th_ids = NULL;
    int** th_args = NULL;
    int no_threads = -1;

    // verify arguments
    if (argc != NO_ARGS + 1) {
        printf("Usage: %s <no threads>\n", argv[0]);
        ret_value = PARAMS_ERROR;
        goto cleanup;
    }
    no_threads = atoi(argv[1]);

    // malloc and arg setup
    th_ids = (pthread_t*) malloc(sizeof(pthread_t) * no_threads);
    th_args = (int**) malloc(sizeof(int*) * no_threads);
    for (int i = 0; i < no_threads; i++) {
        th_args[i] = (int*) malloc(sizeof(int) * 1); // todo
        th_args[i][0] = i;
    }

    // create semaphores
    sem_id = semget(IPC_PRIVATE, NO_SEMAPHORES, IPC_CREAT | 0600);
    if (semget < 0) {
        printf("Error creating semaphores\n");
        ret_value = SEM_CREATE_ERROR;
        goto cleanup;
    }
    for (int i = 0; i < NO_SEMAPHORES; i++) {
        semctl(sem_id, i, SETVAL, 0);
    }
    semctl(sem_id, MUTEX, SETVAL, 1);
    semctl(sem_id, SEM_NO_NON_FINISHED_THREADS, SETVAL, no_threads);

    // create threads
    for (int i = 0; i < no_threads; i++) {
        if (pthread_create(&th_ids[i], 0, thread_function, th_args[i]) != 0) {
            printf("Error creating thread %d\n", i);
            ret_value = THREAD_CREATE_ERROR;
            goto cleanup;
        }
    }

    WAIT_FOR_0(sem_id, SEM_NO_NON_FINISHED_THREADS);
    printf("All threads finished\n");

    // wait for threads
    int* res;
    for (int i = 0; i < no_threads; i++) {
        pthread_join(th_ids[i], (void**) &res);
        printf("Thread %d returned %d\n", i, *res);
        free(res);
    }

    cleanup:
    if (th_ids) {
        free(th_ids);
    }
    if (th_args) {
        for (int i = 0; i < no_threads; i++) {
            free(th_args[i]);
        }
        free(th_args);
    }
    if (sem_id != -1) {
        semctl(sem_id, 0, IPC_RMID, 0);
    }
    return ret_value;
}

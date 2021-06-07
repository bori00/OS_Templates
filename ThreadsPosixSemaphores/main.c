#include <stdio.h>
#include <bits/pthreadtypes.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/** -lpthread */

#define NO_ARGS 1
#define NO_SEMAPHORES 2
#define MUTEX 0
#define SEM_NO_NON_FINISHED_THREADS 1

#define PARAMS_ERROR 1
#define THREAD_CREATE_ERROR 2
#define SEM_CREATE_ERROR 3

int shared_variable = 0;
sem_t sems[NO_SEMAPHORES] = {0};

void P(sem_t *sem)
{
    sem_wait(sem);
}

void V(sem_t *sem)
{
    sem_post(sem);
}

void* thread_function(void* arg) {
    int id = *((int*) arg);
    printf("Thread %d\n", id);
    P(&sems[MUTEX]);
    shared_variable++;
    printf("Thread %d set shared variable to %d\n", id, shared_variable);
    V(&sems[MUTEX]);
    P(&sems[SEM_NO_NON_FINISHED_THREADS]);
    int* res = (int*) malloc(sizeof(int));
    *res = id;
    return res;
}

int get_sem_init_value(int sem_no, int no_threads) {
    switch(sem_no) {
        case MUTEX:
            return 1;
        case SEM_NO_NON_FINISHED_THREADS:
            return no_threads;
        default:
            return 0;
    }
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
    for (int i = 0; i < NO_SEMAPHORES; i++) {
        sem_init(&sems[i], 0, get_sem_init_value(i, no_threads));
    }

    // create threads
    for (int i = 0; i < no_threads; i++) {
        if (pthread_create(&th_ids[i], 0, thread_function, th_args[i]) != 0) {
            printf("Error creating thread %d\n", i);
            ret_value = THREAD_CREATE_ERROR;
            goto cleanup;
        }
    }

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
    for (int i = 0; i < no_threads; i++) {
        sem_destroy(&sems[i]);
    }
    return ret_value;
}
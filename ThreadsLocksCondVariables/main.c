#include <stdio.h>
#include <bits/pthreadtypes.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/** -lpthread */

#define NO_ARGS

#define PARAMS_ERROR 1
#define THREAD_CREATE_ERROR 2
#define LOCK_INIT_ERROR 3
#define COND_VAR_INIT_ERROR 4
#define LOCK_ERROR 5
#define SIGNAL_ERROR 6
#define WAIT_ERROR 7
#define BROADCAST_ERROR 8
#define LOCK_RELEASE_ERROR 9

int shared_variable = 0;
int no_threads_in_crit_reg = 0;

int init_synch_mech = 0;
pthread_mutex_t lock;
pthread_cond_t cond;

void* thread_function(void* arg) {
    int id = *((int*) arg);
    printf("Thread %d\n", id);

    //enter
    if (pthread_mutex_lock(&lock) != 0) {
        perror("Cannot take the lock");
        exit(LOCK_ERROR);
    }
    while (no_threads_in_crit_reg == 1) {
        if (pthread_cond_wait(&cond, &lock) != 0) {
            perror("Cannot wait for cond");
            exit(WAIT_ERROR);
        }
    }
    no_threads_in_crit_reg++;
    if (pthread_mutex_unlock(&lock) != 0) {
        perror("Cannot release the lock");
        exit(LOCK_RELEASE_ERROR);
    }

    //critical region
    shared_variable++;
    printf("Thread %d set shared variable to %d\n", id, shared_variable);


    //exit
    if (pthread_mutex_lock(&lock) != 0) {
        perror("Cannot take the lock");
        exit(LOCK_ERROR);
    }
    // exiting critical region
    no_threads_in_crit_reg--;
    if (pthread_cond_signal(&cond)  != 0) {
        perror("Cannot signal the condition waiters");
        exit(SIGNAL_ERROR);
    }
    if (pthread_mutex_unlock(&lock) != 0) {
        perror("Cannot release the lock");
        exit(LOCK_RELEASE_ERROR);
    }

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

    // create locks and condition variables
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Cannot initialize the lock");
        ret_value = LOCK_INIT_ERROR;
        goto cleanup;
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("Cannot initialize the condition variable");
        ret_value = COND_VAR_INIT_ERROR;
        goto cleanup;
    }
    init_synch_mech = 1;

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
    if (init_synch_mech) {
        if (pthread_mutex_destroy(&lock) != 0) {
            perror("Cannot destroy the lock");
            exit(8);
        }
        if (pthread_cond_destroy(&cond) != 0) {
            perror("Cannot destroy the condition variable");
            exit(9);
        }
    }
    return ret_value;
}
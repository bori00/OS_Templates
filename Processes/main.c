#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NO_ARGS 1

#define PARAMS_ERROR 1
#define FORK_ERROR 2

int process(int id) {
    printf("Process %d, pid = %d, from parent %d\n", id, getpid(), getppid());
    return id;
}


int main(int  argc, char** argv) {
    // the first arg is the  number of processes
    int ret_value = 0;
    pid_t* pids = NULL;

    // verify arguments
    if (argc != NO_ARGS + 1) {
        printf("Usage: %s <no processes>\n", argv[0]);
        ret_value = PARAMS_ERROR;
        goto cleanup;
    }
    int no_processes = atoi(argv[1]);

    // create n processes
    pids = (pid_t*) malloc(sizeof(pid_t) * no_processes);
    for (int i = 0; i < no_processes; i++) {
        pids[i] = fork();
        switch (pids[i]) {
            case -1:
                printf("Error at fork\n");
                ret_value = FORK_ERROR;
                goto cleanup;
            case 0:
                // child
                ret_value = process(i);
                return ret_value;
            default:
                continue;
        }
    }

    // wait for all processes
    int status;
    for (int i = 0; i < no_processes; i++) {
        waitpid(pids[i], &status, 0);
        printf("Process %d returned %d\n", pids[i], WEXITSTATUS(status));
    }

    cleanup:
    if (pids) {
        free(pids);
    }
    return ret_value;
}

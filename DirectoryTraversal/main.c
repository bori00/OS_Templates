#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define PARAMS_ERROR 1
#define STAT_ERROR 2
#define DIR_OPENING_ERROR 3

#define NO_ARGS 1

#define MAX_PATH_LENGTH 500

int traverse_dir(char* dir_name) {
    int ret_value = 0;
    DIR* dir = NULL;
    struct dirent* dir_entry;

    // open directory
    dir = opendir(dir_name);
    if (dir == NULL) {
        printf("Error opening dir: %s\n", dir_name);
        ret_value = DIR_OPENING_ERROR;
        goto traversal_cleanup;
    }

    printf("Directory: %s\n", dir_name);

    // traverse directory
    char entry_path[MAX_PATH_LENGTH+1] = {0};
    struct stat inode;
    while((dir_entry = readdir(dir)) != 0) {
        if (strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, "..") != 0) {
            snprintf(entry_path, MAX_PATH_LENGTH, "%s/%s", dir_name, dir_entry->d_name);

            lstat(entry_path, &inode);
            if (S_ISDIR(inode.st_mode)) {
                traverse_dir(entry_path);
            } else if (S_ISREG(inode.st_mode)) {
                printf("Regular File: %s\n", entry_path);
            } else if (S_ISLNK(inode.st_mode)) {
                printf("Link: %s\n", entry_path);
            } else {
                printf("Unknown file: %s\n", entry_path);
            }
        }
    }

    traversal_cleanup:
    if (dir) {
        closedir(dir);
    }
    return ret_value;
}

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

    // verify that the given argument refers to a directory
    struct stat inode;
    if (lstat(argv[1], &inode) < 0) {
        printf("Error getting inode");
        ret_value = STAT_ERROR;
        goto cleanup;
    }
    if (!S_ISDIR(inode.st_mode)) {
        printf("The first argument must refer to a directory!\n");
        ret_value = PARAMS_ERROR;
        goto cleanup;
    }

    traverse_dir(argv[1]);
    cleanup:
    return ret_value;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vfs.h"


int vfs_user_get_index(const char *username, VirtualFileSystem *vfs)
{
    for (unsigned int i=0; i < vfs->user_count;) {
        if (strcmp(vfs->user[i].name, username) == 0 && vfs->user_count < MAX_USERS) {
            return i;
        }
        i++;
    }
    return -1; // Error: User not found
}

int vfs_user_get_permission(const char *username, VirtualFileSystem *vfs)
{
    int user_index = vfs_user_get_index(username, vfs);
    if (strcmp(vfs->user[user_index].name, username) == 0) {
        return vfs->user[user_index].permission;
    }
    return -1; // User not found
}

void vfs_user_get_list(VirtualFileSystem *vfs)
{
    for (unsigned int i=0; i<vfs->user_count;) {
        printf("%s %s %i\n", vfs->user[i].name, vfs->user[i].passwd, vfs->user[i].permission);
        i++;
    }
}

int vfs_user_create(const char *username,
                        const char *password,
                        int permission,
                        VirtualFileSystem *vfs)
{
    if (vfs->user_count >= MAX_USERS) {
        return -1; // Maximum users reached
    }

    for (unsigned int i = 0; i < vfs->user_count; i++) {
        if (strcmp(vfs->user[i].name, username) == 0) {
            return -2; // User already exists
        }
    }

    vfs->user[vfs->user_count].permission = permission;
    vfs->user[vfs->user_count].name = strdup(username);
    vfs->user[vfs->user_count].passwd = strdup(password);

    vfs->user_count++;
    return 0;
}

int vfs_user_authenticate(const char *username, const char *password, VirtualFileSystem *vfs)
{
    for (unsigned int i = 0; i < vfs->user_count; i++) {
        if (strcmp(vfs->user[i].name, username) == 0 && strcmp(vfs->user[i].passwd, password) == 0) {
            return i; // User authenticated, return user index
        }
    }
    return -1; // Authentication failed
}

// my changes
char *vfs_user_get_name(int user_index, VirtualFileSystem *vfs)
{
    return vfs->user[user_index].name;
}
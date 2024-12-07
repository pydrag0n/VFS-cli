#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/vfs.h"
VirtualFileSystem main_vfs;

VirtualFileSystem *get_main_vfs()
{
    return &main_vfs;
}

int read_file_cmd(const char *filename)
{
    char *content;
    content = strdup(vfs_file_read(filename, get_main_vfs()));

    if (content == NULL) {
        printf("Error reading file: %s\n", filename);
        return -1;
    }

    printf("%s\n", content);
}

int create_file_cmd(const char *filename)
{
    int err = vfs_file_create(filename, get_main_vfs());

    if (err < 0)
    {
        printf("Error creating file: %s\n", filename);
        return -1;
    }

    printf("File created successfully: %s\n", filename);
}

int write_file_cmd(const char *filename, char mode, const char *content)
{

        int err = vfs_file_write(filename, content, mode, get_main_vfs());

        if (err < 0) {
            printf("Error written file: %s\n", filename);
            return -1;
        }

        printf("File written successfully: %s\n", filename);
}


int create_user_cmd(const char *username, const char *passwd, int permission)
{

    int err = vfs_user_create(username, passwd, permission, get_main_vfs());
    if (err < 0)
    {
        printf("Error creating user\n");
        return -1;
    }
    printf("User created successfully\n");

}

void user_command(int argc, char *argv[])
{
    if (argc > 7)
    {
        printf("arguments error: use help user\n");
        return;
    }

    if (argv[1][0]=='-') {

        switch (argv[1][1]) {
        case 'l':
            vfs_user_get_list(get_main_vfs());
            break;

        case 'c':
            const char *username = strdup(argv[2]);
            const char *passwd = strdup(argv[3]);
            int permission = atoi(argv[4]);
            create_user_cmd(username, passwd, permission);
            break;

        case 's':
            const char *username_auth = strdup(argv[2]);
            const char *passwd_auth = strdup(argv[3]);
            int user_index = vfs_user_authenticate(username_auth, passwd_auth, get_main_vfs());

            if (user_index >= 0) {
                printf("User authenticated successfully. User index: %d\n", user_index);
                vfs_set_main_user(user_index);
            } else {
                printf("Authentication failed\n");
            }
            break;

        default:
            printf("argument not found\n");
            break;
        }
    }
    return;


}


void file_command(int argc, char *argv[])
{
    if (argc > 6)
    {
        printf("arguments error: use help file\n");
        return;
    }
    const char *filename = strdup(argv[1]);


    if (argv[2][0]=='-') {

        switch (argv[2][1]) {
        case 'r':
            read_file_cmd(filename);
            break;

        case 'c':
            create_file_cmd(filename);
            break;

        case 'w':
            char mode = argv[3][0];
            const char *content = argv[4];

            write_file_cmd(filename, mode, content);
            break;

        // case 'l':
        //     for (int i=0; i < main_vfs.count;) {
        //         vfs_file_get_metadata_min(i, &main_vfs);
        //         i++;
        //     }

            break;
        case 'i':
                char *info = vfs_file_get_metadata_min(vfs_file_get_index(filename, get_main_vfs()), get_main_vfs());
                printf("%s\n", info);
            break;
        default:
            printf("argument not found\n");
            break;
        }
    }
    return;

}


// char *get_main_vfs_name(int user_index)
// {
//     return main_vfs->user[];
// }
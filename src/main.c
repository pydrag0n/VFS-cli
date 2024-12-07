#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/vfs.h"
#include "include/commands.h"

// Массив команд
struct command cmds[] = {
    {"file", file_command, "Create file"},
    {"user", user_command, "Create user"},
    {NULL, NULL, NULL}
};
void process_command(char *input) {
    if (input[0] == '\0') {
        return;
    }
    char *argv[10];
    int argc = 0;

    char *start = input;
    char *end;


    while (*start != '\0' && argc < 10) {

        if (*start == '\"') {

            end = strchr(start + 1, '\"');
            if (end != NULL) {

                int length = end - start - 1;
                argv[argc] = (char *)malloc(length + 1);
                strncpy(argv[argc], start + 1, length);
                argv[argc][length] = '\0';
                argc++;
                start = end + 1;
            } else {

                printf("error\n");
                return;
            }
        } else {
            end = strchr(start, ' ');
            if (end == NULL) {

                argv[argc++] = strdup(start);
                break;
            } else {

                int length = end - start;
                argv[argc] = (char *)malloc(length + 1);
                strncpy(argv[argc], start, length);
                argv[argc][length] = '\0';
                argc++;
                start = end + 1;
            }
        }
    }

    for (int i = 0; cmds[i].name != NULL; i++) {
        if (strcmp(argv[0], cmds[i].name) == 0) {
            cmds[i].func(argc, argv);

            for (int j = 0; j < argc; j++) {
                free(argv[j]);
            }
            return;
        }
    }

    printf("Command not found: %s\n", argv[0]);

    for (int j = 0; j < argc; j++) {
        free(argv[j]);
    }
}
int main() {
    VirtualFileSystem *vfs = get_main_vfs();
    vfs->count = 0;
    vfs->user_count = 0;

    vfs_user_create("root", "root", 4, vfs);
    int im = vfs_user_get_index("root", vfs);
    vfs_set_main_user(im);

    while (1) {
        char input[100];

        printf("<vfs:%s> ", vfs_user_get_name(vfs_main_user, vfs));
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';
        process_command(input);
    }

    return 0;
}

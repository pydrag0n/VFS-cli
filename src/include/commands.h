
typedef void (*command_func_t)(int argc, char *argv[]);

struct command {
    char *name;
    command_func_t func; // Указатель на функцию
    char *description;
};

VirtualFileSystem *get_main_vfs(void);
void user_command(int argc, char *argv[]);
void file_command(int argc, char *argv[]);
// void switch_vfs(VirtualFileSystem *new_vfs);


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vfs.h"


int vfs_main_user = -1;

char *vfs_get_datetime()
{
        int date_size = 256;
        char *datetime = malloc(date_size);
        if (datetime == NULL) {
            return NULL;
        }

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        sprintf(datetime, "%s  %02d:%02d", __DATE__, timeinfo->tm_hour,  timeinfo->tm_min);

        return datetime;
}

char *vfs_file_get_name(int file_index, VirtualFileSystem *vfs)
{
    if (file_index < vfs->count) {
        return vfs->file[file_index].name;
    }
    return NULL;
}

long vfs_file_get_size(int file_index, VirtualFileSystem *vfs)
{
    if (file_index < vfs->count) {
        return vfs->file[file_index].metadata.size;
    }
    return -1;
}

char *vfs_file_get_content(int file_index, VirtualFileSystem *vfs)
{
    char *content = vfs->file[file_index].content;
    if (file_index < vfs->count && content != NULL) {
        return content;
    }
    return NULL;
}


int vfs_file_get_index(const char *filename, VirtualFileSystem *vfs)
{
    for (unsigned int i = 0; i < vfs->count;) {
        if (strcmp(vfs->file[i].name, filename) == 0) {
            return i;
        }
        i++;
    }

    // printf("%s", "File not found"); // debug
    return -1;
}

int vfs_file_create(const char *filename, VirtualFileSystem *vfs)
{

    if (vfs_file_get_index(filename, vfs) > 0) {
        return -1; // File already exists
    }

    if (vfs->count >= MAX_FILES) {
        return -1; // Maximum file limit reached
    }

    vfs->file[vfs->count].name = malloc(strlen(filename) + 1);

    if (vfs->file[vfs->count].name == NULL) {
        return -1;
    }

    if (vfs->user[vfs_main_user].permission < READ_WRITE) {
        return -2; // Permission denied
    }

    strcpy(vfs->file[vfs->count].name, filename);
    vfs->file[vfs->count].content = NULL;

    vfs->file[vfs->count].metadata.create_date = malloc(256);
    vfs->file[vfs->count].metadata.change_date = malloc(256);
    vfs->file[vfs->count].metadata.open_date = malloc(256);

    char *datetime_str = vfs_get_datetime();

    if (datetime_str != NULL) {
        strcpy(vfs->file[vfs->count].metadata.create_date, datetime_str);
        strcpy(vfs->file[vfs->count].metadata.change_date, datetime_str);
        strcpy(vfs->file[vfs->count].metadata.open_date, datetime_str);
        free(datetime_str);
    }

    vfs->file[vfs->count].metadata.size = DEFAULT_FILE_SIZE;

    vfs->count++;

    return 0;

}


char *vfs_file_read(const char *filename, VirtualFileSystem *vfs)
{

    if (vfs->user[vfs_main_user].permission < ONLY_READ) {
        return NULL; // Permission denied
    }

    int file_index = vfs_file_get_index(filename, vfs);
    if (file_index >= 0 && strcmp(vfs->file[file_index].name, filename) == 0) {
        if (vfs->file[file_index].content == NULL) {
            printf("%s", "File is empty\n"); // debug
            return NULL;
        }

        char *datetime_str = vfs_get_datetime();

        if (datetime_str != NULL) {
            strcpy(vfs->file[file_index].metadata.open_date, datetime_str);
        }

        return vfs->file[file_index].content;
    }

    return NULL;

}


int vfs_file_write(const char *filename,
                    const char *content,
                    char mode,
                    VirtualFileSystem *vfs)
{

    /*
     *   mode: 1  = write
     *   mode: 2  = append
     */

    int file_index = vfs_file_get_index(filename, vfs);
    if (file_index >= 0) {
        if (vfs->user[vfs_main_user].permission < READ_WRITE) {
            return -2; // Permission denied
        }

        switch (mode) {
        case 'w':

            if (vfs->file[file_index].content != NULL) {
                free(vfs->file[file_index].content);
            }

            vfs->file[file_index].content = malloc(strlen(content) + 1);

            if (vfs->file[file_index].content == NULL) {  return -1;  }

            strcpy(vfs->file[file_index].content, content);
            break;

        case 'a':

            if (vfs->file[file_index].content != NULL) {
                vfs->file[file_index].content = realloc(vfs->file[file_index].content,
                                                        strlen(content) +
                                                        strlen(vfs->file[file_index].content) + 1);

                strcat(vfs->file[file_index].content, content);
            } else {
                vfs->file[file_index].content = malloc(strlen(content) + 1);
                strcpy(vfs->file[file_index].content, content);
            }
            break;

        default:
            return -1;
            break;
        }

        char *datetime_str = vfs_get_datetime();

        if (datetime_str != NULL) {
            strcpy(vfs->file[file_index].metadata.open_date, datetime_str);
            strcpy(vfs->file[file_index].metadata.change_date, datetime_str);
        }

        vfs->file[file_index].metadata.size = strlen(vfs->file[file_index].content) + 1;

        return 0;
    }

    return -1;

}


int vfs_file_delete(const char *filename, VirtualFileSystem *vfs)
{
    if (vfs->user[vfs_main_user].permission < READ_WRITE_DEL) {
        return -1;
    }

    int file_index = vfs_file_get_index(filename, vfs);
    if (file_index < 0) {
        return -1;
    }

    free(vfs->file[file_index].name);
    free(vfs->file[file_index].content);
    free(vfs->file[file_index].metadata.create_date);
    free(vfs->file[file_index].metadata.change_date);
    free(vfs->file[file_index].metadata.open_date);

    for (unsigned int i = file_index; i < vfs->count - 1; i++) {
        vfs->file[i] = vfs->file[i + 1];
    }

    vfs->count--;

    return 0;
}


char *vfs_file_get_metadata(const char *filename, VirtualFileSystem *vfs)
{

    int file_index = vfs_file_get_index(filename, vfs);

    if (file_index >= 0 && file_index < vfs->count) {
        unsigned int i = file_index;

        const char *create_date = vfs->file[i].metadata.create_date
                                ? vfs->file[i].metadata.create_date : "N/A";

        const char *change_date = vfs->file[i].metadata.change_date
                                ? vfs->file[i].metadata.change_date : "N/A";

        const char *open_date = vfs->file[i].metadata.open_date
                                ? vfs->file[i].metadata.open_date : "N/A";

        size_t info_size = strlen(vfs->file[i].name) +
                            strlen(create_date) +
                            strlen(change_date) +
                            strlen(open_date) + 50;

        char *info = malloc(info_size);

        if (info == NULL) {
            printf("Failed to allocate memory\n");
            return NULL;
        }

        sprintf(info, "%s     | %s | %s | %s | %ld",
                                    vfs->file[i].name,
                                    create_date,
                                    change_date,
                                    open_date,
                                    vfs->file[i].metadata.size);

        return info;
    }

    // printf("%s", "File not found");
    return NULL;

}

char *vfs_file_get_metadata_index(int file_index, VirtualFileSystem *vfs)
{

    if (file_index >= 0 && file_index < vfs->count) {
        unsigned int i = file_index;

        const char *create_date = vfs->file[i].metadata.create_date
                                ? vfs->file[i].metadata.create_date : "N/A";

        const char *change_date = vfs->file[i].metadata.change_date
                                ? vfs->file[i].metadata.change_date : "N/A";

        const char *open_date = vfs->file[i].metadata.open_date
                                ? vfs->file[i].metadata.open_date : "N/A";

        size_t info_size = strlen(vfs->file[i].name) +
                            strlen(create_date) +
                            strlen(change_date) +
                            strlen(open_date) + 50;

        char *info = malloc(info_size);
        if (info == NULL) {
            // printf("Failed to allocate memory\n");
            return NULL;
        }

        sprintf(info, "%s     | %s | %s | %s | %ld",
                                    vfs->file[i].name,
                                    create_date,
                                    change_date,
                                    open_date,
                                    vfs->file[i].metadata.size);

        return info;
    }

    // printf("Index out of array\n");
    return NULL;

}


char *vfs_file_get_metadata_min(int file_index, VirtualFileSystem *vfs)
{

    if (file_index >= 0 && file_index < vfs->count) {
        unsigned int i = file_index;

        const char *change_date = vfs->file[i].metadata.change_date
                                ? vfs->file[i].metadata.change_date : "N/A";


        size_t info_size = strlen(vfs->file[i].name) + strlen(change_date)+50;

        char *info = malloc(info_size);
        if (info == NULL) {
            // printf("Failed to allocate memory\n");
            return NULL;
        }

        sprintf(info, "%s %s %ld",
                        vfs->file[i].name,
                        change_date,
                        vfs->file[i].metadata.size);

        return info;
    }

    // printf("Index out of array\n");
    return NULL;

}


void vfs_free_memory(VirtualFileSystem *vfs)
{
    for (unsigned int i = 0; i < vfs->count; i++) {
        free(vfs->file[i].name);
        free(vfs->file[i].content);
        free(vfs->file[i].metadata.change_date);
        free(vfs->file[i].metadata.create_date);
        free(vfs->file[i].metadata.open_date);
    }
    vfs->count = 0;

}

void vfs_set_main_user(int user_index)
{
    vfs_main_user = user_index;
}


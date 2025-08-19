#include "shell.h"

char *find_in_path(const char *cmd)
{
    char *path_env = getenv("PATH");
    char *path_copy, *dir;
    char full_path[1024];
    char *res = NULL;

    if (!path_env || *path_env == '\0') return NULL;

    path_copy = strdup(path_env);
    if (!path_copy) return NULL;

    dir = strtok(path_copy, ":");
    while (dir)
    {
        if (dir[0] == '\0')
            snprintf(full_path, sizeof(full_path), "./%s", cmd);
        else
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);

        if (access(full_path, X_OK) == 0)
        {
            res = strdup(full_path);
            break;
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return res;
}

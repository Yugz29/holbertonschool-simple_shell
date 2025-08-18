#include "shell.h"

char *find_in_path(const char *cmd)
{
    char *path = getenv("PATH");
    char *p, *token, full_path[1024];

    if (!path)
        return NULL;

    p = strdup(path);
    if (!p)
        return NULL;

    token = strtok(p, ":");
    while (token)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);
        if (access(full_path, X_OK) == 0)
        {
            free(p);
            return strdup(full_path);
        }
        token = strtok(NULL, ":");
    }
    free(p);
    return NULL;
}

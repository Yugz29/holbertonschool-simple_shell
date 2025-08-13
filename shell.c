#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

extern char **environ;

char *find_in_path(const char *cmd)
{
    char *path_env = getenv("PATH");
    char *path_copy, *dir;
    static char full_path[1024];

    if (!path_env)
        return NULL;

    path_copy = strdup(path_env);
    if (!path_copy)
        return NULL;

    dir = strtok(path_copy, ":");
    while (dir)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0)
        {
            free(path_copy);
            return full_path;
        }
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pid_t pid;
    char *token;
    char *argv[1024];
    int i;

    while (1)
    {
        if (isatty(STDIN_FILENO))
        {
            printf("#cisfun$ ");
            fflush(stdout);
        }
        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            if (isatty(STDIN_FILENO))
                putchar('\n');
            break;
        }
        if (read > 0 && line[read - 1] == '\n')
            line[read - 1] = '\0';

        i = 0;
        token = strtok(line, " \t");
        while (token != NULL && i < 1023)
        {
            argv[i++] = token;
            token = strtok(NULL, " \t");
        }
        argv[i] = NULL;

        if (argv[0] == NULL)
            continue;

        pid = fork();
        if (pid == -1)
        {
            perror("./shell");
            continue;
        }
        else if (pid == 0)
        {
            char *cmd_path = NULL;

            if (strchr(argv[0], '/'))
            {
                execve(argv[0], argv, environ);
            }
            else
            {
                cmd_path = find_in_path(argv[0]);
                if (cmd_path)
                {
                    execve(cmd_path, argv, environ);
                }
            }
            fprintf(stderr, "%s: command not found\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        else
        {
            if (waitpid(pid, NULL, 0) == -1)
                perror("waitpid");
        }
    }
    free(line);
    return 0;
}

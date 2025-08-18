#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

extern char **environ;
int interactive = 0;

/**
 * find_in_path - cherche un exécutable dans $PATH
 * @cmd: nom de la commande
 * Return: chemin complet (malloc) ou NULL
 */
char *find_in_path(const char *cmd)
{
    char *path_env = getenv("PATH");
    char *path_copy, *dir;
    char full_path[1024];

    if (!path_env)
        return (NULL);

    path_copy = strdup(path_env);
    if (!path_copy)
        return (NULL);

    dir = strtok(path_copy, ":");
    while (dir)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0)
        {
            free(path_copy);
            return (strdup(full_path));
        }
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    return (NULL);
}

/**
 * handle_sigint - ignore Ctrl+C dans le shell
 * @sig: signal number
 */
void handle_sigint(int sig)
{
    (void)sig;
    if (interactive)
        write(STDOUT_FILENO, "\n$ ", 3);
}

/**
 * print_env - prints environment variables
 */
void print_env(void)
{
    int i = 0;

    while (environ[i])
    {
        printf("%s\n", environ[i]);
        i++;
    }
}

/**
 * main - main function of simple shell
 * Return: 0 on success
 */
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pid_t pid;
    char *token;
    char *argv[1024];
    int i, status;
    char *cmd_path = NULL;

    interactive = isatty(STDIN_FILENO);
    signal(SIGINT, handle_sigint);

    while (1)
    {
        if (interactive)
        {
            write(STDOUT_FILENO, "$ ", 2);
        }

        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            if (interactive)
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        if (read > 0 && line[read - 1] == '\n')
            line[read - 1] = '\0';

        /* Skip empty lines */
        if (strlen(line) == 0)
            continue;

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

        /* Built-in: exit */
        if (strcmp(argv[0], "exit") == 0)
        {
            int exit_status = 0;
            if (argv[1])
                exit_status = atoi(argv[1]);
            free(line);
            exit(exit_status);
        }

        /* Built-in: env */
        if (strcmp(argv[0], "env") == 0)
        {
            print_env();
            continue;
        }

        /* Find command path */
        if (strchr(argv[0], '/'))
        {
            cmd_path = strdup(argv[0]);
        }
        else
        {
            cmd_path = find_in_path(argv[0]);
        }

        if (!cmd_path)
        {
            fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
            if (!interactive)
            {
                free(line);
                exit(127);
            }
            continue;
        }

        /* Fork and execute */
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(cmd_path);
            continue;
        }
        else if (pid == 0)
        {
            /* Child process */
            signal(SIGINT, SIG_DFL);
            
            if (execve(cmd_path, argv, environ) == -1)
            {
                fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
                free(cmd_path);
                exit(127);
            }
        }
        else
        {
            /* Parent process */
            waitpid(pid, &status, 0);
            free(cmd_path);
        }
    }

    free(line);
    return (0);
}
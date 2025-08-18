#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

extern char **environ;

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
            return strdup(full_path); /* <- malloc */
        }
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

/**
 * handle_sigint - ignore Ctrl+C dans le shell
 */
void handle_sigint(int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n#cisfun$ ", 10);
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
    char *cmd_path = NULL;


    /* Ignorer Ctrl+C dans le shell parent */
    signal(SIGINT, handle_sigint);

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

        /* --- Commandes internes --- */
        if (strcmp(argv[0], "exit") == 0)
        {
            free(line);
            exit(0);
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            char *dir = argv[1];

            if (!dir)
                dir = getenv("HOME"); /* cd sans argument => HOME */

            if (!dir || chdir(dir) != 0)
                perror("cd");

            continue; /* on ne fork pas pour cd */
        }

        if (strchr(argv[0], '/'))
            cmd_path = argv[0];
        else
            cmd_path = find_in_path(argv[0]);

        if (!cmd_path)
        {
            fprintf(stderr, "%s: command not found\n", argv[0]);
            continue; /* pas de fork */
        }

        /* --- Fork pour exécuter les autres commandes --- */
        pid = fork();
        if (pid == -1)
        {
            perror("./shell");
            continue;
        }
        else if (pid == 0)
        {
            /* Restaurer le comportement normal de Ctrl+C dans l’enfant */
            signal(SIGINT, SIG_DFL);
            execve(cmd_path, argv, environ);
            perror(argv[0]);
            if (cmd_path != argv[0])
                free(cmd_path);
            exit(EXIT_FAILURE);
        }
        else
        {
            if (waitpid(pid, NULL, 0) == -1)
                perror("waitpid");
            if (cmd_path != argv[0])
                free(cmd_path);
        }
    }

    free(line);
    return 0;
}

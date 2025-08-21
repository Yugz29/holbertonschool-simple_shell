#include "shell.h"

<<<<<<< HEAD
=======
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
>>>>>>> a41d670 (pull)
int main(void)
{
    char *line = NULL, *argv[1024], *tok;
    size_t len = 0;
    ssize_t n;
    pid_t pid;
<<<<<<< HEAD
    int i;
    int status = 0;
    char *cmd;
=======
    char *token;
    char *argv[1024];
    int i, status;
    char *cmd_path = NULL;
>>>>>>> a41d670 (pull)

    signal(SIGINT, SIG_IGN);

    while (1)
    {
        if (isatty(STDIN_FILENO))
        {
            printf("#cisfun$ ");
            fflush(stdout);
        }

        n = getline(&line, &len, stdin);
        if (n == -1)
        {
            if (isatty(STDIN_FILENO)) putchar('\n');
            break;
        }

        if (n > 0 && line[n - 1] == '\n')
            line[n - 1] = 0;

        i = 0;
        tok = strtok(line, " \t");
        while (tok && i < 1023)
        {
            argv[i++] = tok;
            tok = strtok(NULL, " \t");
        }
        argv[i] = NULL;

        if (!argv[0])
            continue;

        /* --- Builtin: exit --- */
        if (!strcmp(argv[0], "exit"))
        {
            if (argv[1])
            {
                if (!is_number(argv[1]))
                {
                    fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
                    free(line);
                    exit(2);
                }
                status = parse_exit_status(argv[1]);
            }
            free(line);
            exit(status);
        }

        /* --- Builtin: cd --- */
        if (!strcmp(argv[0], "cd"))
        {
            if (chdir(argv[1] ? argv[1] : getenv("HOME")) != 0)
                perror("cd");
            continue;
        }

<<<<<<< HEAD
        /* --- Fork & exec --- */
=======
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
>>>>>>> a41d670 (pull)
        pid = fork();
        if (pid == -1)
        {
            perror("./hsh");
            continue;
        }

        if (pid == 0)
        {
            signal(SIGINT, SIG_DFL);

            if (strchr(argv[0], '/'))
            {
                execve(argv[0], argv, environ);
                perror(argv[0]);
                _exit(errno_to_exit(errno));
            }

            cmd = find_in_path(argv[0]);
            if (cmd)
            {
                execve(cmd, argv, environ);
                perror(argv[0]);
                free(cmd);
                _exit(errno_to_exit(errno));
            }

            fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
            _exit(127);
        }
        else
        {
            int wstatus;

            if (waitpid(pid, &wstatus, 0) == -1)
            {
                perror("waitpid");
            }
            else
            {
                if (WIFEXITED(wstatus))
                    status = WEXITSTATUS(wstatus);
                else
                    status = 1;
            }
        }
    }

    free(line);
    return status;
}

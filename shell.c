#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

extern char **environ;

/* --- Prototypes --- */
char *find_in_path(const char *cmd);
void handle_sigint(int sig);
int is_number(const char *s);
int parse_exit_status(const char *s);
int errno_to_exit(int err);

/**
 * find_in_path - cherche un exécutable dans $PATH
 */
char *find_in_path(const char *cmd)
{
    char *path_env = getenv("PATH");
    char *path_copy, *dir;
    char full_path[1024];

    if (!path_env || *path_env == '\0')
        return NULL;

    path_copy = strdup(path_env);
    if (!path_copy)
        return NULL;

    dir = strtok(path_copy, ":");
    while (dir)
    {
        if (dir[0] == '\0')
            snprintf(full_path, sizeof(full_path), "./%s", cmd);
        else
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);

        if (access(full_path, X_OK) == 0)
        {
            char *res = strdup(full_path);
            free(path_copy);
            return res;
        }
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

/**
 * handle_sigint - ignore Ctrl+C dans le shell parent
 */
void handle_sigint(int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n#cisfun$ ", 10);
}

/* renvoie 1 si s est un entier valide, sinon 0 */
int is_number(const char *s)
{
    const char *p = s;

    if (!s || *s == '\0')
        return 0;

    if (*p == '+' || *p == '-')
        p++;

    if (*p == '\0')
        return 0;

    while (*p)
    {
        if (*p < '0' || *p > '9')
            return 0;
        p++;
    }
    return 1;
}

/* parse l’argument d’exit : modulo 256 */
int parse_exit_status(const char *s)
{
    long val = 0;
    int neg = 0;

    if (!s)
        return 0;

    if (*s == '-')
    {
        neg = 1;
        s++;
    }
    else if (*s == '+')
        s++;

    while (*s)
    {
        val = val * 10 + (*s - '0');
        s++;
    }

    if (neg)
        val = -val;

    return (unsigned char)val;
}

/* map errno -> code de sortie pour execve raté */
int errno_to_exit(int err)
{
    if (err == ENOENT)
        return 127;
    if (err == EACCES || err == EPERM)
        return 126;
    return 126;
}

int main(int argc, char **argv)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t readn;
    pid_t pid;
    char *token;
    char *args[1024];
    int i;
    unsigned int line_number = 0;
    char *argv0 = argv[0];

    (void)argc;
    signal(SIGINT, handle_sigint);

    while (1)
    {
        if (isatty(STDIN_FILENO))
        {
            printf("#cisfun$ ");
            fflush(stdout);
        }

        readn = getline(&line, &len, stdin);
        if (readn == -1)
        {
            if (isatty(STDIN_FILENO))
                putchar('\n');
            break;
        }

        line_number++;

        if (readn > 0 && line[readn - 1] == '\n')
            line[readn - 1] = '\0';

        i = 0;
        token = strtok(line, " \t");
        while (token != NULL && i < 1023)
        {
            args[i++] = token;
            token = strtok(NULL, " \t");
        }
        args[i] = NULL;

        if (args[0] == NULL)
            continue;

        /* --- builtins --- */
        if (strcmp(args[0], "exit") == 0)
        {
            int status = 0;
            if (args[1] != NULL)
            {
                if (!is_number(args[1]))
                {
                    fprintf(stderr, "%s: %d: exit: %s: numeric argument required\n",
                            argv0, line_number, args[1]);
                    free(line);
                    exit(2);
                }
                status = parse_exit_status(args[1]);
            }
            free(line);
            exit(status);
        }
        else if (strcmp(args[0], "cd") == 0)
        {
            char *dir = args[1];
            if (!dir)
                dir = getenv("HOME");
            if (!dir || chdir(dir) != 0)
                perror("cd");
            continue;
        }

        /* --- Fork & exec --- */
        pid = fork();
        if (pid == -1)
        {
            perror(argv0);
            continue;
        }
        else if (pid == 0)
        {
            char *cmd_path;
            signal(SIGINT, SIG_DFL);

            if (strchr(args[0], '/'))
            {
                execve(args[0], args, environ);
                perror(args[0]);
                _exit(errno_to_exit(errno));
            }

            cmd_path = find_in_path(args[0]);
            if (cmd_path)
            {
                execve(cmd_path, args, environ);
                perror(args[0]);
                free(cmd_path);
                _exit(errno_to_exit(errno));
            }

            fprintf(stderr, "%s: %d: %s: not found\n", argv0, line_number, args[0]);
            _exit(127);
        }
        else
        {
            int wstatus = 0;
            if (waitpid(pid, &wstatus, 0) == -1)
                perror("waitpid");
        }
    }

    free(line);
    return 0;
}

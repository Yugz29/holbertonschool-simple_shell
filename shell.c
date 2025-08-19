#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

extern char **environ;

/* --- Prototypes --- */
char *find_in_path(const char *cmd);
int errno_to_exit(int err);

int main(void)
{
    char *line = NULL, *argv[1024], *tok;
    size_t len = 0;
    ssize_t n;
    pid_t pid;
    int i;          /* déclaré en haut */
    int status;     /* pour builtin exit */
    char *end;      /* pour strtol */
    long v;         
    char *cmd;      /* pour exec */

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

        /* --- builtins --- */
        if (!strcmp(argv[0], "exit"))
        {
            status = 0;
            if (argv[1])
            {
                v = strtol(argv[1], &end, 10);
                if (*end)
                {
                    fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
                    free(line);
                    exit(2);
                }
                status = (unsigned char)v;
            }
            free(line);
            exit(status);
        }
        if (!strcmp(argv[0], "cd"))
        {
            if (chdir(argv[1] ? argv[1] : getenv("HOME")) != 0)
                perror("cd");
            continue;
        }

        /* --- Fork & exec --- */
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
            if (waitpid(pid, NULL, 0) == -1)
                perror("waitpid");
        }
    }
    free(line);
    return 0;
}

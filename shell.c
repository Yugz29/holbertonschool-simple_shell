#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pid_t pid;

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
            {
                printf("\n");
            }
            break;
        }

        if (line[read - 1] == '\n')
            line[read - 1] = '\0';

        pid = fork();

        if (pid == -1)
        {
            printf("\n");
            break;
        }

        else if (pid == 0)
        {
            char *argv[2];
            argv[0] = line;
            argv[1] = NULL;

            execve(argv[0], argv, environ);

            perror("./shell");
            exit(EXIT_FAILURE);
        }

        else
            (void)waitpid(pid, NULL, 0);

    }
    free(line);
    return (0);
}

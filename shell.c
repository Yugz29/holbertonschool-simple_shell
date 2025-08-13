#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

extern char **environ;

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pid_t pid;
    char *argv[64];
    int i = 0;
    char *token;

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
                putchar('\n');
            }
            break;
        }

        if (read > 0 && line[read - 1] == '\n')
            line[read - 1] = '\0';

        if (line[0] == '\0')
            continue;
        
        i = 0;
        token = strtok(line, " \t");
        
        while (token != NULL && i < 63)
        {
            argv[i++] = token;
            token = strtok(NULL, " \t");
        }
        
        argv[i++] = NULL;
        pid = fork();

        if (pid == -1)
        {
            perror("./shell");
            continue;
        }

        else if (pid == 0)
        {
            execve(argv[0], argv, environ);

            perror("./shell");
            exit(EXIT_FAILURE);
        }

        else
        {
            if (waitpid(pid, NULL, 0) == -1)
                perror("waitpid");
        }

    }
    free(line);
    return (0);
}

#include <unistd.h>  // for write()
#include <stdlib.h>  // for exit(), malloc(), free()
#include <stdio.h>   // for perror()

extern char **environ;

int main(void)
{
    char *line = NULL; // stock the user input
    size_t len = 0; // contains the size allocated for line
    ssize_t read; // number of characters read or -1
    pid_t pid; // process id for fork()

    while (1)
    {
        printf("> ");
        read = getline(&line, &len, stdin);

        if (read == -1) // EOF or error
        {
            printf("\n");
            break;
        }

        // replace the \n at the end
        if (line[read - 1] == '\n')
            line[read - 1] = '\0';

        pid = fork(); // create a child process

        if (pid == -1) // fork failed
        {
            printf("\n");
            break;
        }

        else if (pid == 0) // child process
        {
            char *argv[2];
            argv[0] = line;
            argv[1] = NULL;

            execve(argv[0], argv, environ); // replace child process with
                                            // new program

            perror("execve"); // if execve fails, print error message
            exit(EXIT_FAILURE);
        }

        else // parent process
            wait(NULL);

    }
    free(line);
    return (0);
}

#include <unistd.h>  // for write()
#include <stdlib.h>  // for exit(), malloc(), free()
#include <stdio.h>   // for perror()

int main(void)
{
    char *line = NULL; // user input stock
    size_t len = 0; // contains the size allocated for line
    ssize_t read; // number of characters read or -1

    while (1)
    {
        printf("> ");
        read = getline(&line, &len, stdin);

        if (read == -1) // EOF or error
        {
            printf("\n");
            break;
        }

        // Remove the \n at the end
        if (line[read - 1] == '\n')
            line[read - 1] = '\0';

    }
    free(line);
    return (0);
}

#include <unistd.h>  // for write()
#include <stdlib.h>  // for exit(), malloc(), free()
#include <stdio.h>   // for perror()

int main(void)
{
    char buffer[1024]; //1024 = octets, typical size for a buffer
    ssize_t read_bytes;

    while (1)
    {
        write(STDOUT_FILENO, "> ", 2);
        read_bytes = read(STDIN_FILENO, buffer, 1024);

        if (read_bytes == 0) // for CTRL+D (EOF)
            break;

        if (read_bytes == -1) // if there is an error
            perror("error : ");

        buffer[read_bytes] = '\0'; // add null bytes at the end if it's ok

    }
}

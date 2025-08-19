#include "shell.h"

#define MAX_ARGS 1024

/* Fonction pour exécuter une commande avec redirections */
void execute_command(char **argv, int in_fd, int out_fd)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);

        if (in_fd != STDIN_FILENO) { dup2(in_fd, STDIN_FILENO); close(in_fd); }
        if (out_fd != STDOUT_FILENO) { dup2(out_fd, STDOUT_FILENO); close(out_fd); }

        if (strchr(argv[0], '/')) execve(argv[0], argv, environ);
        else
        {
            char *cmd_path = find_in_path(argv[0]);
            if (cmd_path)
            {
                execve(cmd_path, argv, environ);
                free(cmd_path);
            }
        }
        perror(argv[0]);
        _exit(errno_to_exit(errno));
    }
    else if (pid > 0)
        waitpid(pid, NULL, 0);
    else
        perror("fork");
}

/* Parse line to detect >, <, and |, return argv, in_fd, out_fd, pipe_fd if needed */
void parse_and_execute(char *line)
{
    char *argv[MAX_ARGS], *token;
    int i = 0, in_fd = STDIN_FILENO, out_fd = STDOUT_FILENO;
    int pipe_fd[2];
    char *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];
    int has_pipe = 0, j;

    /* Tokenize by space */
    token = strtok(line, " \t");
    while (token && i < MAX_ARGS-1)
    {
        argv[i++] = token;
        token = strtok(NULL, " \t");
    }
    argv[i] = NULL;

    if (!argv[0]) return;

    /* Check built-ins */
    if (strcmp(argv[0], "exit") == 0)
    {
        int status = 0;
        if (argv[1] && is_number(argv[1])) status = parse_exit_status(argv[1]);
        else if (argv[1])
        {
            fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
            status = 2;
        }
        exit(status);
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        char *dir = argv[1] ? argv[1] : getenv("HOME");
        if (!dir || chdir(dir) != 0) perror("cd");
        return;
    }

    /* Check for pipe */
    for (j = 0; argv[j]; j++)
    {
        if (strcmp(argv[j], "|") == 0)
        {
            argv[j] = NULL;
            has_pipe = 1;
            break;
        }
        else if (strcmp(argv[j], ">") == 0)
        {
            argv[j] = NULL;
            out_fd = open(argv[j+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        }
        else if (strcmp(argv[j], "<") == 0)
        {
            argv[j] = NULL;
            in_fd = open(argv[j+1], O_RDONLY);
        }
    }

    if (has_pipe)
    {
        for (i = 0; i < j; i++) cmd1[i] = argv[i];
        cmd1[i] = NULL;
        for (i = j+1; argv[i]; i++) cmd2[i-j-1] = argv[i];
        cmd2[i-j-1] = NULL;

        if (pipe(pipe_fd) == -1) { perror("pipe"); return; }

        execute_command(cmd1, in_fd, pipe_fd[1]);
        close(pipe_fd[1]);
        execute_command(cmd2, pipe_fd[0], out_fd);
        close(pipe_fd[0]);
    }
    else
        execute_command(argv, in_fd, out_fd);
}

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t readn;

    signal(SIGINT, SIG_IGN);

    while (1)
    {
        if (isatty(STDIN_FILENO)) { printf("#cisfun$ "); fflush(stdout); }

        readn = getline(&line, &len, stdin);
        if (readn == -1) { if (isatty(STDIN_FILENO)) putchar('\n'); break; }

        if (readn > 0 && line[readn-1] == '\n') line[readn-1] = '\0';
        parse_and_execute(line);
    }

    free(line);
    return 0;
}

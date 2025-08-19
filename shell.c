#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

extern char **environ;

char *find_in_path(const char *cmd) {
    char *path = getenv("PATH"), *copy, *dir, full[1024];
    if (!path || !(copy = strdup(path))) return NULL;

    for (dir = strtok(copy, ":"); dir; dir = strtok(NULL, ":")) {
        snprintf(full, sizeof(full), "%s/%s", *dir ? dir : ".", cmd);
        if (!access(full, X_OK)) {
            free(copy);
            return strdup(full);
        }
    }
    free(copy);
    return NULL;
}

int errno_to_exit(int err) {
    return (err == ENOENT) ? 127 : 126;
}

int main(void) {
    char *line = NULL, *argv[1024], *tok;
    size_t len = 0;
    ssize_t n;
    pid_t pid;

    signal(SIGINT, SIG_IGN);

    while (1) {
        if (isatty(STDIN_FILENO)) printf("#cisfun$ "), fflush(stdout);

        if ((n = getline(&line, &len, stdin)) == -1) {
            if (isatty(STDIN_FILENO)) putchar('\n');
            break;
        }
        if (n > 0 && line[n-1] == '\n') line[n-1] = 0;

        int i = 0;
        for (tok = strtok(line, " \t"); tok && i < 1023; tok = strtok(NULL, " \t"))
            argv[i++] = tok;
        argv[i] = NULL;
        if (!argv[0]) continue;

        /* builtins */
        if (!strcmp(argv[0], "exit")) {
            int status = 0;
            if (argv[1]) {
                char *end; long v = strtol(argv[1], &end, 10);
                if (*end) {
                    fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
                    free(line); exit(2);
                }
                status = (unsigned char)v;
            }
            free(line); exit(status);
        }
        if (!strcmp(argv[0], "cd")) {
            if (chdir(argv[1] ? argv[1] : getenv("HOME"))) perror("cd");
            continue;
        }

        /* exec */
        if ((pid = fork()) == -1) { perror("./hsh"); continue; }
        if (!pid) {
            signal(SIGINT, SIG_DFL);
            if (strchr(argv[0], '/')) {
                execve(argv[0], argv, environ);
                perror(argv[0]); _exit(errno_to_exit(errno));
            }
            char *cmd = find_in_path(argv[0]);
            if (cmd) { execve(cmd, argv, environ); perror(argv[0]); free(cmd); }
            fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
            _exit(127);
        }
        else if (waitpid(pid, NULL, 0) == -1) perror("waitpid");
    }
    free(line);
    return 0;
}

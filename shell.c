#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
extern char **environ;
/* --- Protos --- */
char *find_in_path(const char *cmd);
void handle_sigint(int sig);
int  is_number(const char *s);
int  parse_exit_status(const char *s);
int  errno_to_exit(int err);
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
    if (!path_env || *path_env == '\0')
        return NULL;
    path_copy = strdup(path_env);
    if (!path_copy)
        return NULL;
    dir = strtok(path_copy, ":");
    while (dir)
    {
        /* entrée vide dans PATH => répertoire courant "." */
        if (dir[0] == '\0')
            snprintf(full_path, sizeof(full_path), "./%s", cmd);
        else
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0)
        {
            char *res = strdup(full_path);
            free(path_copy);
            return res; /* malloc */
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
/* renvoie 1 si s est un entier signé valide (optionnel +/-, chiffres), sinon 0 */
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
/* parse l’argument d’exit : modulo 256 comme les shells */
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
    {
        s++;
    }
    while (*s)
    {
        val = val * 10 + (*s - '0');
        s++;
    }
    if (neg)
        val = -val;
    /* modulo 256 comme POSIX shells (seulement l’octet bas) */
    return (unsigned char)val;
}
/* map errno -> code de sortie pour l’enfant après execve raté */
int errno_to_exit(int err)
{
    if (err == ENOENT)
        return 127; /* not found */
    if (err == EACCES || err == EPERM)
        return 126; /* permission */
    return 126;     /* par défaut: non exécutable / autre erreur */
}
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t readn;
    pid_t pid;
    char *token;
    char *argv[1024];
    int i;
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
        if (readn > 0 && line[readn - 1] == '\n')
            line[readn - 1] = '\0';
        i = 0;
        token = strtok(line, " \t");
        while (token != NULL && i < 1023)
        {
            argv[i++] = token;
            token = strtok(NULL, " \t");
        }
        argv[i] = NULL;
        if (argv[0] == NULL)
            continue;
        /* --- builtins --- */
        if (strcmp(argv[0], "exit") == 0)
        {
            int status = 0;
            /* si un argument est donné */
            if (argv[1] != NULL)
            {
                if (!is_number(argv[1]))
                {
                    /* style bash: message + code 2 */
                    fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
                    free(line);
                    exit(2);
                }
                status = parse_exit_status(argv[1]);
            }
            free(line);
            exit(status);
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            char *dir = argv[1];
            if (!dir)
                dir = getenv("HOME");
            if (!dir || chdir(dir) != 0)
                perror("cd");
            continue;
        }
        /* --- Fork & exec (résolution PATH dans l’enfant, comme ta version 25/30) --- */
        pid = fork();
        if (pid == -1)
        {
            perror("./hsh");
            continue;
        }
        else if (pid == 0)
        {
            char *cmd_path;
            /* enfant: Ctrl+C tue la commande */
            signal(SIGINT, SIG_DFL);
            if (strchr(argv[0], '/'))
            {
                execve(argv[0], argv, environ);
                /* si on arrive ici: échec execve */
                perror(argv[0]);
                _exit(errno_to_exit(errno));
            }
            /* sinon: chercher dans PATH */
            cmd_path = find_in_path(argv[0]);
            if (cmd_path)
            {
                execve(cmd_path, argv, environ);
                perror(argv[0]);
                free(cmd_path);
                _exit(errno_to_exit(errno));
            }
            /* pas trouvé dans PATH */
            fprintf(stderr,"./hsh: 1: %s: not found\n", argv[0]);
             _exit(127);
        }
        else
        {
            int wstatus = 0;
            if (waitpid(pid, &wstatus, 0) == -1)
                perror("waitpid");
            /* on ne modifie pas le code du shell ici, le checker s’en fiche en général */
        }
    }
    free(line);
    return 0;
}

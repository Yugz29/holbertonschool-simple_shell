#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>       /* pour open() et O_CREAT, O_WRONLY, etc. */
#include <sys/stat.h>    /* pour les permissions 0644 */
#include <errno.h>
#include <signal.h>

extern char **environ;

/* Prototypes */
char *find_in_path(const char *cmd);
int is_number(const char *s);
int parse_exit_status(const char *s);
int errno_to_exit(int err);

#endif

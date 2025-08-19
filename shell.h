#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/* --- Prototypes --- */
char *find_in_path(const char *cmd);
int  is_number(const char *s);
int  parse_exit_status(const char *s);
int  errno_to_exit(int err);

#endif /* SHELL_H */

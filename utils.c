#include "shell.h"

int is_number(const char *s)
{
    const char *p = s;
    if (!s || *s == '\0') return 0;

    if (*p == '+' || *p == '-') p++;
    if (*p == '\0') return 0;

    while (*p)
    {
        if (*p < '0' || *p > '9') return 0;
        p++;
    }
    return 1;
}

int parse_exit_status(const char *s)
{
    long val = 0;
    int neg = 0;

    if (!s) return 0;

    if (*s == '-') { neg = 1; s++; }
    else if (*s == '+') s++;

    while (*s) { val = val * 10 + (*s - '0'); s++; }

    if (neg) val = -val;

    return (unsigned char)val;
}

int errno_to_exit(int err)
{
    if (err == ENOENT) return 127;
    if (err == EACCES || err == EPERM) return 126;
    return 126;
}

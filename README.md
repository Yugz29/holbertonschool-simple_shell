# 🐚 simple\_shell

A **mini Unix shell** written in C, C90 compatible, capable of executing commands and handling the built-in commands `cd` and `exit`.

---

## 🚀 Features

* Execute commands using `fork` and `execve`.
* Built-in commands:

  * `cd [directory]`: change the current working directory.
  * `exit [status]`: exit the shell with a status code.
* Automatic search for executables in the `PATH`.
* Error handling with proper exit codes.
* Ignore `Ctrl+C` interrupts in the main shell.
* Interactive or scriptable mode.

---

## 📁 Project Structure

```
simple_shell/
├── shell.c       # Main loop and command handling
├── utils.c       # Utility functions (is_number, parse_exit_status, errno_to_exit)
├── path.c        # Executable search in PATH
├── shell.h       # Shared header file
└── README.md     # Documentation
```

---

## 🛠️ Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 shell.c utils.c path.c -o hsh
```

---

## 💻 Usage

Run the shell:

```bash
./hsh
```

Example interactive session:

```bash
#cisfun$ ls -l
#cisfun$ cd /tmp
#cisfun$ pwd
#cisfun$ exit 0
```

---

## 📋 Requirements


List of allowed functions and system calls+
```
all functions from string.h
access (man 2 access)
chdir (man 2 chdir)
close (man 2 close)
closedir (man 3 closedir)
execve (man 2 execve)
exit (man 3 exit)
_exit (man 2 _exit)
fflush (man 3 fflush)
fork (man 2 fork)
free (man 3 free)
getcwd (man 3 getcwd)
getline (man 3 getline)
getpid (man 2 getpid)
isatty (man 3 isatty)
kill (man 2 kill)
malloc (man 3 malloc)
open (man 2 open)
opendir (man 3 opendir)
perror (man 3 perror)
printf (man 3 printf)
fprintf (man 3 fprintf)
vfprintf (man 3 vfprintf)
sprintf (man 3 sprintf)
putchar (man 3 putchar)
read (man 2 read)
readdir (man 3 readdir)
signal (man 2 signal)
stat (__xstat) (man 2 stat)
lstat (__lxstat) (man 2 lstat)
fstat (__fxstat) (man 2 fstat)
strtok (man 3 strtok)
wait (man 2 wait)
waitpid (man 2 waitpid)
wait3 (man 2 wait3)
wait4 (man 2 wait4)
write (man 2 write)
```

---

## ⚠️ Limitations

* Redirections (`>`, `<`) and pipes (`|`) not supported.
* Environment variables cannot be modified.
* Arguments containing spaces with quotes are not handled.

---

## ✍️ Author

* Project completed as part of the **Holberton School / ALX curriculum**
* Author: Yann Duzelier and Kevin Herisson

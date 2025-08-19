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

## ⚠️ Limitations

* Redirections (`>`, `<`) and pipes (`|`) not supported.
* Environment variables cannot be modified.
* Arguments containing spaces with quotes are not handled.

---

## ✍️ Author

* Project completed as part of the **Holberton School / ALX curriculum**
* Author: *Your Name Here*

# mini-bash

A minimal bash-like shell implemented in C as part of the Systems Programming course.

The project demonstrates core OS concepts:
- process creation using `fork`
- program execution with `execv`
- parent–child synchronization via `waitpid`
- handling built-in commands (`cd`, `exit`)
- manual command path resolution (HOME → /bin)
- direct interaction with the Linux kernel using system calls

Advanced shell features such as PATH resolution, pipes, redirection, and job control are intentionally omitted.

## Build
```bash
gcc -Wall -Wextra -pedantic -std=c99 mini_bash.c -o mini_bash

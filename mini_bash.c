#include <unistd.h>     // read, write, chdir, getcwd, STDIN_FILENO, STDOUT_FILENO, fork, execvp, _exit
#include <stdlib.h>     // exit, getenv
#include <string.h>     // strcmp, strtok, strlen
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid

#define BUFF_SIZE 4096  // typical Linux page size
#define MAX_ARGS 64

static int parse_command(char *buf, char *argv[], int max_args)
{
    int argc;

    if (buf == NULL || argv == NULL || max_args <= 0)
        return 0;

    argc = 0;

    argv[argc] = strtok(buf, " \t");
    while (argv[argc] != NULL && argc < max_args - 1)
    {
        argc++;
        argv[argc] = strtok(NULL, " \t");
    }

    argv[argc] = NULL;

    write(STDOUT_FILENO, "Parsing succesful\n", sizeof("Parsing succesful\n") - 1);

    return argc;
}

static int execute_external(char *argv[])
{
    pid_t pid;
    int status;

    write(STDOUT_FILENO, "Trying execute\n", sizeof("Trying execute\n") - 1);

    if (argv == NULL || argv[0] == NULL)
        return 0;

    pid = fork();
    if (pid < 0)
        return -1;

    if (pid == 0)
    {
        execvp(argv[0], argv);

        /* exec failed */
        write(STDOUT_FILENO, "Unknown command: ", sizeof("Unknown command: ") - 1);
        write(STDOUT_FILENO, argv[0], strlen(argv[0]));
        write(STDOUT_FILENO, "\n", 1);

        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0)
        return -1;

    write(STDOUT_FILENO, "Done\n", sizeof("Done\n") - 1);

    return 0;
}

static void print_prompt(void)
{
    char cwd[BUFF_SIZE];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        write(STDOUT_FILENO, "mini_bash$ ", sizeof("mini_bash$ ") - 1);
        return;
    }

    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, "$ ", 2);
}

static ssize_t read_command(char *buf, size_t cap)
{
    ssize_t n;

    if (buf == NULL || cap == 0)
        return -1;

    n = read(STDIN_FILENO, buf, cap - 1);
    if (n <= 0)
        return n;

    buf[n] = '\0';

    if (n > 0 && buf[n - 1] == '\n')
        buf[n - 1] = '\0';

    write(STDOUT_FILENO, "Command read. \n", sizeof("Command read. \n") - 1);

    return n;
}

static int handle_command(char *buf)
{
    char *argv[MAX_ARGS];
    int argc;

    write(STDOUT_FILENO, "Handling command \n", sizeof("Handling command \n") - 1);

    /* empty command */
    if (buf[0] == '\0')
    {
        write(STDOUT_FILENO, "Empty command. \n", sizeof("Empty command. \n") - 1);
        return 0;
    }

    /* built-in: exit (no fork) */
    if (strcmp(buf, "exit") == 0)
    {
        write(STDOUT_FILENO, "Built-in  --  exit\n", sizeof("Built-in  --  exit\n") - 1);
        write(STDOUT_FILENO, "Goodbye!\n", sizeof("Goodbye!\n") - 1);
        exit(0);
    }

    /* 1) parse command + args */
    argc = parse_command(buf, argv, MAX_ARGS);
    if (argc == 0)
        return 0;

    /* built-in: cd (no fork) */
    if (strcmp(argv[0], "cd") == 0)
    {
        char *path;

        write(STDOUT_FILENO, "Built-in  --  cd\n", sizeof("Built-in  --  cd\n") - 1);

        if (argv[1] == NULL)
            path = getenv("HOME");
        else
            path = argv[1];

        if (path == NULL)
        {
            write(STDOUT_FILENO, "cd: HOME not set\n", sizeof("cd: HOME not set\n") - 1);
            return 0;
        }

        if (chdir(path) < 0)
        {
            write(STDOUT_FILENO, "cd: failed\n", sizeof("cd: failed\n") - 1);
            return 0;
        }

        return 0;
    }

    /* 3) external commands: fork + exec + wait */
    execute_external(argv);

    return 0;
}

int main(void)
{
    char buf[BUFF_SIZE];

    write(STDOUT_FILENO, "Alive!\n", sizeof("Alive!\n") - 1);

    while (1)                                                       // main loop
    {
        print_prompt();                                             // prompt

        if (read_command(buf, sizeof(buf)) <= 0)                    // read command
        {
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        handle_command(buf);
    }

    return 0;
}
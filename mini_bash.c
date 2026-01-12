#include <unistd.h>     // read, write, chdir, STDIN_FILENO, STDOUT_FILENO
#include <stdlib.h>     // exit
#include <string.h>     // strcmp

#define BUFF_SIZE 4096  // typical Linux page size

static void print_prompt(void);
static ssize_t read_command(char *buf, size_t cap);
static int handle_command(char *buf);

int main(void)
{
    char buf[BUFF_SIZE];

    write(STDOUT_FILENO, "Alive!\n", sizeof("Alive!\n") - 1);

    while (1)
    {
        print_prompt();

        if (read_command(buf, sizeof(buf)) <= 0)
        {
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        handle_command(buf);
    }

    return 0;
}

static void print_prompt(void)
{
    write(STDOUT_FILENO, "mini_bash$ ", sizeof("mini_bash$ ") - 1);
}

static ssize_t read_command(char *buf, size_t cap)
{
    ssize_t n;

    n = read(STDIN_FILENO, buf, cap - 1);
    if (n <= 0)
        return n;

    if (buf[n - 1] == '\n')
        buf[n - 1] = '\0';
    else
        buf[n] = '\0';

    return n;
}

static int handle_command(char *buf)
{
    /* empty command */
    if (buf[0] == '\0')
        return 0;

    /* built-in: exit */
    if (strcmp(buf, "exit") == 0)
    {
        write(STDOUT_FILENO, "Goodbye!\n", 9);
        exit(0);
    }

    /*
        TODO:
        1) parse command + args
        2) built-ins: cd (no fork)
        3) external commands: fork + exec + wait
        4) if not found: print "Unknown command: <name>"
    */

    return 0;
}

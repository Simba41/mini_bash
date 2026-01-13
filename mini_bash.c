#include <unistd.h>     // read, write, chdir, STDIN_FILENO, STDOUT_FILENO
#include <stdlib.h>     // exit
#include <string.h>     // strcmp strtok


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

static void print_prompt(void);
static ssize_t read_command(char *buf, size_t cap);
static int handle_command(char *buf);

int main(void)
{
    char buf[BUFF_SIZE];

    write(STDOUT_FILENO, "Alive!\n", sizeof("Alive!\n") - 1);

    while (1)                                                       //main loop
    {
        print_prompt();                                             //mini_bash promt

        if (read_command(buf, sizeof(buf)) <= 0)                    //reading command 
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
        write(STDOUT_FILENO, "Built-in\n", sizeof("Built-in\n") - 1);
        write(STDOUT_FILENO, "Goodbye!\n", sizeof("Goodbye!\n") - 1);
        exit(0);
    }

    //1) parse command + args
    argc = parse_command(buf, argv, MAX_ARGS);
    if (argc == 0)
        return 0;

    /*
        argv[0] = command
        argv[1..] = args
        argv[argc] = NULL
    */


    /*
        TODO:
        
        2) built-ins: cd (no fork)
        3) external commands: fork + exec + wait
        4) if not found: print "Unknown command: <name>"
    */

    return 0;
}

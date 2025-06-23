#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
char *read_input()
{
    char *line = readline("myshell> ");
    if (!line)
        return NULL;
    if (*line)
        add_history(line);
    return line;
}
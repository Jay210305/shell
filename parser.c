#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n"
command_t *parse_input(char *input)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    command_t *cmd = malloc(sizeof(command_t));
    char *token;
    if (!tokens || !cmd)
    {
        perror("allocation error");
        exit(EXIT_FAILURE);
    }
    token = strtok(input, TOK_DELIM);
    while (token)
    {
        tokens[position++] = strdup(token);
        if (position >= bufsize)
        {
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                perror("reallocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    cmd->argc = position;
    cmd->argv = tokens;
    cmd->background = (position > 0 && strcmp(tokens[position - 1], "&") == 0);
    if (cmd->background)
    {
        free(tokens[position - 1]);
        tokens[position - 1] = NULL;
        cmd->argc--;
    }
    return cmd;
}
void free_command(command_t *cmd)
{
    for (int i = 0; i < cmd->argc; i++)
        free(cmd->argv[i]);
    free(cmd->argv);
    free(cmd);
}
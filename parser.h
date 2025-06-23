#ifndef PARSER_H
#define PARSER_H

typedef struct
{
    char **argv;
    int argc;
    int background;
} command_t;

command_t *parse_input(char *input);
void free_command(command_t *cmd);

#endif
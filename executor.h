#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
void execute_command(command_t *cmd, int in_fd, int out_fd);

#endif
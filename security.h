#ifndef SECURITY_H
#define SECURITY_H

#include "parser.h"

// Authenticate dangerous commands and control access
typedef enum { CMD_OK = 1, CMD_DENIED = 0 } cmd_status_t;

// Returns CMD_OK if allowed, CMD_DENIED otherwise
cmd_status_t validate_command(command_t *cmd);

#endif 
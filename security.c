#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "security.h"

cmd_status_t validate_command(command_t *cmd) {
    if (!cmd || cmd->argc == 0) return CMD_OK;
    if (strstr(cmd->argv[0], ":(){")) {
        fprintf(stderr, "Error: intento de fork bomb bloqueado.\n");
        return CMD_DENIED;
    }
    if (strcmp(cmd->argv[0], "rm") == 0) {
        for (int i = 1; i < cmd->argc - 1; i++) {
            if (strcmp(cmd->argv[i], "-rf") == 0 && strcmp(cmd->argv[i+1], "/") == 0) {
                fprintf(stderr, "Error: intento de borrado masivo bloqueado.\n");
                return CMD_DENIED;
            }
        }
    }
    return CMD_OK;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "executor.h"
#include "threading.h"
#include "sync.h"
#include "shared_memory.h"
#include "vfs.h"
#include "utils.h"
#include "auth.h"

int main()
{
    init_sync();
    init_shared_memory();
    init_vfs();
    start_monitor_thread();
    char *input;
    if (!authenticate()) return 1;
    while (1)
    {
        input = read_input();
        if (!input)
            break;
        if (strcmp(input, "exit") == 0)
        {
            free(input);
            break;
        }        
        command_t *cmd = parse_input(input);
        if (cmd->argc > 0)
        {
            int in_fd  = -1;
            int out_fd = -1;
            execute_command(cmd, in_fd, out_fd);
        }
        free_command(cmd);
        free(input);
    }
    stop_monitor_thread();
    destroy_vfs();
    destroy_shared_memory();
    destroy_sync();
    printf("Hasta luego.\n");
    return 0;
}

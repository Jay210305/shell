#include "piping.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

static char ***split_pipeline(char **tokens, int argc, int *num_cmds)
{
    int count = 1;
    for (int i = 0; i < argc; i++)
        if (strcmp(tokens[i], "|") == 0)
            count++;
    *num_cmds = count;
    char ***cmds = malloc(count * sizeof(char **));
    int cmd_idx = 0, start = 0;
    for (int i = 0; i <= argc; i++)
    {
        if (i == argc || (i < argc && strcmp(tokens[i], "|") == 0))
        {
            int len = i - start;
            cmds[cmd_idx] = malloc((len + 1) * sizeof(char *));
            for (int j = 0; j < len; j++)
                cmds[cmd_idx][j] = tokens[start + j];
            cmds[cmd_idx][len] = NULL;
            cmd_idx++;
            start = i + 1;
        }
    }
    return cmds;
}

int handle_pipes(command_t *cmd, int in_fd, int out_fd)
{
    int pipe_count = 0;
    for (int i = 0; i < cmd->argc; i++)
        if (strcmp(cmd->argv[i], "|") == 0)
            pipe_count++;
    if (pipe_count == 0)
        return 0;

    int ncmds;
    char ***cmds = split_pipeline(cmd->argv, cmd->argc, &ncmds);

    int (*pipes)[2] = malloc((ncmds - 1) * sizeof(int[2]));
    for (int i = 0; i < ncmds - 1; i++)
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return 1;
        }

    for (int i = 0; i < ncmds; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            if (i == 0)
            {
                if (in_fd >= 0)
                {
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                }
            }
            else
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i == ncmds - 1)
            {
                if (out_fd >= 0)
                {
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                }
            }
            else
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < ncmds - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(cmds[i][0], cmds[i]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    if (in_fd >= 0)
        close(in_fd);
    if (out_fd >= 0)
        close(out_fd);
    for (int i = 0; i < ncmds - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < ncmds; i++)
        wait(NULL);

    free(pipes);
    free(cmds);
    return 1;
}

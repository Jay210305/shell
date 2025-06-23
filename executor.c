#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "executor.h"
#include "piping.h"
#include "vfs.h"
#include "web.h"
#include "tictactoe.h"
#include "security.h"
#include "auth.h"

static int check_executable(const char *cmd)
{
    struct stat sb;
    if (strchr(cmd, '/'))
    {
        if (stat(cmd, &sb) == 0)
        {
            if (sb.st_mode & S_IXUSR)
                return 1;
            fprintf(stderr, "Error: archivo '%s' no tiene permiso de ejecución.\n", cmd);
            return 0;
        }
    }
    return 1;
}

static int handle_redirection_simple(char **argv, int *in_fd, int *out_fd)
{
    for (int i = 0; argv[i]; i++)
    {
        if (strcmp(argv[i], ">") == 0)
        {
            if (!argv[i + 1])
            {
                fprintf(stderr, "syntax error: expected file after '>'\n");
                return -1;
            }
            *out_fd = open(argv[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (*out_fd < 0)
            {
                perror("open output");
                return -1;
            }
            argv[i] = NULL;
            return 1;
        }
        if (strcmp(argv[i], "<") == 0)
        {
            if (!argv[i + 1])
            {
                fprintf(stderr, "syntax error: expected file after '<'\n");
                return -1;
            }
            *in_fd = open(argv[i + 1], O_RDONLY);
            if (*in_fd < 0)
            {
                perror("open input");
                return -1;
            }
            argv[i] = NULL;
            return 1;
        }
    }
    return 0;
}

void execute_command(command_t *cmd)
{
    // Security: validate before executing
    if (validate_command(cmd) == CMD_DENIED)
    {
        free_command(cmd);
        return;
    }

    if (strcmp(cmd->argv[0], "cd") == 0)
    {
        if (cmd->argc < 2)
            fprintf(stderr, "cd: missing argument\n");
        else if (chdir(cmd->argv[1]) != 0)
            perror("cd failed");
        return;
    }
    if (strcmp(cmd->argv[0], "openweb") == 0)
    {
        if (cmd->argc < 2)
            fprintf(stderr, "Uso: openweb <URL>\n");
        else
            open_web(cmd->argv[1]);
        return;
    }
    if (strcmp(cmd->argv[0], "tictactoe") == 0)
    {
        play_game();
        return;
    }
    if (strcmp(cmd->argv[0], "touchv") == 0)
    {
        if (cmd->argc < 2)
            fprintf(stderr, "touchv: missing filename\n");
        else
            vfs_create(cmd->argv[1]);
        return;
    }
    if (strcmp(cmd->argv[0], "lsv") == 0)
    {
        vfs_list();
        return;
    }

    if (handle_pipes(cmd))
        return;

    int in_fd = -1, out_fd = -1;
    if (handle_redirection_simple(cmd->argv, &in_fd, &out_fd) < 0)
        return;

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    if (pid == 0)
    {
        if (in_fd != -1)
        {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd != -1)
        {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        if (!check_executable(cmd->argv[0]))
            exit(EXIT_FAILURE);
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        if (!cmd->background)
            waitpid(pid, NULL, 0);
        else
            printf("[Started background pid %d]\n", pid);
    }
    if (strcmp(cmd->argv[0], "listusers") == 0) {
        list_users();
        return;
    }
}
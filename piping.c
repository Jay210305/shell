#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "piping.h"

typedef struct
{
    char **argv;
    int argc;
} simple_cmd;
static simple_cmd split_cmd(char **tokens, int start, int end)
{
    simple_cmd sc;
    sc.argc = end - start;
    sc.argv = malloc((sc.argc + 1) * sizeof(char *));
    for (int i = 0; i < sc.argc; i++)
        sc.argv[i] = strdup(tokens[start + i]);
    sc.argv[sc.argc] = NULL;
    return sc;
}

int handle_pipes(command_t *cmd)
{
    int pipe_pos = -1;
    for (int i = 0; i < cmd->argc; i++)
        if (strcmp(cmd->argv[i], "|") == 0)
        {
            pipe_pos = i;
            break;
        }
    if (pipe_pos < 0)
        return 0;

    // detect redirection after pipeline
    int out_fd = -1, in_fd = -1;
    char *redir_out = NULL, *redir_in = NULL;
    for (int i = pipe_pos + 1; i < cmd->argc; i++)
    {
        if (strcmp(cmd->argv[i], ">") == 0 && cmd->argv[i + 1])
        {
            redir_out = cmd->argv[i + 1];
            cmd->argv[i] = NULL;
        }
        if (strcmp(cmd->argv[i], "<") == 0 && cmd->argv[i + 1])
        {
            redir_in = cmd->argv[i + 1];
            cmd->argv[i] = NULL;
        }
    }
    simple_cmd c1 = split_cmd(cmd->argv, 0, pipe_pos);
    simple_cmd c2 = split_cmd(cmd->argv, pipe_pos + 1, cmd->argc);
    int fd[2];
    if (pipe(fd) < 0)
    {
        perror("pipe");
        return 1;
    }

    pid_t p1 = fork();
    if (p1 == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp(c1.argv[0], c1.argv);
        perror("execvp c1");
        exit(EXIT_FAILURE);
    }

    pid_t p2 = fork();
    if (p2 == 0)
    {
        if (redir_in)
        {
            in_fd = open(redir_in, O_RDONLY);
            if (in_fd < 0)
                perror("open");
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        if (redir_out)
        {
            out_fd = open(redir_out, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (out_fd < 0)
                perror("open");
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        execvp(c2.argv[0], c2.argv);
        perror("execvp c2");
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    close(fd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    for (int i = 0; i < c1.argc; i++)
        free(c1.argv[i]);
    free(c1.argv);
    for (int i = 0; i < c2.argc; i++)
        free(c2.argv[i]);
    free(c2.argv);
    return 1;
}
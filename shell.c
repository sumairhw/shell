// clang-format off
#include "parse.h"
#include "builtin.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/history.h>
#include <readline/readline.h>
// clang-format on

int execute(char *args[], parseInfo *);
int execute_cmd(char *args[], parseInfo *);
void setRedirection(parseInfo *);

char *buildPrompt() {
    return "% ";
}

int main(int argc, char *argv[]) {
    char *cmdLine;
    parseInfo *info;          /*stores all information returned by parser*/
    struct commandType *cmmd; /*stores command name and arg list for one command*/

    int status;
    while (1) {
        cmdLine = readline(buildPrompt());

        if (cmdLine == NULL) {
            fprintf(stderr, "Unable to read command");
        }

        info = parse(cmdLine);
        cmmd = &info->cmd;

        if (cmmd->argLen != 0) {
            status = execute(cmmd->argList, info);
        }

        free_info(info);
        free(cmdLine);
    }
    return 0;
}

int execute(char **args, parseInfo *info) {
    if (args[0] == NULL) {
        // empty command
        return 1;
    }

    int i;
    for (i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_cmd[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return execute_cmd(args, info);
}

int execute_cmd(char **args, parseInfo *info) {
    pid_t cpid, wpid;
    int status = 1;

    cpid = fork();
    switch (cpid) {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        setRedirection(info);
        execvp(args[0], args);
        perror("command not found");
        exit(1);
    default:
        wpid = waitpid(cpid, &status, 0);
        printf("Child exited with status %d\n", status >> 8);
        return 0;
    }
}

void setRedirection(parseInfo *info) {
    if (info->boolInfile == 1) {
        // printf("setting input redirection\n");
        int input_fd = open(info->inFile, O_RDONLY);
        if (input_fd < 0) {
            perror("failed to open file\n");
        }
        int newfd = dup2(input_fd, STDIN_FILENO);
    }

    if (info->boolOutfile == 1) {
        // printf("setting output redirection\n");
        int output_fd = open(info->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            perror("failed to open file\n");
        }
        int newfd = dup2(output_fd, STDOUT_FILENO);
    }
}

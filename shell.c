// clang-format off
#include "parse.c"

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

int execute(parseInfo *);
int execute_cmd(parseInfo *);
int execute_piped_cmd(parseInfo *);
int execute_bg_cmd(parseInfo *);
int execute_set_variable(parseInfo *);
void setRedirection(parseInfo *);
int splitCommands(char **, char *);

char *buildPrompt() {
    return "% ";
}

int main(int argc, char *argv[]) {
    load_shell_variables();
    char *cmdLine;
    parseInfo *info; /*stores all information returned by parser*/

    int status;
    while (1) {
        cmdLine = readline(buildPrompt());

        if (cmdLine == NULL) {
            fprintf(stderr, "Unable to read command");
        }

        add_history(cmdLine);

        // handle multiple cmds separated by ;
        char **commands;
        commands = malloc(1024 * sizeof(char *));
        int cmd_count = splitCommands(commands, cmdLine);

        for (int cmd_i = 0; cmd_i < cmd_count; cmd_i++) {
            info = parse(commands[cmd_i]);

            if (info->boolFail) {
                printf("variable not found\n");
                continue;
            }

            if (info->commArray[0].argLen != 0) {
                status = execute(info);
            }

            free_info(info);
        }

        free(commands);
        free(cmdLine);
    }
    return 0;
}

int execute(parseInfo *info) {
    char **args = info->commArray[0].argList;
    if (args[0] == NULL) {
        // empty command
        return 1;
    }

    for (int i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_cmd[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    if (info->boolIsPiped) return execute_piped_cmd(info);
    if (info->boolisVariableSetter) return execute_set_variable(info);
    // else if (info->boolBackground) {
    //     return execute_bg_cmd(info);
    // }
    return execute_cmd(info);
}

int execute_set_variable(parseInfo *info) {
    char **args = info->commArray[0].argList;
    write_shell_variable(args[1], args[2]);
    return 0;
}

int execute_cmd(parseInfo *info) {
    char **args = info->commArray[0].argList;
    pid_t cpid, wpid;
    int status = 1;

    cpid = fork();
    if (cpid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (cpid == 0) {
        if (info->boolInfile || info->boolOutfile) {
            setRedirection(info);
        }

        execvp(args[0], args);
        perror("command not found");
        exit(1);
    }

    wpid = waitpid(cpid, &status, 0);
    printf("Child exited with status %d\n", status >> 8);
    return 0;
}

int execute_piped_cmd(parseInfo *info) {
    char **cmd1 = info->commArray[0].argList;
    char **cmd2 = info->commArray[1].argList;

    pid_t cpid, wpid, cpid2;
    int status = 1;
    int fd[2];

    if (pipe(fd) == -1) {
        printf("pipe failed\n");
        exit(0);
    }

    // first child
    cpid = fork();
    if (cpid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (cpid == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(cmd1[0], cmd1);
        perror("command not found");
        exit(1);
    }

    // second child
    cpid2 = fork();
    if (cpid2 < 0) {
        perror("fork failed");
        exit(1);
    }

    if (cpid2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        execvp(cmd2[0], cmd2);
        perror("command not found");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    wpid = waitpid(cpid, &status, 0);
    wpid = waitpid(cpid2, &status, 0);
    printf("Child exited with status %d\n", status >> 8);
    return 0;
}

int execute_bg_cmd(parseInfo *info) {
    // to do
    // a thread is a very expensive way to prevent a zombie. The standard
    // recommendation is to catch SIGCHLD and then call wait in your signal
    // handler.

    return 0;
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

int splitCommands(char **commands, char *cmdLine) {
    // commands = malloc(1024 * sizeof(char *));
    int count = 0;
    char *cmd;

    cmd = strtok(cmdLine, ";");
    while (cmd != NULL) {
        commands[count] = cmd;
        count = count + 1;
        cmd = strtok(NULL, ";");
    }
    commands[count] = NULL;
    return count;
}

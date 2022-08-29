#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

char *builtin_cmd[] = {"cd", "help", "exit"};

/* It’s an array of function pointers (that take array of strings and return an
 * int*/
int (*builtin_func[])(char **) = {&shell_cd, &shell_help, &shell_exit};

int shell_num_builtins() {
    return sizeof(builtin_cmd) / sizeof(char *);
}

int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: Expected an argument to cd\n");
    } else {
        int rv = chdir(args[1]);
        if (rv != 0) {
            perror("shell : ");
        }
    }
    return 1;
}

int shell_help(char **args) {
    int i;
    printf("Welcome to my shell\n");
    printf("The following are built in:\n");

    for (i = 0; i < shell_num_builtins(); i++) {
        printf("  %s\n", builtin_cmd[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shell_exit(char **args) {
    exit(0);
}

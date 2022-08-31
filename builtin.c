#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define MAPSIZE 30

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
void load_shell_variables();
char *search_shell_variable(char *);
void write_shell_variable(char *, char *);
void save_shell_variables();

/************** related to shell variables ***********/
struct Variable {
    char name[20], value[20];
};
struct Variable shell_variables[30];
int last_var_position = -1;

/************** related to builtin commands ***********/
char *builtin_cmd[] = {"cd", "help", "exit"};

/* It’s an array of function pointers (that take array of strings and return
 * an int*/
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
    save_shell_variables();
    exit(0);
}

/******* shell variables functions****************/
void load_shell_variables() {
    FILE *fp;
    fp = fopen(".env", "r");
    if (fp == NULL) {
        // perror(".env: cannot open file");
        return;
    }

    struct Variable input;
    while (fscanf(fp, "%s %s\n", input.name, input.value) != EOF) {
        last_var_position++;
        strcpy(shell_variables[last_var_position].name, input.name);
        strcpy(shell_variables[last_var_position].value, input.value);
    }

    fclose(fp);
}

void write_shell_variable(char *name, char *value) {
    FILE *fp;
    fp = fopen(".env", "a");
    if (fp == NULL) {
        perror(".env: cannot open file for writing");
        return;
    }

    bool found = false;
    for (int i = 0; i <= last_var_position; i++) {
        if (strcmp(name, shell_variables[i].name) == 0) {
            found = true;
            strcpy(shell_variables[i].value, value);
            printf("shell: variable changed successfully\n");
            break;
        }
    }

    if (!found) {
        last_var_position++;
        strcpy(shell_variables[last_var_position].name, name);
        strcpy(shell_variables[last_var_position].value, value);
        printf("shell: variable written successfully\n");
    }

    fclose(fp);
}

char *search_shell_variable(char *varname) {
    for (int i = 0; i <= last_var_position; i++) {
        if (strcmp(varname, shell_variables[i].name) == 0) {
            // printf("shell: variable found successfully\n");
            return shell_variables[i].value;
        }
    }

    return "notfound";
}

void save_shell_variables() {
    FILE *fp;
    fp = fopen(".env", "w");
    if (fp == NULL) {
        perror(".env: cannot open file for writing");
        return;
    }

    for (int i = 0; i <= last_var_position; i++) {
        fprintf(fp, "%s %s\n", shell_variables[i].name, shell_variables[i].value);
    }
}

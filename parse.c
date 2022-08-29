#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

parseInfo *parse(char *cmdLine) {
    parseInfo *Result;
    Result = malloc(sizeof(parseInfo));
    init_info(Result);

    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    token = strtok(cmdLine, TOK_DELIM);
    while (token != NULL) {
        int rv = parse_help_redirection(token, Result);
        if (rv != -1 && rv != 0) {
            tokens[position] = token;
            position++;
        }

        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;

    for (int i = 0; i < position; i++) {
        strcpy(Result->cmd.argList[i], tokens[i]);
        Result->cmd.argLen++;
    }

    Result->cmd.argList[position] = NULL;
    Result->cmd.command = tokens[0];

    // print_info(Result);
    free(tokens);
    return Result;
}

// int parse_help_pipe(char *token, parseInfo *Result) {
//     // give space between commands
//     if (token[0] == '|') {
//         // pipe exists
//     }

//     return 1;
// }

int parse_help_redirection(char *token, parseInfo *Result) {
    if (token == NULL) {
        return -1;
    }

    char symbol = token[0];

    // no space between filename and redirection symbol
    if (symbol == '<') {
        Result->boolInfile = 1;
        Result->inFile = token + 1;
        return 0;
    } else if (symbol == '>') {
        Result->boolOutfile = 1;
        Result->outFile = token + 1;
        return 0;
    }
    return 1;
}

void init_info(parseInfo *info) {
    // printf("init_info: initializing parseInfo struct\n");
    info->boolBackground = 0;
    info->boolInfile = 0;
    info->boolOutfile = 0;
    info->inFile = NULL;
    info->outFile = NULL;

    info->cmd.command = NULL;
    info->cmd.argLen = 0;

    info->cmd.argList = (char **)malloc(sizeof(char *) * MAXARGS);
    for (int i = 0; i < MAXARGS; i++) {
        info->cmd.argList[i] = (char *)malloc(sizeof(char *) * TOK_BUFSIZE);
    }
}

void print_info(parseInfo *info) {
    printf("print_info: printing info about parseInfo struct\n");
    printf("infile: %d\t%s\n", info->boolInfile, info->inFile);
    printf("outfile: %d\t%s\n", info->boolOutfile, info->outFile);
    printf("background: %d\n", info->boolBackground);
    printf("command: %s\targlen: %d\nargs: ", info->cmd.command,
           info->cmd.argLen);

    for (int i = 0; i < info->cmd.argLen; i++) {
        printf("%s ", info->cmd.argList[i]);
    }

    printf("\n");
}

void free_info(parseInfo *info) {
    // printf("free_info: freeing memory associated to parseInfo struct\n");
    for (int i = 0; i < MAXARGS; i++) {
        free(info->cmd.argList[i]);
    }
    free(info->cmd.argList);

    free(info);
}

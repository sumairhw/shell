#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

parseInfo *parse(char *cmdLine) {
    parseInfo *Result;
    Result = malloc(sizeof(parseInfo));
    init_info(Result);
    // print_info(Result);

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

    int idx = 0, list_i = 0;
    for (int i = 0; i < position; i++) {
        if (tokens[i][0] == '|') {
            Result->boolIsPiped = 1;
            Result->commArray[idx].argList[list_i++] = NULL;
            Result->commArray[idx].command = tokens[0];

            list_i = 0;
            idx++;

            if (idx >= PIPE_MAX_NUM) {
                perror("oops: Only one pipe is supported!\n");
                break;
            }
            continue;
        }

        strcpy(Result->commArray[idx].argList[list_i++], tokens[i]);
        Result->commArray[idx].argLen++;
    }

    Result->commArray[idx].argList[list_i] = NULL;
    Result->commArray[idx].command = Result->commArray[idx].argList[0];

    // print_info(Result);
    free(tokens);
    return Result;
}

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
    info->boolIsPiped = 0;
    info->inFile = NULL;
    info->outFile = NULL;

    for (int idx = 0; idx < PIPE_MAX_NUM; idx++) {
        info->commArray[idx].command = NULL;
        info->commArray[idx].argLen = 0;

        info->commArray[idx].argList = (char **)malloc(sizeof(char *) * MAXARGS);
        for (int i = 0; i < MAXARGS; i++) {
            info->commArray[idx].argList[i] =
                (char *)malloc(sizeof(char *) * TOK_BUFSIZE);
        }
    }
}

void print_info(parseInfo *info) {
    printf("print_info: printing info about parseInfo struct\n");
    printf("infile: %d\t%s\n", info->boolInfile, info->inFile);
    printf("outfile: %d\t%s\n", info->boolOutfile, info->outFile);
    printf("background: %d\n", info->boolBackground);
    printf("piped: %d\n", info->boolIsPiped);

    for (int idx = 0; idx < PIPE_MAX_NUM; idx++) {
        printf("command: %s\targlen: %d\nargs: ", info->commArray[idx].command,
               info->commArray[idx].argLen);

        for (int i = 0; i < info->commArray[idx].argLen; i++) {
            printf("%s ", info->commArray[idx].argList[i]);
        }

        printf("\n");
    }
}

void free_info(parseInfo *info) {
    // printf("free_info: freeing memory associated to parseInfo struct\n");
    for (int idx = 0; idx < PIPE_MAX_NUM; idx++) {
        for (int i = 0; i < MAXARGS; i++) {
            free(info->commArray[idx].argList[i]);
        }
        free(info->commArray[idx].argList);
    }

    free(info);
}

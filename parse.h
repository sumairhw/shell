#define MAXARGS 10
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a="
#define PIPE_MAX_NUM 2

struct commandType {
    char *command;
    char **argList;
    int argLen;
};

typedef struct {
    int boolInfile;     /*infile specified*/
    int boolOutfile;    /*infile specified*/
    int boolBackground; /*run command in bg */
    int boolIsPiped;    /*pipe exists*/
    struct commandType commArray[PIPE_MAX_NUM];
    // struct commandType cmd;
    char *inFile;  /*file for input redirection*/
    char *outFile; /*file for output redirection*/
} parseInfo;

parseInfo *parse(char *);
void print_info(parseInfo *);
void free_info(parseInfo *);
void init_info(parseInfo *);
int parse_help_redirection(char *, parseInfo *);
int parse_help_pipe(char *, parseInfo *);

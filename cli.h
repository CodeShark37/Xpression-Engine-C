#ifndef CLI_H
#define CLI_H

#include "context.h"

typedef struct {
    int json;
    int xml;
    int group;
    int eval;
    const char *filename;
    char *expr;
} CLIOptions;

//The enum ORDER is very important!
//modify carefully
typedef enum {
    S_START,        // Initial state
    S_FORMAT,       // After -json or -xml
    S_EVAL,         // After -eval
    S_GROUP,        // After -g/-group (requires format or eval)
    S_FILE,         // After -f (terminal for options)
    S_EXPR,         // Expression parsing (terminal)
    S_ERROR         // Error state
} State;

// Token type identification
typedef enum {
    TOK_FORMAT,
	TOK_EVAL,
	TOK_GROUP,
	TOK_FILE,
	TOK_EXPR,
	TOK_UNKNOWN
} TokenType;

typedef struct {
    State state;
    int arg_pos;
    char error[256];
} CLIParser;


CLIOptions parse_cli(int argc, char **argv);
void process_expressions(CLIOptions *opts, CtxNode *ctx);
int process_expressions_count(CLIOptions *opts, CtxNode *ctx);
#endif

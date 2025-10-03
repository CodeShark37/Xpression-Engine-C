#ifndef CLI_H
#define CLI_H

#include "context.h"

#define MAX_STATE 6
#define MAX_TOK 5

// Token type identification
typedef enum {
    TOK_FORMAT,
	TOK_EVAL,
	TOK_GROUP,
	TOK_FILE,
	TOK_EXPR,
	TOK_UNKNOWN
} TokenType;

typedef enum {
    ERR_PARSE,
	ERR_MALFORMED_PLACEHOLDERS,
	ERR_NO_EXPRESSION,
    ERR_INVALID_OPTION,
	ERR_MISSING_FILENAME,
	ERR_FILE_READ,
    ERR_UNEXPECTED_ARGS,
	ERR_INVALID_FORMAT,
	ERR_UNKNOWN_TOKEN,
    ERR_GROUP_REQUIRES_FLAG,
	ERR_INCOMPLETE_COMMAND,
	ERR_PROCESS_EXPRESSION,
    ERR_MEMORY_ALLOC
} ErrorType;

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
} CLIState;

typedef struct {
    int json;
    int xml;
    int group;
    int eval;
    const char *filename;
    char *expr;
} CLIOptions;

typedef struct {
    CLIState state;
	CLIOptions opts;
    int arg_pos;
    int has_error;
    char error[256];
} CLIContext;

//int parse_cli(CLIContext *ctx, int argc, char **argv);
int process_cli(int argc, char **argv,CtxNode *eval_ctx);
//void process_expressions(CLIOptions *opts, CtxNode *ctx);
//int process_expressions_count(CLIOptions *opts, CtxNode *ctx);
#endif

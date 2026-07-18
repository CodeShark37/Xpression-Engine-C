/**
 * @file cli.c
 * @brief Deterministic CLI parser based on a finite state machine.
 * @ingroup cli
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 *
 * Implements a strict FSM-driven command-line parser that enforces
 * option ordering and semantic correctness.
 *
 * All helper functions in this file are internal and intentionally
 * not exposed via cli.h.
 */

#include "xpression.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief CLI state transition table.
 *
 * Defines all valid state transitions for each token type.
 * Any transition not explicitly allowed results in S_ERROR.
 *
 * Rows represent current state.
 * Columns represent token type.
 */
static const CLIState transitions[MAX_STATE][CMAX_TOK] = {
    //                CTOK_FORMAT  CTOK_EVAL  CTOK_GROUP  CTOK_INPUT  CTOK_VERSION  CTOK_UNKNOWN
    [S_START]     = { S_FORMAT,   S_EVAL,   S_ERROR,   S_DONE,    S_DONE,      S_ERROR },
    [S_FORMAT]    = { S_ERROR,    S_EVAL,   S_GROUP,   S_DONE,    S_ERROR,     S_ERROR },
    [S_EVAL]      = { S_ERROR,    S_ERROR,  S_GROUP,   S_DONE,    S_ERROR,     S_ERROR },
    [S_GROUP]     = { S_ERROR,    S_ERROR,  S_ERROR,   S_DONE,    S_ERROR,     S_ERROR },
    [S_DONE]      = { S_ERROR,    S_ERROR,  S_ERROR,   S_ERROR,   S_ERROR,     S_ERROR },
    [S_ERROR]     = { S_ERROR,    S_ERROR,  S_ERROR,   S_ERROR,   S_ERROR,     S_ERROR }
};

static void print_usage(const char* prog) {
    printf("Usage:\n %s [\n\t \"<expressions>\" |\n\t [ [-json | -xml] [-eval] [-group | -g] [[-f \"<file>\"] | \"<expressions>\"] ] \n\t]\n\t[--version]", prog);
    printf("\nOptions must appear in this exact order:\n");
    printf("  1. Format flag: -json or -xml (optional)\n");
    printf("  2. Eval flag: -eval (optional)\n");
    printf("  3. Group flag: -group or -g (only with format flag or eval)\n");
    printf("  4. File flag: -f file (only if no expression)\n");
    printf("  5. Expression: text to process (only if no file flag)\n");
    printf("\nExamples:\n");
	printf("  %s \"${\'my expression\'}\"\n", prog);
    printf("  %s -json \"${\'my expression\'}\"\n", prog);
    printf("  %s -xml -eval -group \"${SUM(1,2)}\"\n", prog);
    printf("  %s -json -group -f \"input.txt\"\n", prog);
    printf("  %s --version\n", prog);

}

static void print_version(void) {
    puts("\n  Xpression Engine 1.0.r");
    puts("  ======================");
	puts("  A modular expression evaluation engine "
         "\n  encapsulated in the ${EXPRESSION} format"
         "\n  with built-in functions, context variables,"
         "\n  and AST export in JSON/XML.");
    puts("\n  author: Joshua @Codeshark37");
    puts("  contact: labprogramming7@gmail.com\n");
    puts("  Made in Angola (2025)");
}

/**
 * @brief Classify a raw argv token.
 *
 * Maps a string argument to its logical CLI token type.
 *
 * @param arg Raw argv string
 * @return Corresponding CLITokenType
 */
static CLITokenType classify_token(const char* arg) {
	if (strcmp(arg, "-json") == 0 || strcmp(arg, "-xml") == 0) return CTOK_FORMAT;
    if (strcmp(arg, "-group") == 0 || strcmp(arg, "-g") == 0)  return CTOK_GROUP;
    if (strcmp(arg, "-eval") == 0 || strcmp(arg, "-e") == 0)   return CTOK_EVAL;
    if (strcmp(arg, "--version") == 0)                         return CTOK_VERSION;
    if (strcmp(arg, "-f") == 0)                                return CTOK_INPUT;

    if (strncmp(arg, "-", 1) == 0) return CTOK_UNKNOWN;

    return CTOK_INPUT;
}

static void process_format_flag(CLIContext* ctx) {
    if (strcmp(ctx->argv[ctx->arg_pos], "-json") == 0) {
        ctx->opts.format = FORMAT_JSON;
    }else if (strcmp(ctx->argv[ctx->arg_pos], "-xml") == 0) {
        ctx->opts.format = FORMAT_XML;
    }else{
		ctx->opts.format = FORMAT_PLAIN;
	}
}

static void process_input_flag(CLIContext* ctx){

	if (strcmp(ctx->argv[ctx->arg_pos], "-f") == 0) {
        if ((ctx->arg_pos + 1) < ctx->argc) {
            ctx->opts.filename = ctx->argv[ctx->arg_pos + 1];
            ctx->arg_pos++;
        } else {
            error_add(ctx->errors, CERR_MISSING_FILENAME);
            ctx->state = S_ERROR;
        }
    } else {
        ctx->opts.expr = ctx->argv[ctx->arg_pos];
    }
    if (ctx->state == S_START) {
        ctx->opts.eval = true;
    }
}

/**
 * @brief Perform a state transition and process token side effects.
 *
 * Applies semantic effects of the token (flags, file loading, etc.)
 * and advances the CLI state machine.
 *
 * @param ctx CLI context
 * @param tok Classified token
 */
 static void transition(CLIContext* ctx, CLITokenType tok) {
    CLIState next_state = transitions[ctx->state][tok];

    switch (tok) {
        case CTOK_FORMAT:  process_format_flag(ctx); break;
        case CTOK_EVAL:    ctx->opts.eval = true; break;
        case CTOK_GROUP:   ctx->opts.group = true; break;
        case CTOK_INPUT:   process_input_flag(ctx); break;
        case CTOK_VERSION: ctx->opts.version = true; break;
        default: break;
    }

    ctx->state = next_state;
    ctx->arg_pos++;
}

// Validate final state
static bool validate_cli(CLIContext* ctx) {
    if (ctx->state != S_DONE) {
        error_add(ctx->errors, CERR_NO_EXPRESSION);
        return false;
    }

    /* Se o loop acabou e restaram argumentos na cauda (argc > arg_pos), 
       significa que o usuário digitou lixo após o token de input terminal */
    if (ctx->arg_pos < ctx->argc) {
        error_add(ctx->errors, CERR_UNEXPECTED_ARGS, ctx->argv[ctx->arg_pos]);
        return false;
    }

    return true;
}

/**
 * @brief Parse and validate CLI arguments.
 *
 * Executes the FSM until completion or error.
 *
 * @param ctx CLI context
 * @return true if parsing succeeded
 */
static bool parse_cli(CLIContext* ctx) {
    if (!ctx || ctx->argc < 2) return false;

    /* O while ficou absurdamente limpo e focado */
    while (ctx->arg_pos < ctx->argc && ctx->state != S_ERROR) {
        const char* arg = ctx->argv[ctx->arg_pos];
        CLITokenType token = classify_token(arg);

        if (transitions[ctx->state][token] == S_ERROR) {
            error_add(ctx->errors, CERR_INVALID_OPTION, arg);
            ctx->state = S_ERROR;
            return false;
        }

        transition(ctx, token);
    }
    /* parse_cli assume a responsabilidade de chamar o validador interno */
    return validate_cli(ctx);
}


// Main CLI orchestration function
bool process_cli(int argc, char** argv, CLIOptions* xopts) {

    CLIContext ctx = {
        .state = S_START,
        .arg_pos = 1,
        .opts = {0},
		.argc = argc,
		.argv = (const char**)argv
    };

    if (argc < 2) {
        print_usage(argv[0]);
		return true;
    }
	
    if(!parse_cli(&ctx))return false;
    *xopts = ctx.opts;
    return true;
}

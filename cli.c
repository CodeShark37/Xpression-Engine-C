#include "cli.h"
#include "parser.h"
#include "printer.h"
#include "evaluator.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// State transition table - defines valid transitions
static CLIState transitions[MAX_STATE][MAX_TOK] = {
              // FORMAT      EVAL     GROUP   FILE     EXPR    
    [S_START] = {S_FORMAT, S_EVAL,   S_ERROR, S_ERROR, S_ERROR},
    [S_FORMAT]= {S_ERROR,  S_EVAL,   S_GROUP, S_FILE,  S_EXPR},
    [S_EVAL]  = {S_ERROR,  S_ERROR,  S_GROUP, S_FILE,  S_EXPR},
    [S_GROUP] = {S_ERROR,  S_ERROR,  S_ERROR, S_FILE,  S_EXPR},
    [S_FILE]  = {S_ERROR,  S_ERROR,  S_ERROR, S_ERROR, S_ERROR},
    [S_EXPR]  = {S_ERROR,  S_ERROR,  S_ERROR, S_ERROR, S_ERROR}
};

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-json|-xml] [-eval] [-group|-g] [[-f file]| \"<expressions>\"]\n", prog);
    fprintf(stderr, "\nOptions must appear in this exact order:\n");
    fprintf(stderr, "  1. Format flag: -json or -xml (optional)\n");
    fprintf(stderr, "  3. Eval flag: -eval (optional)\n");
    fprintf(stderr, "  2. Group flag: -group or -g (only with format flag or eval)\n");
    fprintf(stderr, "  4. File flag: -f file (requires at least one previous option)\n");
    fprintf(stderr, "  5. Expression: text to process (only if no -f flag)\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s -json ${\"hello world\"}\n", prog);
    fprintf(stderr, "  %s -xml -eval -group \"hello world\"\n", prog);
    fprintf(stderr, "  %s -json -group -f input.txt\n", prog);
}


static void print_group_boundary(const CLIOptions *opts, int is_start) {
    if (!opts->group) return;

    if (opts->xml) {
        printf(is_start ? "<expressions>\n" : "</expressions>\n");
    }else{
        printf(is_start ? "[\n" : "]\n");
	}
}

static void print_plain_result(Value *result, const CLIContext *ctx,int is_last) {
    if (!result) return;

    const char *output = value_to_string(result);
    if (output) {
        printf("  %s", output);
        if (ctx->opts.group && !is_last) printf(",");
        printf("\n");
    }
}

static void print_error(CLIContext *ctx, const char *extra_context) {

	if (ctx->opts.json) {
		fprintf(stderr,"// %s",ctx->error);
	} else if (ctx->opts.xml) {
		fprintf(stderr,"  <!-- %s: %s -->\n", ctx->error, extra_context ? extra_context : "");
	} else {
		fprintf(stderr, "Error: %s%c", ctx->error,(!extra_context)?'\n':' ');
		if (extra_context) {
			fprintf(stderr, ": %s\n", extra_context);
		}
	}
}

static void set_error(CLIContext *ctx, ErrorType type, const char *context) {
    const char *messages[] = {
        [ERR_PARSE] = "Parse failed",
        [ERR_MALFORMED_PLACEHOLDERS] = "Malformed placeholders",
        [ERR_NO_EXPRESSION] = "No expression to process",
        [ERR_INVALID_OPTION] = "Invalid option or wrong order",
        [ERR_MISSING_FILENAME] = "Missing filename after -f",
        [ERR_FILE_READ] = "Failed to read file",
        [ERR_UNEXPECTED_ARGS] = "Unexpected arguments after filename",
        [ERR_INVALID_FORMAT] = "Invalid format flag",
        [ERR_UNKNOWN_TOKEN] = "Unknown token type",
        [ERR_GROUP_REQUIRES_FLAG] = "-group requires -json, -xml, or -eval",
        [ERR_INCOMPLETE_COMMAND] = "Incomplete command",
        [ERR_PROCESS_EXPRESSION] = "Failed to process expression",
        [ERR_MEMORY_ALLOC] = "Memory allocation failed"
    };

    ctx->state = S_ERROR;
    ctx->has_error = 1;

    snprintf(ctx->error, sizeof(ctx->error), "%s%s%s",
             messages[type],
             context ? ": " : "",
             context ? context : "");
}

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *content = malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }

    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    return content;
}

static TokenType classify_token(const char *arg) {

	if (!strcmp(arg, "-json") || !strcmp(arg, "-xml")) return TOK_FORMAT;
	if (!strcmp(arg, "-g") || !strcmp(arg, "-group")) return TOK_GROUP;
	if (!strcmp(arg, "-eval")) return TOK_EVAL;
	if (!strcmp(arg, "-f")) return TOK_FILE;
	if(strstr(arg,"${") && strchr(arg,'}'))return TOK_EXPR;	
	
	return TOK_UNKNOWN;
}

static int process_format_flag(const char *arg, CLIContext *ctx) {
    if (!strcmp(arg, "-json")) {
        ctx->opts.json = 1;
        return 1;
    }
    if (!strcmp(arg, "-xml")) {
        ctx->opts.xml = 1;
        return 1;
    }
    return 0;
}

static int transition(CLIContext *ctx, TokenType tok, char **argv, int argc) {
    if (ctx->state >= MAX_STATE ||
		tok >= MAX_TOK ||
		transitions[ctx->state][tok] == S_ERROR) {
        set_error(ctx, ERR_INVALID_OPTION, argv[ctx->arg_pos]);
        return 0;
    }

    CLIState next_state = transitions[ctx->state][tok];

    switch (tok) {
        case TOK_FORMAT:
            if (!process_format_flag(argv[ctx->arg_pos], ctx)) {
                set_error(ctx, ERR_INVALID_FORMAT, argv[ctx->arg_pos]);
                return 0;
            }
            ctx->state = next_state;
            ctx->arg_pos++;
            break;

        case TOK_EVAL:
            ctx->opts.eval = 1;
            ctx->state = next_state;
            ctx->arg_pos++;
            break;

        case TOK_GROUP:
			/*if (ctx->arg_pos + 1 >= argc) {
                set_error(ctx, ERR_NO_EXPRESSION, NULL);
                return 0;
            }*/
            ctx->opts.group = 1;
            ctx->state = next_state;
            ctx->arg_pos++;
            break;

        case TOK_FILE:
            if (ctx->arg_pos + 1 >= argc) {
                set_error(ctx, ERR_MISSING_FILENAME, NULL);
                return 0;
            }

            ctx->opts.filename = argv[ctx->arg_pos + 1];
            if (!ctx->opts.filename || !*ctx->opts.filename) {
                set_error(ctx, ERR_FILE_READ, ctx->opts.filename);
                return 0;
            }
            ctx->opts.expr = read_file(ctx->opts.filename);
			
            ctx->state = next_state;
            ctx->arg_pos += 2;
            break;

		case TOK_UNKNOWN:
        case TOK_EXPR:
			puts("EXPR");
            if (!argv[ctx->arg_pos] || !*argv[ctx->arg_pos]) {
                set_error(ctx, ERR_PROCESS_EXPRESSION, NULL);
                return 0;
            }
			ctx->opts.expr = argv[ctx->arg_pos];
			ctx->state = next_state;
            ctx->arg_pos++;
            break;
    }

    return 1;
}

static int process_expressions(CLIContext *ctx, CtxNode *eval_ctx) {

    print_group_boundary(&ctx->opts, 1);
	
	const char *p = ctx->opts.expr;

	while(*p){
		Node *ast = parse_expression(&p);
		if (!ast) {
			print_group_boundary(&ctx->opts, 0);
			return 0;
		}
		//look ahead to get the last node 
		const char* tmp = strdup(p);
		int is_last = !(parse_expression(&tmp));
		
		Value *result = (ctx->opts.eval && eval_ctx) ? eval_node(ast, eval_ctx) : NULL;

		if (ctx->opts.json) {
			print_json(ast, result, ctx->opts.group,is_last);
		} else if (ctx->opts.xml) {
			print_xml(ast, result, ctx->opts.group);
		} else {
			print_plain_result(result, ctx,is_last);
		}
	    
		val_free(result);
		node_free(ast);
	}
	print_group_boundary(&ctx->opts, 0);
    return 1;
}

// Validate final state
static int validate_ctx(CLIContext *ctx) {

    if (!ctx->opts.expr || !*ctx->opts.expr) {
        set_error(ctx, ERR_MISSING_FILENAME, NULL);
        return 0;
    }

    return 1;
}

// Parse command line arguments
int parse_cli(CLIContext *ctx, int argc, char **argv) {
	if (!ctx || argc < 2) return 0;
	
    while (ctx->arg_pos < argc && ctx->state != S_ERROR) {
        TokenType token = classify_token(argv[ctx->arg_pos]);
		if (!transition(ctx, token, argv, argc))break;
    }
    return ctx->state != S_ERROR;
}

// Main CLI orchestration function
int process_cli(int argc, char **argv, CtxNode *eval_ctx) {

    CLIContext ctx = {
        .state = S_START,
        .arg_pos = 1,
        .opts = {0},
        .error = {0},
		.has_error = 0
    };

    if (argc < 2) {
        print_usage(argv[0]);
		return 0;
    }
	// Parse arguments
	if (!parse_cli(&ctx, argc, argv)) {
		print_error(&ctx,NULL);
		return 0;
	}

    // Validate configuration
    if (!validate_ctx(&ctx)) {
		print_error(&ctx,NULL);
        free(ctx.opts.expr);
		ctx.opts.expr = NULL;
        return 0;
    }
	
    // Process expressions
    if(!process_expressions(&ctx, eval_ctx)){
		print_error(&ctx,NULL);
		return 0;
	}
	
	if (ctx.opts.expr && ctx.opts.filename) {
        free(ctx.opts.expr);
    }

    return 1;
}


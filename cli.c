#include "cli.h"
#include "parser.h"
#include "printer.h"
#include "evaluator.h"
#include "value.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// State transition table - defines valid transitions
static State transitions[6][5] = {
    //           FORMAT      EVAL     GROUP   FILE     EXPR
    [S_START] = {S_FORMAT, S_EVAL,  S_ERROR,  S_ERROR, S_ERROR},
    [S_FORMAT]= {S_ERROR,  S_EVAL,  S_ERROR,  S_FILE,  S_EXPR},
    [S_EVAL]  = {S_ERROR,  S_ERROR,  S_GROUP, S_FILE,  S_EXPR},
    [S_GROUP] = {S_ERROR,  S_ERROR,  S_ERROR,  S_FILE,  S_EXPR},
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
    fprintf(stderr, "  %s -json \"hello world\"\n", prog);
    fprintf(stderr, "  %s -xml -eval -group \"hello world\"\n", prog);
    fprintf(stderr, "  %s -json -group -f input.txt\n", prog);
}


static void print_group_boundary(const CLIOptions *opts, int is_start) {
    if (!opts->group) return;
    
    if (opts->xml) {
        printf(is_start ? "<expressions>\n" : "</expressions>\n");
    }else{
        printf(is_start ? "[\n" : "\n]\n");
	}
}

static void print_error(const char *expr_str, const CLIOptions *opts) {
    if (opts->group) {
        if (opts->json) {
            printf("null");
        } else if (opts->xml) {
            printf("  <!-- parse failed: %s -->\n", expr_str);
        }
    } else {
        fprintf(stderr, "Parse failed: %s\n", expr_str);
    }
}

static void set_error(CLIParser *p, const char *msg) {
    p->state = S_ERROR;
    strncpy(p->error, msg, sizeof(p->error) - 1);
    p->error[sizeof(p->error) - 1] = '\0';
}

static void cleanup_expressions(char **exprs) {
    if (!exprs) return;
    for (char **p = exprs; *p != NULL; p++) {
        free(*p);
    }
    free(exprs);
}


static TokenType classify_token(const char *arg) {
    if (!strcmp(arg, "-json") || !strcmp(arg, "-xml")) return TOK_FORMAT;
    if (!strcmp(arg, "-g") || !strcmp(arg, "-group")) return TOK_GROUP;
    if (!strcmp(arg, "-eval")) return TOK_EVAL;
    if (!strcmp(arg, "-f")) return TOK_FILE;
    return TOK_EXPR;
}
static char *join_args(char **argv, int start, int end) {
    if (start >= end) return NULL;
    
    size_t total_len = 0;
    for (int i = start; i < end; i++) {
        total_len += strlen(argv[i]) + 1;
    }
    
    char *result = malloc(total_len);
    if (!result) return NULL;
    
    result[0] = '\0';
    for (int i = start; i < end; i++) {
        strcat(result, argv[i]);
        if (i + 1 < end) strcat(result, " ");
    }
    
    return result;
}

static char *read_file_content(const char *filename) {
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

static int process_format_flag(const char *arg, CLIOptions *opt) {
    if (!strcmp(arg, "-json")) {
        opt->json = 1;
        return 1;
    }
    if (!strcmp(arg, "-xml")) {
        opt->xml = 1;
        return 1;
    }
    return 0;
}

static int process_single_expr(const char *expr_str, CLIOptions *opts, CtxNode *ctx) {
    
    Node *ast = parse_expression_str(expr_str);
    if (!ast) {
        print_error(expr_str, opts);
        return 0;
    }
    
    Value *result = (opts->eval && ctx) ? eval_node(ast, ctx) : NULL;
    
    if (opts->json) {
        print_json(ast, result, opts->group);
    } else if(opts->xml) {
        print_xml(ast, result, opts->group);
    }else{
		printf("%s%c \n",value_to_string(result),opts->group?',':' ');
	}
	
    
    val_free(result);
    node_free(ast);
    return 1;
}


void process_expressions(CLIOptions *opts, CtxNode *ctx) {
    if (!opts->expr) {
        fprintf(stderr, "Error: No expression to process\n");
        return;
    }
    
   PlaceholderResult result = parse_placeholders(opts->expr);
   
    if (result.error_code != 0) {
        fprintf(stderr, "Error: Malformed placeholders\n");
        cleanup_expressions(result.nodes);
        return;
    } 
    
    print_group_boundary(opts, 1);
    
    for (char **p = result.nodes; *p != NULL; p++) {
        if (*p) {
            process_single_expr(*p, opts, ctx);
        }
    }
    
    print_group_boundary(opts, 0);
    cleanup_expressions(result.nodes);
}

int process_expressions_count(CLIOptions *opts, CtxNode *ctx) {
    if (!opts->expr) return -1;
    
    PlaceholderResult result = parse_placeholders(opts->expr);
   
    if (result.error_code != 0) {
        fprintf(stderr, "Error: Malformed placeholders\n");
        cleanup_expressions(result.nodes);
        return -1;
    } 
    
    print_group_boundary(opts, 1);
    
    int success_count = 0;
	for (char **p = result.nodes; *p != NULL; p++) {
        if (*p && process_single_expr(*p, opts, ctx)) {
            success_count++;
        }
    }
    
    print_group_boundary(opts, 0);
    cleanup_expressions(result.nodes);
    return success_count;
}

// Simplified state machine for CLI parsing

// Core state machine transition function
static int transition(CLIParser *p, TokenType tok, char **argv, int argc, CLIOptions *opt) {
    // Check if transition is valid
	int max_state=6,max_tok=5;
    if (p->state >= max_state || tok >= max_tok || transitions[p->state][tok] == S_ERROR) {
        set_error(p, "Invalid option or wrong order");
        return 0;
    }
    
    State next_state = transitions[p->state][tok];
    
    switch (tok) {
        case TOK_FORMAT:
            if (!process_format_flag(argv[p->arg_pos], opt)) {
                set_error(p, "Invalid format flag");
                return 0;
            }
            p->state = next_state;
            p->arg_pos++;
            break;
            
        case TOK_GROUP:
            opt->group = 1;
            p->state = next_state;
            p->arg_pos++;
            break;
            
        case TOK_EVAL:
            opt->eval = 1;
            p->state = next_state;
            p->arg_pos++;
            break;
            
        case TOK_FILE:
            if (p->arg_pos + 1 >= argc) {
                set_error(p, "Missing filename after -f");
                return 0;
            }
            
            opt->filename = argv[p->arg_pos + 1];
            opt->expr = read_file_content(opt->filename);
            if (!opt->expr) {
                set_error(p, "Failed to read file");
                return 0;
            }
            
            p->state = next_state;
            p->arg_pos += 2;
            
            // Check for extra args after filename
            if (p->arg_pos < argc) {
                set_error(p, "Unexpected arguments after filename");
                return 0;
            }
            break;
            
        case TOK_EXPR:
            opt->expr = join_args(argv, p->arg_pos, argc);
            if (!opt->expr) {
                set_error(p, "Failed to process expression");
                return 0;
            }
            p->state = next_state;
            p->arg_pos = argc; // Consume all remaining args
            break;
            
        default:
            set_error(p, "Unknown token type");
            return 0;
    }
    
    return 1;
}

// Validation rules after parsing
static int validate_final_state(const CLIOptions *opt, CLIParser *p) {
    // Rule 1: -group requires format flag
    if (opt->group && !opt->json && !opt->xml && !opt->eval) {
        set_error(p, "-group requires -json or -xml or -eval");
        return 0;
    }
    
    // Rule 2: Must have either file or expression
    if (!opt->expr) {
        set_error(p, "Missing expression or file input");
        return 0;
    }
    
    // Rule 3: Final state must be valid terminal
    if (p->state != S_FILE && p->state != S_EXPR) {
        set_error(p, "Incomplete command");
        return 0;
    }
    
    return 1;
}

CLIOptions parse_cli(int argc, char **argv) {
    CLIOptions opt = {0};
    
    if (argc < 2) {
        print_usage(argv[0]);
        return opt;
    }
    
    CLIParser parser = {
        .state = S_START,
        .arg_pos = 1,
        .error = {0}
    };
    
    // Main parsing loop using state machine
    while (parser.arg_pos < argc && parser.state != S_ERROR) {
        TokenType token = classify_token(argv[parser.arg_pos]);
        
        if (token == TOK_UNKNOWN) {
            // Unknown flag - treat as expression start
            token = TOK_EXPR;
        }
        
        if (!transition(&parser, token, argv, argc, &opt)) {
            break; // Error occurred
        }
        
        // Terminal states end parsing
        if (parser.state == S_FILE || parser.state == S_EXPR) {
            break;
        }
    }
    
    // Validate final state and options
    if (parser.state != S_ERROR) {
        validate_final_state(&opt, &parser);
    }
    
    // Handle errors
    if (parser.state == S_ERROR) {
        fprintf(stderr, "Error: %s\n", parser.error);
        print_usage(argv[0]);
        if (opt.expr && opt.filename) free(opt.expr);
        opt = (CLIOptions){0}; // Reset to invalid state
    }
    
    return opt;
}
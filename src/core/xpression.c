/**
 * @file xpression.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-06-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */


#include "xpression.h"

#include <stdio.h>

bool xpression_init(
    XpressionContext* xpr,
    const char* source,
    const ErrorContext* err,
    const CLIOptions* cli
)
{
    if (!xpr)
        return false;

    memset(xpr, 0, sizeof(*xpr));

    xpr->source = source;

    error_ctx_init(&xpr->errors, err_cfg);

    if (cli)
        xpr->cli = *cli;

    return true;
}

char* xpression_load_source(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f){
		error_add(CERR_FILE_READ, filename);
		return NULL;
	}

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char* content = calloc(size + 1,sizeof(char));

    if (!content) {
		error_add(SERR_MEMORY_ALLOC);
        fclose(f);
        return NULL;
    }

    if(fread(content, 1, size, f) != (size_t)size && ferror(f) > 0){
		error_add(CERR_FILE_READ, filename);
		free(content);
		fclose(f);
        return NULL;
	}

    fclose(f);
    return content;
}

/**
 * @brief Execute expression parsing and output rendering.
 *
 * @param ctx CLI context
 * @param eval_ctx Evaluation context
 * @return true on success
 */
static bool process_xpression(CLIContext* ctx, CtxNode* eval_ctx) {

    // Parse all expressions
    Node* ast = parse_xpression_ex(ctx->opts.expr, eval_ctx);

    if (!ast || !ast->n_children) return false;

    // Printer handles everything
    print_xpression(ast, eval_ctx, ctx);

    // Cleanup
    node_free(&ast);
    return true;
}

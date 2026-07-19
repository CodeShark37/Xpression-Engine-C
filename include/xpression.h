/**
 * @file xpression.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-06-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef XPRESSION_H
#define XPRESSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>

#include "cli.h"
#include "ast.h"
#include "context.h"
#include "error.h"

typedef struct {
    const char* source;
    Node* ast;
    CtxNode* eval_ctx;
    ErrorContext errors;
    CLIOptions cli;
} XpressionContext;

bool xpression_init(
    XpressionContext* xpr,
    const ErrorConfig* err_cfg,
    const CLIOptions* cli
);

void  xpression_free(XpressionContext* xpr);
char* xpression_load_source(char* filename);
bool xpression_run(XpressionContext* xpr);

#ifdef __cplusplus
}
#endif

#endif /* XPRESSION_H */

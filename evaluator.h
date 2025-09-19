#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "context.h"

Value *eval_node(Node *n, CtxNode *ctx);

#endif

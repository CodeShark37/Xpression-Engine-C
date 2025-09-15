#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "context.h"
#include "functions.h"
#include "value.h"

Value *eval_node(Node *n, CtxNode *ctx);

#endif

/**
 * @file eval.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-06-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef EVAL_H
#define EVAL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "xpression.h"

typedef struct {
	Node* node;
	CtxNode* root;
	ErrorContext* errors;
} EvalContext;

Value eval_xpression(XpressionContext *xpr);

#ifdef __cplusplus
}
#endif

#endif
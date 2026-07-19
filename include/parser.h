/**
	XPression Engine C (c)2025

	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com

	parser.h
*/

#ifndef PARSER_H
#define PARSER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stddef.h>

#include "xpression.h"

typedef struct {
    const char *src, *pos;
    size_t line, col;
	ErrorContext* errors;
	bool has_error;
} ParserContext;

typedef Node *(*ParserFn)(ParserContext*);

Node *parse_xpression(const char *s);
Node *parse_xpression_ex(const char *s, CtxNode* ctx);

#ifdef __cplusplus
}
#endif

#endif

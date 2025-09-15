#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

Node *parse_expression_str(const char *s);
int extract_placeholders(const char *text, char ***out, size_t *out_count);

#endif

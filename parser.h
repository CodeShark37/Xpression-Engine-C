#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

typedef struct {
    char **nodes;
    int error_code;
} PlaceholderResult;

typedef struct {
    const char *src, *pos;
    int line, col;
    char error_msg[256];
    int has_error;
} Parser;


Node *parse_expression_str(const char *s);
const char *parser_get_last_error(void);
PlaceholderResult parse_placeholders(const char *text);

#endif

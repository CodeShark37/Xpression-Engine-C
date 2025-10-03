#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

typedef struct {
    const char *src, *pos;
    int line, col;
    int has_error;
    char error_msg[256];
} Parser;


Node *parse_expression(const char **s);

#endif

#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    NODE_OBJECT, NODE_PROPERTY, NODE_FUNCTION,
    NODE_LIST, NODE_STRING, NODE_NUMBER, NODE_IDENTIFIER
} NodeKind;

typedef struct Node {
    NodeKind kind;
    char *value;
    struct Node **children;
    size_t n_children;
} Node;

Node *node_new(NodeKind k, const char *value);
void node_add(Node *parent, Node *child);
void node_free(Node *n);

#endif

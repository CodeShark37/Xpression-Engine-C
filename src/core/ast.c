/**
 * @file ast.c
 * @brief Implementation of AST node functions.
 * @ingroup ast
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#include "xpression.h"

#include <stdlib.h>
#include <string.h>



Node *node_new(NodeKind k, const char *value)
{
    Node *n = calloc(1, sizeof(Node));
    if (!n){
        error_add(SERR_MEMORY_ALLOC);
        return NULL;
    }
    n->value = value ? strdup(value) : "";
    if (!n->value){
        error_add(SERR_MEMORY_ALLOC);
        free(n); n = NULL;
        return n;
    }
    n->kind = k;
    n->len = strlen(n->value);
    return n;
}

Node *node_new_pos(NodeKind k, const char *value, const char *start, size_t line, size_t col)
{
    Node *n = node_new(k, value);
    node_set_pos(n, start, line, col);
    return n;
}

Node *node_new_len(NodeKind k, const char *value, const char *start, size_t line, size_t col, size_t len)
{
    Node *n = node_new(k, value);
    node_set_pos(n, start, line, col);
    node_set_len(n,len);
    return n;
}

inline void node_set_pos(Node *n, const char *start, size_t line, size_t col)
{
    if (!n)
        return;
    n->start = start;
    n->line = line;
    n->col = col;
}

inline void node_set_len(Node *n, size_t len)
{
    if (!n)
        return;
    n->len = len;
}

bool node_add(Node *parent, Node *child)
{
    if (!parent || !child)
        return false;
    size_t size = parent->n_children + 1;
    Node **tmp = realloc(parent->children, sizeof(Node *) * size);
    if (!tmp)
    {
        error_add(SERR_MEMORY_ALLOC);
        node_free(&child);
        return false;
    }
    parent->children = tmp;
    tmp = NULL;
    parent->children[parent->n_children++] = child;
    return true;
}

bool node_equal(Node* n1, Node* n2){
    if(!n1 != !n2) return false;
    if(n1->kind != n2->kind) return false;
    if((!n1->value != !n2->value) || strcmp(n1->value, n2->value) != 0) return false;
    if(n1->n_children != n2->n_children) return false;
    if(!n1->children != !n2->children) return false;
    if(n1->n_children > 0){
        for(size_t i=0; i < n1->n_children; i++){
            if(!node_equal(n1->children[i], n2->children[i]))
                return false;
        }
    }
    return true;
}

void node_free(Node **np)
{
    if (!np || !(*np))
        return;
    Node *n = *np;

    n->start = NULL;

    if (n->value){
        free(n->value);
        n->value = NULL;
    }

    if (n->children){
        for (size_t i = 0; i < n->n_children; i++){
            node_free(&(n->children[i]));
        }
        free(n->children);
        n->children = NULL;
    }

    free(n);
    n = NULL;
    *np = NULL;
}

inline bool check_nodekind(Node* n,NodeKind k){
    return n ? (n->kind == k): false;
}

inline const char* nodekind_name(NodeKind k){
    switch(k){
        case NODE_OBJECT : return "Object";
        case NODE_FUNCTION : return "Function";
        case NODE_LIST : return "Array";
        case NODE_STRING : return "String";
        case NODE_NUMBER : return "Number";
        case NODE_BOOL : return "Boolean";
        case NODE_NULL : return "Null";
        default:
            return "Unknown";
    }
}

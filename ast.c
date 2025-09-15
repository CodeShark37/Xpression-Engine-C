#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node *node_new(NodeKind k, const char *value) {
    Node *n = calloc(1, sizeof(Node));
    if (!n) { perror("calloc"); exit(1); }
    n->kind = k;
    n->value = value ? strdup(value) : NULL;
    return n;
}

void node_add(Node *parent, Node *child) {
    parent->children = realloc(parent->children, sizeof(Node*)*(parent->n_children+1));
    if (!parent->children) { perror("realloc"); exit(1); }
    parent->children[parent->n_children++] = child;
}

void node_free(Node *n) {
    if (!n) return;
    for (size_t i=0;i<n->n_children;i++){
		node_free(n->children[i]);
	}
    free(n->children);
	n->children = NULL;
    free(n->value);
	n->value = NULL;
    free(n);
	n = NULL;
}

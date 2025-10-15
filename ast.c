#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"


Node *node_new(NodeKind k, const char *value) {
    Node *n = xpr_calloc(1, sizeof(Node));
    if (!n) { xpr_error_set(XPR_ERR_MEMORY, "failed to allocate Node"); return NULL; }
    n->kind = k;
    n->value = value ? xpr_strdup(value) : NULL;
    return n;
}

void node_add(Node *parent, Node *child) {
    Node **new_children = xpr_realloc(parent->children, sizeof(Node*)*(parent->n_children+1));
    if (!new_children) { xpr_error_set(XPR_ERR_MEMORY, "failed to grow children"); return; }
    parent->children = new_children;
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

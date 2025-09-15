#include "context.h"
#include <stdlib.h>
#include <string.h>

CtxNode *ctx_new(const char *name) {
    CtxNode *n = calloc(1,sizeof(CtxNode));
    n->name = name ? strdup(name) : NULL;
    n->props = NULL;
    n->children = NULL;
    n->n_children = 0;
    return n;
}

void ctx_add_child(CtxNode *parent, CtxNode *child) {
    parent->children = realloc(parent->children, sizeof(CtxNode*)*(parent->n_children+1));
    parent->children[parent->n_children++] = child;
}

void ctx_set_prop(CtxNode *n, const char *key, Value *v) {
    PropKV *kv = malloc(sizeof(PropKV)); kv->key = strdup(key); kv->val = v; kv->next = n->props; n->props = kv;
}

Value *ctx_get_prop(CtxNode *n, const char *key) {
    for (PropKV *k = n->props; k; k = k->next) if (strcmp(k->key, key) == 0) return k->val;
    return NULL;
}

CtxNode *ctx_find_child(CtxNode *n, const char *name) {
    for (size_t i=0;i<n->n_children;i++) if (strcmp(n->children[i]->name,name) == 0) return n->children[i];
    return NULL;
}

void ctx_free(CtxNode *n) {
    if (!n) return;
    free(n->name);
    for (PropKV *k=n->props;k;) { PropKV *nx = k->next; free(k->key); val_free(k->val); free(k); k = nx; }
    for (size_t i=0;i<n->n_children;i++) ctx_free(n->children[i]);
    free(n->children);
    free(n);
}

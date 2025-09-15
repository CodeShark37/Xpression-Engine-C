#ifndef CONTEXT_H
#define CONTEXT_H

#include "value.h"

typedef struct PropKV {
 char *key;
 Value *val;
 struct PropKV *next;
} PropKV;

typedef struct CtxNode {
    char *name;
    PropKV *props;
    struct CtxNode **children;
    size_t n_children;
} CtxNode;

CtxNode *ctx_new(const char *name);
void ctx_add_child(CtxNode *parent, CtxNode *child);
void ctx_set_prop(CtxNode *n, const char *key, Value *v);
Value *ctx_get_prop(CtxNode *n, const char *key);
CtxNode *ctx_find_child(CtxNode *n, const char *name);
void ctx_free(CtxNode *n);

#endif

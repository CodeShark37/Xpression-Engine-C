#include "context.h"
#include <stdlib.h>
#include <string.h>
#include "error.h"


CtxNode *ctx_new(const char *name) {
    CtxNode *n = xpr_calloc(1,sizeof(CtxNode));
    if (!n) { xpr_error_set(XPR_ERR_MEMORY, "failed to allocate CtxNode"); return NULL; }
    n->name = name ? xpr_strdup(name) : NULL;
    n->props = NULL;
    n->children = NULL;
    n->n_children = 0;
    return n;
}

void ctx_add_child(CtxNode *parent, CtxNode *child) {
    CtxNode **new_children = xpr_realloc(parent->children, sizeof(CtxNode*)*(parent->n_children+1));
    if (!new_children) { xpr_error_set(XPR_ERR_MEMORY, "failed to grow ctx children"); return; }
    parent->children = new_children;
    parent->children[parent->n_children++] = child;
}

void ctx_set_prop(CtxNode *n, const char *key, Value *v) {
    PropKV *kv = xpr_malloc(sizeof(PropKV));
    if (!kv) { xpr_error_set(XPR_ERR_MEMORY, "failed to allocate PropKV"); return; }
    kv->key = xpr_strdup(key);
    kv->val = v;
    kv->next = n->props;
    n->props = kv;
}

Value *ctx_get_prop(CtxNode *n, const char *key) {
    for (PropKV *k = n->props; k; k = k->next) {
		if (!strcmp(k->key, key)) return k->val;
	}
    return NULL;
}

CtxNode *ctx_find_child(CtxNode *n, const char *name) {
    for (size_t i=0;i<n->n_children;i++){ 
		if (!strcmp(n->children[i]->name,name)) 
			return n->children[i];
	}
    return NULL;
}

void ctx_free(CtxNode *n) {
    if (!n) return;
    free(n->name);
    for (PropKV *k=n->props;k;) {
		PropKV *nx = k->next;
		free(k->key);
		val_free(k->val);
		free(k); k = nx;
	}
    for (size_t i=0;i<n->n_children;i++){
		ctx_free(n->children[i]);
		n->children[i] = NULL;
	}
    free(n->children);
	n->children=NULL;
    free(n);
	n=NULL;
}

/**
 * @file context.c
 * @brief Implementation of hierarchical context nodes.
 *
 * @ingroup context
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#include "xpression.h"

#include <stdlib.h>
#include <string.h>


CtxNode* ctx_new(const char* name) {
    if (!name || !*name) return NULL;
    CtxNode* n = calloc(1, sizeof(CtxNode));
    if (!n) {
        error_add(SERR_MEMORY_ALLOC);
        return NULL;
    }
    n->name = strdup(name);
    if (!n->name) {
        error_add(SERR_MEMORY_ALLOC);
        ctx_free(&n);
        return NULL;
    }
    return n;
}

bool ctx_add_child(CtxNode* parent, CtxNode* child) {
    if (!parent || !child) return false;
    size_t size = parent->n_children + 1;
    CtxNode** tmp = realloc(parent->children, sizeof(CtxNode*) * size);
    if (!tmp) {
        error_add(SERR_MEMORY_ALLOC);
        ctx_free(&child);
        return false;
    }
    parent->children = tmp;
    parent->children[parent->n_children++] = child;
    return true;
}

static void propkv_free(PropKV* kv) {
    if (!kv) return;
    if (kv->key) free(kv->key);
    val_free(&kv->val);
    free(kv);
}

bool ctx_set_prop(CtxNode* n, const char* key, Value v) {
    if (!n || !key) return false;

    for (PropKV* k = n->props; k; k = k->next) {
        if (!strcmp(k->key, key)) {
            val_free(&k->val);
            k->val = v;
            return true;
        }
    }

    PropKV* kv = calloc(1, sizeof(PropKV));
    if (!kv) {
        error_add(SERR_MEMORY_ALLOC);
        val_free(&v);
        return false;
    }
    kv->key = strdup(key);
    if (!kv->key) {
        error_add(SERR_MEMORY_ALLOC);
        val_free(&v);
        free(kv);
        return false;
    }
    kv->val  = v;
    kv->next = n->props;
    n->props = kv;
    return true;
}

Value ctx_get_prop(CtxNode* n, const char* key) {
    if (!n || !key) return val_null();
    for (PropKV* k = n->props; k; k = k->next) {
        if (strcmp(k->key, key) == 0) return k->val;
    }
    return val_null();
}

CtxNode* ctx_find_child(CtxNode* n, const char* name) {
    if (!n || !name) return NULL;
    for (size_t i = 0; i < n->n_children; i++) {
        if (strcmp(n->children[i]->name, name) == 0)
            return n->children[i];
    }
    return NULL;
}

void ctx_free(CtxNode** np) {
    if (!np || !(*np)) return;
    CtxNode* n = *np;

    if (n->name) {
        free(n->name);
        n->name = NULL;
    }

    PropKV* current = n->props;
    while (current) {
        PropKV* next = current->next;
        propkv_free(current);
        current = next;
    }
    n->props = NULL;

    if (n->children) {
        for (size_t i = 0; i < n->n_children; i++) {
            if (n->children[i]) ctx_free(&n->children[i]);
        }
        free(n->children);
        n->children = NULL;
    }

    free(n);
    *np = NULL;
}
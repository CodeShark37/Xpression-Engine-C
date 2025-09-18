#include "evaluator.h"
#include "functions.h"
#include "value.h"
#include <stdlib.h>
#include <string.h>

static Value *resolve_chain(Node *n, CtxNode *root);

static Value *resolve_property(Node *child, CtxNode **pcurctx, CtxNode *root, Value *current_val) {
    if (*pcurctx) {
        CtxNode *next = ctx_find_child(*pcurctx, child->value);
        if (next) {
            *pcurctx = next;
            val_free(current_val);
            Value *maybe = ctx_get_prop(*pcurctx, "value");
            current_val = maybe ? val_dup(maybe) : val_null();
        } else {
            Value *pv = ctx_get_prop(*pcurctx, child->value);
            val_free(current_val);
            current_val = pv ? val_dup(pv) : val_null();
        }
    } else {
        Value *pv = ctx_get_prop(root, child->value);
        val_free(current_val);
        current_val = pv ? val_dup(pv) : val_null();
    }

    if (child->n_children > 0) {
        Value *tmp = resolve_chain(child, ((*pcurctx) ? *pcurctx : root));
        val_free(current_val);
        current_val = tmp;
    }
    return current_val;
}

static Value *resolve_function(Node *child, CtxNode *root, Value *current_val) {
    size_t na = child->n_children;
    Value **argv = calloc(na, sizeof(Value*));
    for (size_t a = 0; a < na; a++) {
        argv[a] = eval_node(child->children[a], root);
    }

    FuncCB cb = find_func_cb(child->value);
    Value *res = NULL;
    Value *this_for_cb = current_val ? val_dup(current_val) : val_null();
    if (cb) res = cb(this_for_cb, argv, na);
    else res = val_null();

    for (size_t a = 0; a < na; a++) val_free(argv[a]);
    free(argv);
    val_free(this_for_cb);
    val_free(current_val);
    return res;
}

static Value *resolve_list(Node *child, CtxNode *curctx, CtxNode *root, Value *current_val) {
    CtxNode *eval_ctx = curctx ? curctx : root;

    if (child->n_children == 1) {
        Value *v = eval_node(child->children[0], eval_ctx);
        val_free(current_val);
        return v;
    } else {
        Value *L = val_list();
        for (size_t a = 0; a < child->n_children; a++) {
            Value *it = eval_node(child->children[a], eval_ctx);
            val_list_append(L, it);
        }
        val_free(current_val);
        return L;
    }
}

static Value *resolve_fallback(Node *child, CtxNode *root, Value *current_val) {
    Value *r = eval_node(child, root);
    val_free(current_val);
    return r;
}

static Value *resolve_chain(Node *n, CtxNode *root) {
    if (!n || !n->value) return val_null();

    CtxNode *curctx = ctx_find_child(root, n->value);
    Value *current_val = NULL;

    if (curctx) {
        current_val = val_null();
    } else {
        Value *pv = ctx_get_prop(root, n->value);
        current_val = pv ? val_dup(pv) : val_null();
    }

    for (size_t i = 0; i < n->n_children; i++) {
        Node *child = n->children[i];
        if (!child) continue;

        switch (child->kind) {
            case NODE_PROPERTY:
                current_val = resolve_property(child, &curctx, root, current_val);
                break;
            case NODE_FUNCTION:
                current_val = resolve_function(child, root, current_val);
                break;
            case NODE_LIST:
                current_val = resolve_list(child, curctx, root, current_val);
                break;
            default:
                current_val = resolve_fallback(child, root, current_val);
                break;
        }
    }

    return current_val;
}

Value *eval_node(Node *n, CtxNode *ctx) {
    if (!n) return val_null();
    switch (n->kind) {
        case NODE_IDENTIFIER: {
            Value *v = ctx_get_prop(ctx, n->value);
            if (v) return val_dup(v);
            /* fallback: return identifier as string */
            return val_str(n->value);
        }
		
        case NODE_OBJECT:
        case NODE_PROPERTY:
            return resolve_chain(n, ctx);

        case NODE_STRING:
            return val_str(n->value ? n->value : "");

        case NODE_NUMBER:
            return val_num(n->value ? atof(n->value) : 0.0);
		
		case NODE_BOOL:
			return val_bool(!strcasecmp(n->value, "true"));
      
        case NODE_FUNCTION: {
            size_t na = n->n_children;
            Value **argv = calloc(na, sizeof(Value*));
            for (size_t i = 0; i < na; i++) {
                /* For top-level function calls we evaluate args in ctx (root caller's context) */
                argv[i] = eval_node(n->children[i], ctx);
            }
            FuncCB cb = find_func_cb(n->value);
            Value *res = NULL;
            if (cb) res = cb(NULL, argv, na);
            else res = val_null();
            for (size_t i = 0; i < na; i++) val_free(argv[i]);
            free(argv);
            return res;
        }

        case NODE_LIST: {
            /* generic list evaluation (used when list appears stand-alone, or as function arg).
               We evaluate items in the provided ctx. */
            Value *L = val_list();
            for (size_t i = 0; i < n->n_children; i++) {
                Value *it = eval_node(n->children[i], ctx);
                val_list_append(L, it);
            }
            return L;
        }
    }
    return val_null();
}

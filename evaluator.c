#include "evaluator.h"
#include <stdlib.h>

static Value *resolve_chain(Node *n, CtxNode *root) {
    if (!n || !n->value) return val_null();
    CtxNode *curctx = ctx_find_child(root, n->value);
    Value *current_val = NULL;
    if (curctx) current_val = val_null();
    else {
        Value *pv = ctx_get_prop(root, n->value);
        current_val = pv ? val_dup(pv) : val_null();
    }

    for (size_t i=0;i<n->n_children;i++) {
        Node *child = n->children[i];
        if (!child) continue;
        if (child->kind == NODE_PROPERTY) {
            if (curctx) {
                CtxNode *next = ctx_find_child(curctx, child->value);
                if (next) {
                    curctx = next;
                    val_free(current_val);
                    Value *maybe = ctx_get_prop(curctx, "value");
                    current_val = maybe ? val_dup(maybe) : val_null();
                } else {
                    Value *pv = ctx_get_prop(curctx, child->value);
                    val_free(current_val);
                    current_val = pv ? val_dup(pv) : val_null();
                }
            } else {
                Value *pv = ctx_get_prop(root, child->value);
                val_free(current_val);
                current_val = pv ? val_dup(pv) : val_null();
            }
            if (child->n_children > 0) {
                Value *tmp = resolve_chain(child, curctx ? curctx : root);
                val_free(current_val);
                current_val = tmp;
            }
        } else if (child->kind == NODE_FUNCTION) {
            size_t na = child->n_children;
            Value **argv = calloc(na, sizeof(Value*));
            for (size_t a=0;a<na;a++) argv[a] = eval_node(child->children[a], root);
            FuncCB cb = find_func_cb(child->value);
            Value *res = NULL;
            Value *this_for_cb = current_val ? val_dup(current_val) : val_null();
            if (cb) res = cb(this_for_cb, argv, na);
            else res = val_null();
            for (size_t a=0;a<na;a++) val_free(argv[a]);
            free(argv);
            val_free(this_for_cb);
            val_free(current_val);
            current_val = res;
        } else if (child->kind == NODE_LIST) {
            Value *L = val_list();
            for (size_t a=0;a<child->n_children;a++) val_list_append(L, eval_node(child->children[a], root));
            val_free(current_val);
            current_val = L;
        } else {
            Value *r = eval_node(child, root);
            val_free(current_val);
            current_val = r;
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
            // fallback: return identifier as string (helps MIXED, etc.)
            return val_str(n->value);
        }
        case NODE_OBJECT:
        case NODE_PROPERTY:
            return resolve_chain(n, ctx);
        case NODE_STRING:
            return val_str(n->value ? n->value : "");
        case NODE_NUMBER:
            return val_num(n->value ? atof(n->value) : 0.0);
        case NODE_FUNCTION: {
            size_t na = n->n_children;
            Value **argv = calloc(na, sizeof(Value*));
            for (size_t i=0;i<na;i++) argv[i] = eval_node(n->children[i], ctx);
            FuncCB cb = find_func_cb(n->value);
            Value *res = NULL;
            if (cb) res = cb(NULL, argv, na);
            else res = val_null();
            for (size_t i=0;i<na;i++) val_free(argv[i]);
            free(argv);
            return res;
        }
        case NODE_LIST: {
            Value *L = val_list();
            for (size_t i=0;i<n->n_children;i++) val_list_append(L, eval_node(n->children[i], ctx));
            return L;
        }
    }
    return val_null();
}

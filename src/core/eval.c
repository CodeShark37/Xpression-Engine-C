/**
 * @file eval.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-06-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdbool.h>
#include <stdlib.h>

#include "eval.h"
#include "functions.h"

static Value resolve_chain(Node* n, CtxNode* root);

/**
 * @brief Resolve a property access in a context chain.
 * @param child The property node to resolve.
 * @param pcurctx Pointer to the current context (may be updated).
 * @param root The root context.
 * @param current_val The current value (will be freed and replaced).
 * @return A new Value (caller must free with val_free).
 */
static Value resolve_property(Node* child, CtxNode** pcurctx, CtxNode* root, Value current_val) {
    if (*pcurctx) {
        CtxNode* next = ctx_find_child(*pcurctx, child->value);
        if (next) {
            *pcurctx = next;
            val_free(&current_val);
            Value maybe = ctx_get_prop(*pcurctx, "value");
            current_val = val_dup(maybe);
        } else {
            Value pv = ctx_get_prop(*pcurctx, child->value);
            val_free(&current_val);
            current_val = val_dup(pv);
        }
    } else {
        Value pv = ctx_get_prop(root, child->value);
        val_free(&current_val);
        current_val = val_dup(pv);
    }

    if (child->n_children > 0) {
        Value tmp = resolve_chain(child, (*pcurctx) ? *pcurctx : root);
        val_free(&current_val);
        current_val = tmp;
    }
    return current_val;
}

static Value resolve_function(Node* child, CtxNode* root, Value current_val, ErrorContext* err) {
    size_t na = child->n_children;
    Value* argv = calloc(na, sizeof(Value));
    if (!argv) {
        error_add(err, SERR_MEMORY_ALLOC);
        val_free(&current_val);
        return val_null();
    }
    for (size_t a = 0; a < na; a++) {
        argv[a] = eval(child->children[a], root);
    }

    Func cb = find_function(child->value);
    Value this_for_cb = val_dup(current_val);
    Value res = cb ? cb(this_for_cb, argv, na) : val_null();

    for (size_t a = 0; a < na; a++) val_free(&argv[a]);
    free(argv);
    val_free(&this_for_cb);
    val_free(&current_val);
    return res;
}

static Value resolve_list(Node* child, CtxNode* curctx, CtxNode* root, Value current_val) {
    CtxNode* eval_ctx = curctx ? curctx : root;

    if (child->n_children == 1) {
        Value v = eval(child->children[0], eval_ctx);
        val_free(&current_val);
        return v;
    }

    Value L = val_list();
    for (size_t a = 0; a < child->n_children; a++) {
        Value it = eval(child->children[a], eval_ctx);
        if (!val_list_append(L, it)) {
            val_free(&L);
            val_free(&current_val);
            return val_null();
        }
    }
    val_free(&current_val);
    return L;
}

static Value resolve_fallback(Node* child, CtxNode* root, Value current_val) {
    Value r = eval(child, root);
    val_free(&current_val);
    return r;
}

/* 1. resolve_chain: lhs passa a ser children[0], loop começa em i=1 */
static Value resolve_chain(Node* n, CtxNode* root, ErrorContext *err) {
    if (!n || !n->n_children) return val_null();

    Node* lhs = n->children[0];
    if (!lhs || !lhs->value) return val_null();

    CtxNode* curctx = ctx_find_child(root, lhs->value);
    Value current_val;

    if (curctx) {
        current_val = val_null();
    } else {
        Value pv = ctx_get_prop(root, lhs->value);
        current_val = val_dup(pv);
    }

    for (size_t i = 1; i < n->n_children; i++) {
        Node* child = n->children[i];
        if (!child) continue;

        switch (child->kind) {
            case NODE_OBJECT:
                current_val = resolve_property(child, &curctx, root, current_val);
                break;
            case NODE_FUNCTION:
                current_val = resolve_function(child, root, current_val, err);
                break;
            case NODE_MULTI_ACCESS:
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

/* Constru��o de contexto a partir de NODE_CTX_DEF */
// static Value* eval_ctx_def(Node* node, CtxNode* parent) {
//    /* Cria um novo contexto din�mico */
//    CtxNode* ctx = ctx_new(node->value);
//    for (size_t i = 0; i < node->n_children; i++) {
//        Node* prop = node->children[i];
//        if (!prop || prop->kind != NODE_OBJECT) continue;
//        const char* key = prop->value;
//        Value* val = NULL;
//        if (prop->n_children == 1)
//            val = eval(prop->children[0], parent);
//        else
//            val = val_null();
//        ctx_set_prop(ctx, key, val);
//        val_free(&val);
//    }
//    /* devolve contexto como Value */
//      /* fun��o j� existente no teu runtime */
//    return val_null();
// }

static Value eval(Node* n, EvalContext* ctx) {
    if (!n) return val_null();

    switch (n->kind) {
        case NODE_ACCESS:
            return resolve_chain(n, ctx->root, ctx->errors);

        case NODE_OBJECT: {
            Value pv = ctx_get_prop(ctx->root, n->value);
            return val_dup(pv);
        }

        case NODE_STRING:
            return val_str(n->value ? n->value : "");

        case NODE_NUMBER:
            return val_num(n->value ? atof(n->value) : 0.0);

        case NODE_BOOL:
            return val_bool(n->len == 4); /* "true" tem 4 chars */

        case NODE_FUNCTION: {
            size_t na = n->n_children;
            Value* argv = calloc(na, sizeof(Value));
            if (!argv) {
                error_add(ctx->errors, SERR_MEMORY_ALLOC);
                return val_null();
            }
            for (size_t i = 0; i < na; i++) {
             /* For top-level function calls evaluate args in ctx (root caller's context) */
                argv[i] = eval(n->children[i], ctx);
            }
            Func cb = find_function(n->value);
            Value res = cb ? cb(val_null(), argv, na) : val_null();
            for (size_t i = 0; i < na; i++) val_free(&argv[i]);
            free(argv);
            return res;
        }

        case NODE_MULTI_ACCESS:
        case NODE_LIST: {
             /* generic list evaluation (used when list appears stand-alone, or as function arg).
               evaluate items in the provided ctx. */
            Value L = val_list();
            for (size_t i = 0; i < n->n_children; i++) {
                Value it = eval(n->children[i], ctx);
                if (!val_list_append(L, it)) {
                    val_free(&L);
                    return val_null();
                }
            }
            if (val_list_len(L)) return L;
            val_free(&L);
            return val_null();
        }
        //case NODE_CTX_DEF:
        
        default:
            return val_null();
    }
}

Value eval_xpression(XpressionContext *xpr) {
    
    EvalContext ctx = {
        .node = xpr->ast,
        .root = xpr->eval_ctx,
        .errors  = &xpr->errors
    }; 

   return eval(ctx.node, &ctx);
}
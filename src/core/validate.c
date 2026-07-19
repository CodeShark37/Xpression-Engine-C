/**
 validate.c
 Uses FuncEntry descriptors from functions.c to validate:
  - function existence (unknown functions are errors)
  - arity (min/max)
  - basic argument kinds (when allowed_kinds_mask is set)
 Reports errors using node positional info via error_add_ctx and collects multiple errors.
*/

#include "xpression.h"
#include "validate.h"
#include "functions.h"

#include <stdlib.h>
#include <string.h>


ArgKindMask nodekind_to_mask(NodeKind k) {
    switch (k) {
        case NODE_NUMBER: return AK_NUMBER;
        case NODE_STRING: return AK_STRING;
        case NODE_BOOL:   return AK_BOOL;
        case NODE_LIST:   return AK_LIST;
        case NODE_NULL:   return AK_NULL;
        case NODE_OBJECT: return AK_OBJECT;
        default: return AK_ANY;
    }
}

/* Map ArgKindMask to NodeKind  - ensure NODE_FUNCTION is handled explicitly */
NodeKind mask_to_nodekind(ArgKindMask k) {
    switch (k) {
        case AK_NULL:   return NODE_NULL;
        case AK_NUMBER: return NODE_NUMBER;
        case AK_STRING: return NODE_STRING;
        case AK_BOOL:   return NODE_BOOL;
        case AK_LIST:   return NODE_LIST;
        case AK_OBJECT: return NODE_OBJECT;
        default: return NODE_UNKNOWN;
    }
}

static int8_t bit_pos(uint8_t *n) {
    uint8_t v;
    int8_t pos = 0;

    if (!*n) return -1;
    v = *n;
    while (!(v & 1u)) {
        v >>= 1;
        pos++;
    }
    *n &= *n - 1u;
    return pos;
}

static char* fmt_bitmask(uint8_t mask) {
    uint8_t tmp = mask;
    size_t len = 0;
    size_t count = 0;
    char *res;

    /* 1� passagem: calcular tamanho */
    while (tmp) {
        int8_t pos = bit_pos(&tmp);
        if(pos < 0) return NULL;
        uint8_t bit = 1u << pos;

        if (count > 0)
            len += 4; /* " or " */

        len += strlen(nodekind_name(mask_to_nodekind(bit)));
        count++;
    }

    if (!len)
        return NULL;

    res = calloc(len + 1, sizeof(char));
    if (!res) {
        error_add(VERR_MEMORY_ALLOC);
        return NULL;
    }

    /* 2� passagem: construir string */
    tmp = mask;
    char *ptr = res;
    bool first = true;

    while (tmp) {
        int8_t pos = bit_pos(&tmp);
        if(pos < 0){
            free(res);
            res = NULL;
            return res;
        }
        uint8_t bit = 1u << pos;
        const char *name = nodekind_name(mask_to_nodekind(bit));
        size_t nlen = strlen(name);

        if (!first) {
            memcpy(ptr, " or ", 4);
            ptr += 4;
        }

        memcpy(ptr, name, nlen);
        ptr += nlen;
        first = false;
        name = NULL;
    }

    return res;
}

static bool validate_node(NodeKind prev, Node* n, CtxNode* ctx, size_t depth);

static bool validate_children(NodeKind prev, Node* n, CtxNode* ctx, size_t depth) {
    if(!n) return false;
    bool ok = true;
    for (size_t i = 0; i < n->n_children; ++i) {
        if (!validate_node(prev, n->children[i],ctx, depth + 1)) ok = false;
    }
    return ok;
}

static int validate_function_arity(FuncEntry* fe,Node* n,int8_t got_arity) {
    if(!fe || !n) return false;

    ErrorType err = VERR_NONE;
    int8_t need_arity = -1;

    if(got_arity < fe->desc->min_args){
        err = VERR_FUNCTION_ARITY_MIN;
        need_arity = fe->desc->min_args;
    }else if((fe->desc->max_args >= 0) &&
              (got_arity > fe->desc->max_args)){
        err = VERR_FUNCTION_ARITY_MAX;
        need_arity = fe->desc->max_args;
    }

    if(err != VERR_NONE){
        error_add_ctx(err, n->line, n->col,
                      n->len, NULL,
                      fe->name, need_arity, got_arity);
        return false;
    }

    return true;
}

static bool validate_function_args(FuncEntry* fe,Node* arg, uint8_t gotmask,size_t arg_pos){
    if(!fe || !arg) return false;

    char* expected_str = NULL;
    char* got_str = NULL;
    bool ok = true;

    // Determinar a m�scara do que � exigido (needmask)
    ArgKindMask needmask = AK_ANY;
    if (arg_pos < fe->desc->pos_count && fe->desc->pos_arg_kinds) {
        needmask = fe->desc->pos_arg_kinds[arg_pos];
    } else if (fe->desc->allowed_kinds_mask != AK_NONE) {
        needmask = fe->desc->allowed_kinds_mask;
    }

    // L�GICA DE VALIDA��O:
    if (needmask != AK_ANY && gotmask != AK_ANY) {
        if (!ak_has(gotmask,(needmask | AK_NULL))) {

            expected_str = fmt_bitmask(needmask);
            got_str = (check_nodekind(arg, NODE_FUNCTION))
                       ? fmt_bitmask(gotmask)
                       : strdup(nodekind_name(arg->kind));

            error_add_ctx(VERR_FUNCTION_ARG_TYPE,
                          arg->line, arg->col,
                          arg->len, NULL,
                          fe->name, (int)(arg_pos + 1),
                          expected_str ? expected_str : "<?>",
                          got_str ? got_str : "<?>");

            ok = false;
        }
    }
    if (expected_str) free(expected_str);
    if (got_str) free(got_str);
    expected_str = NULL;
    got_str = NULL;

   return ok;
}

static bool validate_function(NodeKind prev_kind, Node* n, CtxNode* ctx, size_t depth) {
    if (!n->value || !*n->value) {
        return false;
    }
 
    if (prev_kind != NODE_LIST &&
        prev_kind != NODE_UNKNOWN &&
        prev_kind != NODE_FUNCTION &&
        prev_kind != NODE_ACCESS) {
        return false;
    }
 
    FuncEntry* fe = find_function_entry(n->value);
    if (!fe) {
        error_add_ctx(VERR_UNKNOWN_FUNCTION, n->line, n->col, n->len, NULL, n->value);
        return false;
    }
 
    if (fe->desc == NULL) return true;
    bool ok = true;
 
    size_t got = n->n_children;
    if (!validate_function_arity(fe, n, got)){
        ok = false;
    }
 
    for (size_t i = 0; i < n->n_children && ok; ++i) {
        Node* arg = n->children[i];
        if (!arg) continue;
 
        if (!validate_node(NODE_UNKNOWN, arg, ctx, depth + 1)) {
            ok = false;
            continue;
        }
 
        unsigned int gotmask = AK_NONE;
        if (check_nodekind(arg, NODE_FUNCTION)) {
            FuncEntry* inner_entry = find_function_entry(arg->value);
            gotmask = (inner_entry && inner_entry->desc) ?
                        inner_entry->desc->ret_kind_mask : AK_ANY;
        } else {
            gotmask = nodekind_to_mask(arg->kind);
        }
        if(!validate_function_args(fe,arg,gotmask,i)){
            ok = false;
        }
        arg = NULL;
    } 
    return ok;
}

// check for duplicated arguments
// return false if found duplicated arg
static bool unique_arg(Node* n) {
    bool ok = true;

    for (size_t k = 0; k < n->n_children; k++) {
        for (size_t i = k + 1; i < n->n_children; i++) {
            if (node_equal(n->children[k], n->children[i])) {
                error_add_ctx(VERR_DUP_ARG_MULTI_ACCESS,
                              n->children[i]->line, n->children[i]->col,
                              n->children[i]->len, NULL, i + 1);
                ok = false;
                break;
            }
        }
    }
    return ok;
}

static bool validate_list(NodeKind prev, Node* n, CtxNode* ctx, size_t depth){
    (void)prev;
    if(!n) return false;
 
    if (!n->n_children) {
        error_add_ctx(VERR_EMPTY_ARRAY, n->line, n->col, n->len, NULL);
        return false;
    }
 
    return validate_children(NODE_LIST, n, ctx, depth);
}

static bool validate_access(NodeKind prev, Node* n, CtxNode* ctx, size_t depth) {
    (void)prev;
    if (!n->n_children) return false;
 
    /* children[0] is the lhs — must be NODE_OBJECT */
    Node* lhs = n->children[0];
    if (!check_nodekind(lhs, NODE_OBJECT)) {
        error_add_ctx(VERR_EXPECTED_PROPERTY_OR_MULTI_ACCESS,
                      lhs->line, lhs->col, lhs->len, NULL);
        return false;
    }
 
    bool ok = true;
    /* children[1..n] are the access steps — must be NODE_OBJECT or NODE_MULTI_ACCESS */
    for (size_t i = 1; i < n->n_children; ++i) {
        Node* step = n->children[i];
        if (!step) continue;
 
        if (!check_nodekind(step, NODE_OBJECT) && !check_nodekind(step, NODE_MULTI_ACCESS)) {
            error_add_ctx(VERR_EXPECTED_PROPERTY_OR_MULTI_ACCESS,
                          step->line, step->col, step->len, NULL);
            ok = false;
            continue;
        }
 
        if (!validate_node(NODE_ACCESS, step, ctx, depth + 1)) ok = false;
    }
    return ok;
}

static bool validate_multi_access(NodeKind prev, Node* cur, CtxNode* ctx, size_t depth) {
    (void)prev;(void)ctx;(void)depth;

    bool ok = true;

    if (!cur->n_children) {
        error_add_ctx(VERR_EMPTY_MULTI_ACCESS, cur->line, cur->col, cur->len, NULL);
        return false;
    }

    if(cur->n_children < 2){
        error_add_ctx(VERR_FEW_ARGS_MULTI_ACCESS, cur->children[0]->line,
                      cur->children[0]->col, cur->children[0]->len,
                      NULL, cur->n_children);
        return false;
    }

     for (size_t i = 0; i < cur->n_children; ++i) {
        Node* item = cur->children[i];

        if (!check_nodekind(item, NODE_OBJECT) && !check_nodekind(item, NODE_ACCESS)) {
            error_add_ctx(VERR_EXPECTED_OBJECT_MULTI_ACCESS,
                          item->line, item->col, item->len, NULL,
                          i + 1, nodekind_name(item->kind));
            ok = false;
            continue; /* item inválido: não vale a pena validar recursivamente */
        }

        if (!validate_node(NODE_MULTI_ACCESS, item, ctx, depth + 1)) ok = false;
    }

    if (ok && !unique_arg(cur)) ok = false;
    return ok;
}

static bool validate_node(NodeKind prev_kind, Node* cur, CtxNode* ctx, size_t depth) {
    if (!cur) return false;
    if (depth > VALIDATE_MAX_DEPTH) {
        error_add_ctx(VERR_MAX_DEPTH, cur->line, cur->col, cur->len, NULL);
        return false;
    }

    switch (cur->kind) {
        case NODE_FUNCTION:
            return validate_function(prev_kind,cur, ctx, depth);
        case NODE_ACCESS:
            return validate_access(prev_kind, cur, ctx, depth);
        case NODE_MULTI_ACCESS:
            return validate_multi_access(prev_kind, cur, ctx, depth);
        case NODE_LIST:
            return validate_list(prev_kind, cur, ctx, depth);
        case NODE_OBJECT:
        case NODE_STRING:
        case NODE_NUMBER:
        case NODE_BOOL:
        case NODE_NULL:
            return true;
        default:
            error_add_ctx(VERR_UNKNOWN_NODE,
                          cur->line, cur->col,
                          cur->len, NULL,
                          nodekind_name(cur->kind));
           return false;
    }
}

bool validate_xpression(XpressionContext* xpr) {
    Node* root = xpr->ast;
    CtxNode* ctx = xpr->eval_ctx;

    if (!root || root->kind != NODE_LIST) return false;

    if (!root->n_children) {
        error_add(VERR_NO_EXPRESSION_FOUND);
        return false;
    }
    /* validate each child but keep collecting errors */
    return validate_children(NODE_UNKNOWN, root, ctx, 0);
}

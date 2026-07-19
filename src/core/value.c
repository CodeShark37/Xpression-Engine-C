/**
	XPression Engine C (c)2025

	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com

	value.c
*/

#include "xpression.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 


#ifdef VALBOX_32

inline bool val_is_number(Value v) { return (v.tag & 0x7FF00000u) != 0x7FF00000u; }
inline bool val_is_null  (Value v) { return v.tag == _TAG_NULL;   }
inline bool val_is_bool  (Value v) { return v.tag == _TAG_BOOL;   }
inline bool val_is_string(Value v) { return v.tag == _TAG_STRING; }
inline bool val_is_list  (Value v) { return v.tag == _TAG_LIST;   }

#elif defined(VALBOX_64)

inline bool val_is_number(Value v) { return (v & _NAN_MASK) != _NAN_MASK;     }
inline bool val_is_null  (Value v) { return v == _TAG_NULL;                    }
inline bool val_is_bool  (Value v) { return (v & ~(uint64_t)1u) == _TAG_BOOL; }
inline bool val_is_string(Value v) { return (v & _TAG_MASK) == _TAG_STRING;   }
inline bool val_is_list  (Value v) { return (v & _TAG_MASK) == _TAG_LIST;     }

#endif

ValKind val_kind(Value v) {
    if (val_is_number(v)) return VAL_NUMBER;
    if (val_is_null(v))   return VAL_NULL;
    if (val_is_bool(v))   return VAL_BOOL;
    if (val_is_string(v)) return VAL_STRING;
    if (val_is_list(v))   return VAL_LIST;
    return VAL_NULL;
}

/* ════════════════════════════════════════════════════════
   BOXING / UNBOXING
   ════════════════════════════════════════════════════════ */

#ifdef VALBOX_32

Value val_null(void) {
    return (Value){ .payload = 0, .tag = _TAG_NULL };
}

Value val_bool(bool b) {
    return (Value){ .payload = (uint32_t)!!b, .tag = _TAG_BOOL };
}

Value val_num(double d) {
    Value v;
    memcpy(&v, &d, 8);
    return v;
}

Value val_box_str(void *p) {
    return (Value){ .payload = (uint32_t)p, .tag = _TAG_STRING };
}

Value val_box_list(void *p) {
    return (Value){ .payload = (uint32_t)p, .tag = _TAG_LIST };
}

double unbox_num(Value v)  { double d; memcpy(&d, &v, 8); return d; }
bool   unbox_bool(Value v) { return (bool)v.payload; }
void  *unbox_ptr(Value v)  { return (void*)v.payload; }

#elif defined(VALBOX_64)

inline Value val_null(void) {
    return _TAG_NULL;
}

inline Value val_bool(bool b) {
    return _TAG_BOOL | (uint64_t)!!b;
}

Value val_num(double d) {
    Value v;
    memcpy(&v, &d, 8);
    return v;
}

inline Value val_box_str(void *p) {
    return _TAG_STRING | ((uint64_t)(uintptr_t)p & _PTR_MASK);
}

inline Value val_box_list(void *p) {
    return _TAG_LIST | ((uint64_t)(uintptr_t)p & _PTR_MASK);
}

double unbox_num(Value v)  { double d; memcpy(&d, &v, 8); return d; }
inline bool   unbox_bool(Value v) { return (bool)(v & 1); }
inline void  *unbox_ptr(Value v)  { return (void*)(uintptr_t)(v & _PTR_MASK); }

#endif

/* ════════════════════════════════════════════════════════
   CONSTRUTORES — API pública (alloc)
   ════════════════════════════════════════════════════════ */

Value val_str(const char *s) {
    if (!s) return val_null();
    size_t len = strlen(s);
    char *copy = calloc(len + 1, sizeof(char));
    if (!copy) {
        error_add(SERR_MEMORY_ALLOC);
        return val_null();
    }
    memcpy(copy, s, len);
    return val_box_str(copy);
}

Value val_list(void) {
    ValList *vl = calloc(1, sizeof(ValList));
    if (!vl) {
        error_add(SERR_MEMORY_ALLOC);
        return val_null();
    }
    vl->_state = _VL_BUILDING;
    vl->len    = 0;
    list_init(&vl->_list);
    return val_box_list(vl);
}

/* ════════════════════════════════════════════════════════
   VALLIST — append e freeze implícito
   ════════════════════════════════════════════════════════ */

bool val_list_append(Value list, Value item) {
    if (!val_is_list(list)) return false;
    ValList *vl = (ValList*)unbox_ptr(list);

    if (vl->_state == _VL_FROZEN) {
        /* Lista já congelada — não há suporte para voltar a crescer.
           Decisão de design: append pós-freeze falha. */
        return false;
    }

    ValNode *n = malloc(sizeof(ValNode));
    if (!n) {
        error_add(SERR_MEMORY_ALLOC);
        return false;
    }
    n->val = item;
    list_push_back(&vl->_list, &n->node);
    vl->len++;
    return true;
}

/* Congela a lista — converte nós ligados em array contíguo.
   Chamado internamente na primeira leitura por índice. */
static bool _val_list_freeze(ValList *vl) {
    if (vl->_state == _VL_FROZEN) return true;

    Value *arr = NULL;
    if (vl->len) {
        arr = malloc(sizeof(Value) * vl->len);
        if (!arr) {
            error_add(SERR_MEMORY_ALLOC);
            return false;
        }
    }

    size_t i = 0;
    list_for_each_safe(it, tmp, &vl->_list) {
        ValNode *n = container_of(it, ValNode, node);
        arr[i++] = n->val;
        list_remove(it);
        free(n);
    }

    vl->_state = _VL_FROZEN;
    vl->_data  = arr;
    return true;
}

/* ════════════════════════════════════════════════════════
   VAL_FREE — liberta recursivamente
   ════════════════════════════════════════════════════════ */

void val_free(Value *v) {
    if (!v) return;

    switch (val_kind(*v)) {
        case VAL_STRING: {
            void *p = unbox_ptr(*v);
            if (p) free(p);
            break;
        }
        case VAL_LIST: {
            ValList *vl = (ValList*)unbox_ptr(*v);
            if (!vl) break;

            if (vl->_state == _VL_BUILDING) {
                list_for_each_safe(it, tmp, &vl->_list) {
                    ValNode *n = container_of(it, ValNode, node);
                    val_free(&n->val);
                    free(n);
                }
            } else {
                for (size_t i = 0; i < vl->len; i++) {
                    val_free(&vl->_data[i]);
                }
                free(vl->_data);
            }
            free(vl);
            break;
        }
        default:
            /* VAL_NULL, VAL_BOOL, VAL_NUMBER — sem heap, nada a fazer */
            break;
    }

    *v = val_null();
}

/* ════════════════════════════════════════════════════════
   VALLIST — acesso por índice (freeze implícito)
   ════════════════════════════════════════════════════════ */

size_t val_list_len(Value list) {
    if (!val_is_list(list)) return 0;
    ValList *vl = (ValList*)unbox_ptr(list);
    return vl->len;
}

Value val_list_get(Value list, size_t i) {
    if (!val_is_list(list)) return val_null();
    ValList *vl = (ValList*)unbox_ptr(list);

    if (vl->_state == _VL_BUILDING) {
        if (!_val_list_freeze(vl)) return val_null();
    }
    if (i >= vl->len) return val_null();
    return vl->_data[i];
}

/* ════════════════════════════════════════════════════════
   VAL_DUP — cópia profunda
   ════════════════════════════════════════════════════════ */

Value val_dup(Value src) {
    switch (val_kind(src)) {
        case VAL_NULL:   return val_null();
        case VAL_BOOL:   return val_bool(unbox_bool(src));
        case VAL_NUMBER: return val_num(unbox_num(src));
        case VAL_STRING: return val_str((char*)unbox_ptr(src));
        case VAL_LIST: {
            Value L = val_list();
            size_t n = val_list_len(src);
            for (size_t i = 0; i < n; i++) {
                Value item = val_dup(val_list_get(src, i));
                if (!val_list_append(L, item)) {
                    val_free(&L);
                    return val_null();
                }
            }
            return L;
        }
        default:
            return val_null();
    }
}

/* ════════════════════════════════════════════════════════
   VAL_EQUALS
   ════════════════════════════════════════════════════════ */

static bool _val_list_equals(Value a, Value b) {
    size_t na = val_list_len(a);
    size_t nb = val_list_len(b);
    if (na != nb) return false;

    for (size_t i = 0; i < na; i++) {
        if (!val_equals(val_list_get(a, i), val_list_get(b, i)))
            return false;
    }
    return true;
}

bool val_equals(Value a, Value b) {
    ValKind ka = val_kind(a);
    ValKind kb = val_kind(b);
    if (ka != kb) return false;

    switch (ka) {
        case VAL_NULL:   return true;
        case VAL_BOOL:   return unbox_bool(a) == unbox_bool(b);
        case VAL_NUMBER: return unbox_num(a) == unbox_num(b);
        case VAL_STRING: {
            char *sa = (char*)unbox_ptr(a);
            char *sb = (char*)unbox_ptr(b);
            if (sa == sb) return true;
            if (!sa || !sb) return false;
            return strcmp(sa, sb) == 0;
        }
        case VAL_LIST:
            return _val_list_equals(a, b);
        default:
            return false;
    }
}

/* ════════════════════════════════════════════════════════
   VAL_COMPARE
   ════════════════════════════════════════════════════════ */

int val_compare(Value a, Value b) {
    ValKind ka = val_kind(a);
    ValKind kb = val_kind(b);
    if (ka != kb) return (ka < kb) ? -1 : 1;

    switch (ka) {
        case VAL_NULL:
            return 0;
        case VAL_BOOL: {
            bool xa = unbox_bool(a), xb = unbox_bool(b);
            return (xa == xb) ? 0 : (xa < xb ? -1 : 1);
        }
        case VAL_NUMBER: {
            double xa = unbox_num(a), xb = unbox_num(b);
            return (xa == xb) ? 0 : (xa < xb ? -1 : 1);
        }
        case VAL_STRING:
            return strcmp((char*)unbox_ptr(a), (char*)unbox_ptr(b));
        case VAL_LIST: {
            size_t na = val_list_len(a), nb = val_list_len(b);
            if (na != nb) return (na < nb) ? -1 : 1;
            for (size_t i = 0; i < na; i++) {
                int cmp = val_compare(val_list_get(a, i), val_list_get(b, i));
                if (cmp != 0) return cmp;
            }
            return 0;
        }
        default:
            return 0;
    }
}

int val_compare_sort(const void *a, const void *b) {
    const Value *va = (const Value*)a;
    const Value *vb = (const Value*)b;
    return val_compare(*va, *vb);
}

/* ════════════════════════════════════════════════════════
   VALUE_TO_STRING
   ════════════════════════════════════════════════════════ */

static size_t _val_list_strlen(Value v) {
    size_t n = val_list_len(v);
    size_t total_len = 2; /* '[' e ']' */

    for (size_t i = 0; i < n; i++) {
        char *s = value_to_string(val_list_get(v, i));
        if (!s) return 0; /* falha na alocação */
        total_len += strlen(s);
        free(s);
        if (i + 1 < n) total_len += 1; /* vírgula entre itens */
    }
    total_len += 1; /* terminador '\0' */
    return total_len;
}

/* Devolve o array interno já congelado, para operações in-place (ex: qsort).
   O array continua a pertencer ao ValList — não fazer free() directo nele. */
Value *val_list_raw(Value list, size_t *out_len) {
    if (!val_is_list(list)) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    ValList *vl = (ValList*)unbox_ptr(list);

    if (vl->_state == _VL_BUILDING) {
        if (!_val_list_freeze(vl)) {
            if (out_len) *out_len = 0;
            return NULL;
        }
    }

    if (out_len) *out_len = vl->len;
    return vl->_data;
}

/* Verifica se 'item' já existe em 'list', SEM forçar freeze.
   Seguro para chamar durante a construção (_VL_BUILDING) — itera a lista
   intrusiva directamente. Se já estiver congelada, itera o array. */
bool val_list_contains(Value list, Value item) {
    if (!val_is_list(list)) return false;
    ValList *vl = (ValList*)unbox_ptr(list);
 
    if (vl->_state == _VL_BUILDING) {
        list_for_each(it, &vl->_list) {
            ValNode *n = container_of(it, ValNode, node);
            if (val_equals(n->val, item)) return true;
        }
        return false;
    }
 
    for (size_t i = 0; i < vl->len; i++) {
        if (val_equals(vl->_data[i], item)) return true;
    }
    return false;
}

char* value_to_string(Value v) {
    switch (val_kind(v)) {
        case VAL_NULL:
            return strdup("null");

        case VAL_STRING: {
            char *s = (char*)unbox_ptr(v);
            return strdup(s ? s : "");
        }

        case VAL_BOOL:
            return strdup(unbox_bool(v) ? "true" : "false");

        case VAL_NUMBER: {
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "%g", unbox_num(v));
            return strdup(tmp);
        }

        case VAL_LIST: {
            size_t len = _val_list_strlen(v);
            if (len == 0) return strdup("[]"); /* vazio ou erro */

            char *out = calloc(len, 1);
            if (!out) {
                error_add(SERR_MEMORY_ALLOC);
                return NULL;
            }

            size_t n = val_list_len(v);
            size_t pos = 0;
            out[pos++] = '[';

            for (size_t i = 0; i < n; i++) {
                char *s = value_to_string(val_list_get(v, i));
                if (!s) {
                    free(out);
                    return NULL;
                }
                size_t slen = strlen(s);
                memcpy(out + pos, s, slen);
                pos += slen;
                free(s);
                if (i + 1 < n) {
                    out[pos++] = ',';
                }
            }
            out[pos++] = ']';
            return out;
        }

        default:
            return strdup("null");
    }
}
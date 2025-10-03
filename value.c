#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Value *val_null(void) { 
	Value *v=calloc(1,sizeof(Value));
	v->kind=VALUE_NULL;
	return v;
}

Value *val_str(const char *s) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=VALUE_STRING;
	v->str=strdup(s);
	return v; 
}

Value *val_num(double x) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=VALUE_NUMBER;
	v->num=x;
	return v;
}

Value *val_bool(unsigned int b) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=VALUE_BOOL;
	v->num= b ? 1:0;
	return v;
}

Value *val_list(void) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=VALUE_LIST;
	v->items=NULL;
	v->n_items=0;
	return v;
}

void val_list_append(Value *L, Value *it) {
	L->items = realloc(L->items, sizeof(Value*)*(L->n_items+1));L->items[L->n_items++] = it;
}

void val_free(Value *v) {
    if (!v) return;
	
	switch(v->kind){
		case VALUE_STRING:
			if(v->str){
				free(v->str);
				v->str = NULL;
			}
			break;
			
		case VALUE_LIST:
			if(v->items){
				for (size_t i=0;i<v->n_items;i++){
					val_free(v->items[i]);
				}
				free(v->items);
				v->items=NULL;
			}
			break;
			
		default:
			break;
			
	}
    
    free(v);
	v = NULL;
}

Value *val_dup(const Value *src) {
    if (!src) return val_null();
    switch (src->kind) {
        case VALUE_NULL: return val_null();
        case VALUE_STRING:  return val_str(src->str ? src->str : "");
        case VALUE_NUMBER:  return val_num(src->num);
        case VALUE_BOOL: return val_bool(src->num > 0);
        case VALUE_LIST: {
            Value *L = val_list();
            for (size_t i = 0; i < src->n_items; i++) {
                val_list_append(L, val_dup(src->items[i]));
            }
            return L;
        }
    }
    return val_null();
}

static unsigned int val_list_equals(Value *v1,Value *v2){
	unsigned int res = 0;
	if(v1->n_items == v2->n_items){
		for(int i=0;v1->n_items;i++){
			res += val_equals(v1->items[i],v2->items[i]);
		}
		return (res == v1->n_items)?1:0;
	}
	return res;
}

int val_equals(Value *v1,Value *v2){
	if (v1->kind == v2->kind){
		switch(v1->kind){
			case VALUE_BOOL:
			case VALUE_NUMBER: return (v1->num == v2->num);
			case VALUE_STRING: return (!strcmp(v1->str,v2->str));
			case VALUE_LIST: return val_list_equals(v1,v2);
			case VALUE_NULL: return -1;
		}
	}
	return 0;
}

/* Comparação estável de valores (para uso em SORT) */
int val_compare_sort(const Value *a, const Value *b) {
    const Value *va = *(const Value **)a;
    const Value *vb = *(const Value **)b;

    if (!va || !vb) return 0;

    /* Primeiro: ordena por tipo */
    if (va->kind < vb->kind) return -1;
    if (va->kind > vb->kind) return 1;

    /* Dentro do mesmo tipo, compara conteúdo */
    switch (va->kind) {
        case VALUE_NUMBER:
            if (va->num < vb->num) return -1;
            if (va->num > vb->num) return 1;
            return 0;

        case VALUE_STRING:
            if (!va->str && !vb->str) return 0;
            if (!va->str) return -1;
            if (!vb->str) return 1;
            return strcmp(va->str, vb->str);

        case VALUE_BOOL:
            return (va->num > vb->num) - (va->num < vb->num);

        default:
            return 0; /* lists, null, objects */
    }
}

int val_compare(const Value *a, const Value *b) {
    if (!a || !b) return 0;

    if (a->kind != b->kind) {
        // ordenação estável entre tipos diferentes
        return (a->kind < b->kind) ? -1 : 1;
    }

    switch (a->kind) {
        case VALUE_NULL: return 0;
        case VALUE_BOOL:
            return (a->num == b->num) ? 0 : (a->num < b->num ? -1 : 1);
        case VALUE_NUMBER:
            return (a->num == b->num) ? 0 : (a->num < b->num ? -1 : 1);
        case VALUE_STRING:
            return strcmp(a->str ? a->str : NULL, b->str ? b->str : NULL);
        case VALUE_LIST:
            if (a->n_items != b->n_items)
                return (a->n_items < b->n_items) ? -1 : 1;
            for (size_t i = 0; i < a->n_items; i++) {
                int cmp = val_compare(a->items[i], b->items[i]);
                if (cmp != 0) return cmp;
            }
            return 0;
        default:
            return 0;
    }
}


char *value_to_string(const Value *v) {
    if (!v) return strdup("null");
    switch (v->kind) {
        case VALUE_NULL: return strdup("null");
        case VALUE_STRING:  return strdup(v->str ? v->str : "");
        case VALUE_BOOL: return strdup(v->num ? "true" : "false");
        case VALUE_NUMBER: {
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "%g", v->num);
            return strdup(tmp);
        }
        case VALUE_LIST: {
            size_t cap = 128;
            char *out = malloc(cap);
            size_t pos = 0;
            out[pos++] = '[';
            for (size_t i = 0; i < v->n_items; i++) {
                char *it = value_to_string(v->items[i]);
                size_t L = strlen(it);
                if (pos + L + 4 > cap) {
                    cap = (pos + L + 4) * 2;
                    out = realloc(out, cap);
                }
                memcpy(out + pos, it, L);
                pos += L;
                if (i + 1 < v->n_items) out[pos++] = ',';
                free(it);
            }
            out[pos++] = ']';
            out[pos] = 0;
            return out;
        }
    }
    return strdup("null");
}

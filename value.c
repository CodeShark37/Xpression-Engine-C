#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Value *val_null(void) { 
	Value *v=calloc(1,sizeof(Value));
	v->kind=V_NULL;
	return v;
}
Value *val_str(const char *s) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=V_STR;
	v->s=strdup(s);
	return v; 
}
Value *val_num(double x) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=V_NUM;
	v->num=x;
	return v;
}
Value *val_list(void) {
	Value *v=calloc(1,sizeof(Value));
	v->kind=V_LIST;
	v->items=NULL;
	v->n_items=0;
	return v;
}

void val_list_append(Value *L, Value *it) {
	L->items = realloc(L->items, sizeof(Value*)*(L->n_items+1));L->items[L->n_items++] = it;
}

void val_free(Value *v) {
    if (!v) return;
    if (v->kind==V_STR && v->s) {free(v->s); v->s = NULL;}
    if (v->kind==V_LIST && v->items) {
		for (size_t i=0;i<v->n_items;i++){
			val_free(v->items[i]);
		}
		free(v->items);
		v->items=NULL; 
	}
    free(v);
	v = NULL;
}

Value *val_dup(const Value *src) {
    if (!src) return val_null();
    if (src->kind==V_NULL) return val_null();
    if (src->kind==V_STR) return val_str(src->s?src->s:"");
    if (src->kind==V_NUM) return val_num(src->num);
    if (src->kind==V_LIST) { Value *L=val_list(); for (size_t i=0;i<src->n_items;i++) val_list_append(L, val_dup(src->items[i])); return L; }
    return val_null();
}

char *value_to_string(const Value *v) {
    if (!v) return strdup("null");
    if (v->kind==V_NULL) return strdup("null");
    if (v->kind==V_STR) return strdup(v->s?v->s:"");
    if (v->kind==V_NUM) { char tmp[64]; snprintf(tmp,sizeof(tmp),"%g",v->num); return strdup(tmp); }
    if (v->kind==V_LIST) {
        size_t cap=128; char *out=malloc(cap); size_t pos=0; out[pos++]='[';
        for (size_t i=0;i<v->n_items;i++) {
            char *it = value_to_string(v->items[i]); size_t L = strlen(it);
            if (pos + L + 4 > cap) { cap = (pos+L+4)*2; out = realloc(out, cap); }
            memcpy(out+pos, it, L); pos += L;
            if (i+1 < v->n_items) out[pos++] = ',';
            free(it);
        }
        out[pos++] = ']'; out[pos]=0; return out;
    }
    return strdup("null");
}

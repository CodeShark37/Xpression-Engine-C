#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


static FuncEntry *func_registry = NULL;

/* Builtins implementations */

static Value *fn_uppercase(Value *this_val, Value **args, size_t nargs) {
    if (nargs < 1 || !args[0]) return val_str("");
    char *s = value_to_string(args[0]);
    for (size_t i=0;i<strlen(s);i++) s[i] = toupper((unsigned char)s[i]);
    Value *r = val_str(s); free(s); return r;
}
static Value *fn_to_upper(Value *t, Value **a, size_t n) { return fn_uppercase(t,a,n); }

static Value *fn_sum(Value *t, Value **a, size_t n) {
    double acc=0;
    for (size_t i=0;i<n;i++) {
        if (!a[i]) continue;
        if (a[i]->kind==V_NUM) acc += a[i]->num;
        else { char *s = value_to_string(a[i]); acc += atof(s); free(s); }
    }
    return val_num(acc);
}
static Value *fn_max(Value *t, Value **a, size_t n) {
    if (n==0) return val_num(0);
    double m = -INFINITY;
    for (size_t i=0;i<n;i++) {
        double v = (a[i]->kind==V_NUM) ? a[i]->num : atof(value_to_string(a[i]));
        if (v > m) m = v;
    }
    return val_num(m);
}
static Value *fn_min(Value *t, Value **a, size_t n) {
    if (n==0) return val_num(0);
    double m = INFINITY;
    for (size_t i=0;i<n;i++) {
        double v = (a[i]->kind==V_NUM) ? a[i]->num : atof(value_to_string(a[i]));
        if (v < m) m = v;
    }
    return val_num(m);
}
static Value *fn_mixed(Value *t, Value **a, size_t n) {
    size_t cap=128; char *buf=malloc(cap); size_t pos=0; buf[0]=0;
    for (size_t i=0;i<n;i++) {
        char *s = value_to_string(a[i]);
        size_t L = strlen(s);
        if (pos + L + 2 >= cap) { cap = (pos+L+32)*2; buf = realloc(buf, cap); }
        memcpy(buf+pos, s, L); pos += L; buf[pos] = 0;
        if (i+1 < n) buf[pos++]='|';
        free(s);
    }
    buf[pos]=0;
    Value *r = val_str(buf); free(buf); return r;
}

/* register builtins */
void register_builtins_default(void) {
    register_func_cb("UPPERCASE", fn_uppercase);
    register_func_cb("TO_UPPER", fn_to_upper);
    register_func_cb("SUM", fn_sum);
    register_func_cb("MAX", fn_max);
    register_func_cb("MIN", fn_min);
    register_func_cb("MIXED", fn_mixed);
}

void register_func_cb(const char *name, FuncCB cb) {
    FuncEntry *e = malloc(sizeof(FuncEntry)); e->name = strdup(name); e->cb = cb; e->next = func_registry; func_registry = e;
}
FuncCB find_func_cb(const char *name) {
    for (FuncEntry *e = func_registry; e; e = e->next) if (strcmp(e->name, name) == 0) return e->cb;
    return NULL;
}
void free_funcs(void) {
    for (FuncEntry *e = func_registry; e; ) { FuncEntry *nx = e->next; free(e->name); free(e); e = nx; }
    func_registry = NULL;
}


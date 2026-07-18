/**
 * @file functions.c
 * @brief Implementation of functions.
 * @ingroup functions
 * @author Joshua (Codeshark37) - labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#include "xpression.h"
#include "functions.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>




static FuncEntry* func_registry = NULL;

/* ════════════════════════════════════════════════════════
   TRAVESSIA RECURSIVA
   ════════════════════════════════════════════════════════ */

static void traverse_list_internal(Value v, ReducerFn fn, size_t argc, va_list args) {
    if (!fn) return;

    if (val_is_list(v)) {
        size_t n = val_list_len(v);
        for (size_t i = 0; i < n; i++) {
            va_list args_copy;
            va_copy(args_copy, args);
            traverse_list_internal(val_list_get(v, i), fn, argc, args_copy);
            va_end(args_copy);
        }
    } else {
        fn(v, argc, args);
    }
}

static void traverse_list(Value v, ReducerFn fn, size_t argc, ...) {
    if (!fn) return;

    va_list args;
    va_start(args, argc);
    traverse_list_internal(v, fn, argc, args);
    va_end(args);
}

/* ════════════════════════════════════════════════════════
   CONTAINS / COUNT / AVG
   ════════════════════════════════════════════════════════ */

static void contains_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    bool* found = va_arg(args, bool*);
    Value* target = va_arg(args, Value*);

    if (*found) return;
    if (val_equals(v, *target)) *found = true;
}

static Value fn_contains(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    bool found = false;
    traverse_list(args[0], contains_cb, 2, &found, &args[1]);
    return val_bool(found);
}

static void count_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    int* count = va_arg(args, int*);
    Value* target = va_arg(args, Value*);

    if (val_equals(v, *target)) (*count)++;
}

static Value fn_count(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    int count = 0;
    traverse_list(args[0], count_cb, 2, &count, &args[1]);
    return val_num(count);
}

static void avg_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    double* sum = va_arg(args, double*);
    size_t* count = va_arg(args, size_t*);

    if (val_is_number(v)) {
        *sum += unbox_num(v);
        (*count)++;
    }
}

static Value fn_avg(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    double sum = 0.0;
    size_t count = 0;
    traverse_list(args[0], avg_cb, 2, &sum, &count);
    if (count == 0) return val_null();
    return val_num(sum / count);
}

/* ════════════════════════════════════════════════════════
   UPPERCASE / MAP / SORT
   ════════════════════════════════════════════════════════ */

static Value fn_uppercase(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    char* s = value_to_string(args[0]);
    if (!s) return val_null();

    for (size_t i = 0; s[i]; i++) {
        s[i] = (char)toupper((unsigned char)s[i]);
    }

    Value r = val_str(s);
    free(s);
    return r;
}

static Value fn_map(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;

    const char* fn_name = (char*)unbox_ptr(args[1]);
    Func cb = find_function(fn_name);
    if (!cb) return val_null();

    Value result = val_list();

    size_t n = val_list_len(args[0]);
    for (size_t i = 0; i < n; i++) {
        Value single_arg = val_dup(val_list_get(args[0], i));
        Value mapped = cb(val_null(), &single_arg, 1);
        if (!val_list_append(result, mapped)) {
            val_free(&single_arg);
            val_free(&result);
            return val_null();
        }
        val_free(&single_arg);
    }
    return result;
}

static Value fn_sort(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    Value result = val_list();

    size_t n = val_list_len(args[0]);
    for (size_t i = 0; i < n; i++) {
        if (!val_list_append(result, val_dup(val_list_get(args[0], i)))) {
            val_free(&result);
            return val_null();
        }
    }

    size_t len = 0;
    Value* raw = val_list_raw(result, &len);
    if (raw && len > 1) {
        qsort(raw, len, sizeof(Value), val_compare_sort);
    }

    return result;
}

/* ════════════════════════════════════════════════════════
   SUM / MUL / MAX / MIN
   ════════════════════════════════════════════════════════ */

static void sum_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    if (val_is_number(v)) {
        double* total = va_arg(args, double*);
        *total += unbox_num(v);
    }
}

static Value fn_sum(Value this, Value* args, size_t argc) {
    (void)this;
    double total = 0.0;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], sum_cb, 1, &total);
    }
    return val_num(total);
}

static void mul_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    if (val_is_number(v)) {
        double* total = va_arg(args, double*);
        *total *= unbox_num(v);
    }
}

static Value fn_mul(Value this, Value* args, size_t argc) {
    (void)this;
    double total = 1.0;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], mul_cb, 1, &total);
    }
    return val_num(total);
}

static void max_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    if (val_is_number(v)) {
        double* max = va_arg(args, double*);
        double n = unbox_num(v);
        if (n > *max) *max = n;
    }
}

static Value fn_max(Value this, Value* args, size_t argc) {
    (void)this;
    double m = -INFINITY;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], max_cb, 1, &m);
    }
    return val_num(m);
}

static void min_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    if (val_is_number(v)) {
        double* min = va_arg(args, double*);
        double n = unbox_num(v);
        if (n < *min) *min = n;
    }
}

static Value fn_min(Value this, Value* args, size_t argc) {
    (void)this;
    if (!argc) return val_null();
    double m = INFINITY;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], min_cb, 1, &m);
    }
    return val_num(m);
}

/* ════════════════════════════════════════════════════════
   CONCAT / MIXED
   ════════════════════════════════════════════════════════ */

static void concat_len_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    size_t* total = va_arg(args, size_t*);
    if (val_is_string(v)) {
        char* s = (char*)unbox_ptr(v);
        if (s) *total += strlen(s);
    }
}

static void concat_fill_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    char** pos = va_arg(args, char**);
    if (val_is_string(v)) {
        char* s = (char*)unbox_ptr(v);
        if (s) {
            size_t len = strlen(s);
            memcpy(*pos, s, len);
            *pos += len;
        }
    }
}

static Value fn_concat(Value this, Value* args, size_t argc) {
    (void)this;
    size_t total_len = 0;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], concat_len_cb, 1, &total_len);
    }
    char* buf = calloc(total_len + 1, sizeof(char));
    if (!buf) {
        error_add(SERR_MEMORY_ALLOC);
        return val_null();
    }
    char* pos = buf;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], concat_fill_cb, 1, &pos);
    }
    Value r = val_str(buf);
    free(buf);
    return r;
}

static void mixed_len_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    size_t* total = va_arg(args, size_t*);
    char* s = value_to_string(v);
    if (!s) return;
    *total += strlen(s) + 1;
    free(s);
}

static void mixed_fill_cb(Value v, size_t index, va_list args) {
    char** pos = va_arg(args, char**);
    char* s = value_to_string(v);
    if (!s) return;

    if (index > 0) {
        **pos = '|';
        (*pos)++;
    }
    size_t L = strlen(s);
    memcpy(*pos, s, L);
    *pos += L;
    free(s);
}

static Value fn_mixed(Value this, Value* args, size_t argc) {
    (void)this;

    size_t total_len = 0;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], mixed_len_cb, 0, &total_len);
    }

    if (!total_len) return val_str("");

    char* buf = calloc(total_len, 1);
    if (!buf) {
        error_add(SERR_MEMORY_ALLOC);
        return val_null();
    }

    char* pos = buf;
    for (size_t i = 0; i < argc; i++) {
        traverse_list(args[i], mixed_fill_cb, i, &pos);
    }

    if (pos > buf) pos--;

    Value r = val_str(buf);
    free(buf);
    return r;
}

/* ════════════════════════════════════════════════════════
   UNIQUE
   ════════════════════════════════════════════════════════ */

static void unique_cb(Value v, size_t argc, va_list args) {
    (void)argc;
    Value* result = va_arg(args, Value*);
    if (val_list_contains(*result, v)) return;
    val_list_append(*result, val_dup(v));
}

static Value fn_unique(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    Value result = val_list();
    traverse_list(args[0], unique_cb, 1, &result);
    return result;
}

/* ════════════════════════════════════════════════════════
   IF / EQ / NEQ / GT / LT / GTE / LTE
   ════════════════════════════════════════════════════════ */

static Value fn_if(Value this, Value* args, size_t argc) {
    (void)this;
    (void)argc;
    Value cond = args[0];
    bool truthy = false;

    switch (val_kind(cond)) {
        case VAL_BOOL:
            truthy = unbox_bool(cond);
            break;
        case VAL_NUMBER:
            truthy = unbox_num(cond) > 0;
            break;
        case VAL_STRING: {
            char* s = (char*)unbox_ptr(cond);
            truthy = s && *s;
            break;
        }
        case VAL_LIST:
            truthy = val_list_len(cond) > 0;
            break;
        default:
            truthy = false;
    }

    return truthy ? val_dup(args[1]) : val_dup(args[2]);
}

static Value fn_eq(Value this, Value* args, size_t argc) {
    (void)this; (void)argc;
    return val_bool(val_compare(args[0], args[1]) == 0);
}

static Value fn_neq(Value this, Value* args, size_t argc) {
    (void)this; (void)argc;
    return val_bool(val_compare(args[0], args[1]) != 0);
}

static Value fn_gt(Value this, Value* args, size_t argc) {
    (void)this; (void)argc;
    return val_bool(val_compare(args[0], args[1]) > 0);
}

static Value fn_lt(Value this, Value* args, size_t argc) {
    (void)this; (void)argc;
    return val_bool(val_compare(args[0], args[1]) < 0);
}

static Value fn_gte(Value this, Value* args, size_t argc) {
    (void)this; (void)argc;
    return val_bool(val_compare(args[0], args[1]) >= 0);
}

static Value fn_lte(Value this, Value* args, size_t argc) {
    (void)this; (void)argc;
    return val_bool(val_compare(args[0], args[1]) <= 0);
}

inline bool ak_valid(ArgKindMask m){
    return !(m & ~AK_ANY);
}

inline bool ak_has(ArgKindMask got,ArgKindMask need){
    return (got & need);
}

/* ════════════════════════════════════════════════════════
   REGISTO DE FUNÇÕES
   ════════════════════════════════════════════════════════ */

void register_builtins_default(void) {
    register_function("UPPERCASE", fn_uppercase, NULL);
    register_function("MUL", fn_mul, NULL);
    register_function("CONCAT", fn_concat, NULL);
    register_function("COUNT", fn_count, NULL);
    register_function("CONTAINS", fn_contains, NULL);
    register_function("SUM", fn_sum, NULL);
    register_function("MIN", fn_min, NULL);
    register_function("MAX", fn_max, NULL);
    register_function("MIXED", fn_mixed, NULL);
    register_function("IF", fn_if, NULL);
    register_function("MAP", fn_map, NULL);
    register_function("UNIQUE", fn_unique, NULL);
    register_function("AVG", fn_avg, NULL);
    register_function("SORT", fn_sort, NULL);
    register_function("EQ", fn_eq, NULL);
    register_function("NEQ", fn_neq, NULL);
    register_function("GT", fn_gt, NULL);
    register_function("LT", fn_lt, NULL);
    register_function("GTE", fn_gte, NULL);
    register_function("LTE", fn_lte, NULL);
}

void register_builtins_default_ex(void) {
    register_function("CONCAT", fn_concat, function_descriptor_new(1, -1, (AK_STRING | AK_LIST), 0, NULL, AK_STRING));
    register_function("UPPERCASE", fn_uppercase, function_descriptor_new(1, 1, AK_STRING, 0, NULL, AK_STRING));
    register_function("MIXED", fn_mixed, function_descriptor_new(1, -1, AK_ANY, 0, NULL, AK_STRING));

    register_function("SUM", fn_sum, function_descriptor_new(1, -1, (AK_NUMBER | AK_LIST | AK_OBJECT), 0, NULL, AK_NUMBER));
    register_function("MUL", fn_mul, function_descriptor_new(1, -1, (AK_NUMBER | AK_LIST | AK_OBJECT), 0, NULL, AK_NUMBER));
    register_function("MIN", fn_min, function_descriptor_new(1, -1, (AK_NUMBER | AK_OBJECT | AK_LIST), 0, NULL, AK_NUMBER));
    register_function("MAX", fn_max, function_descriptor_new(1, -1, (AK_NUMBER | AK_OBJECT | AK_LIST), 0, NULL, AK_NUMBER));
    register_function("AVG", fn_avg, function_descriptor_new(1, 1, (AK_LIST | AK_OBJECT), 0, NULL, AK_NUMBER));

    register_function("CONTAINS", fn_contains, function_descriptor_new(2, 2, AK_NONE, 2, arg_pos(2, (AK_LIST | AK_OBJECT), AK_ANY), AK_BOOL));
    register_function("COUNT", fn_count, function_descriptor_new(2, 2, AK_NONE, 2, arg_pos(2, AK_LIST, AK_ANY), AK_NUMBER));
    register_function("MAP", fn_map, function_descriptor_new(2, 2, AK_NONE, 2, arg_pos(2, AK_LIST, AK_STRING), AK_LIST));

    register_function("SORT", fn_sort, function_descriptor_new(1, 1, (AK_LIST | AK_OBJECT), 0, NULL, AK_LIST));
    register_function("UNIQUE", fn_unique, function_descriptor_new(1, 1, (AK_LIST | AK_OBJECT), 0, NULL, AK_LIST));

    register_function("IF", fn_if, function_descriptor_new(3, 3, AK_ANY, 0, NULL, AK_BOOL));
    register_function("EQ", fn_eq, function_descriptor_new(2, 2, AK_ANY, 0, NULL, AK_BOOL));
    register_function("NEQ", fn_neq, function_descriptor_new(2, 2, AK_ANY, 0, NULL, AK_BOOL));
    register_function("GT", fn_gt, function_descriptor_new(2, 2, AK_ANY, 0, NULL, AK_BOOL));
    register_function("LT", fn_lt, function_descriptor_new(2, 2, AK_ANY, 0, NULL, AK_BOOL));
    register_function("GTE", fn_gte, function_descriptor_new(2, 2, AK_ANY, 0, NULL, AK_BOOL));
    register_function("LTE", fn_lte, function_descriptor_new(2, 2, AK_ANY, 0, NULL, AK_BOOL));
}

static FuncEntry* func_entry_new(const char* name, Func cb) {
    FuncEntry* e = calloc(1, sizeof(FuncEntry));
    if (!e) {
        error_add(SERR_MEMORY_ALLOC);
        return NULL;
    }
    e->name = name ? strdup(name) : NULL;
    if (name && !e->name) {
        error_add(SERR_MEMORY_ALLOC);
        free(e);
        return NULL;
    }
    e->cb = cb;
    e->desc = NULL;
    e->next = NULL;
    return e;
}

void register_function(const char* name, Func cb, FuncDescriptor* desc) {
    FuncEntry* e = func_entry_new(name, cb);
    if (!e) {
        if (desc) {
            if (desc->pos_arg_kinds) free(desc->pos_arg_kinds);
            free(desc);
        }
        return;
    }
    e->desc = desc;
    e->next = func_registry;
    func_registry = e;
}

Func find_function(const char* name) {
    if (!name) return NULL;
    for (FuncEntry* e = func_registry; e; e = e->next) {
        if (!strcmp(e->name, name)) return e->cb;
    }
    return NULL;
}

FuncEntry* find_function_entry(const char* name) {
    if (!name) return NULL;
    for (FuncEntry* e = func_registry; e; e = e->next) {
        if (!strcmp(e->name, name)) return e;
    }
    return NULL;
}

FuncDescriptor* function_descriptor_new(int8_t min_args, int8_t max_args,
                                         uint8_t allowed_kinds_mask,
                                         size_t pos_count,
                                         ArgKindMask *pos_arg_kinds,
                                         uint8_t ret_kind_mask) {
    FuncDescriptor* desc = calloc(1, sizeof(FuncDescriptor));
    if (!desc) {
        if (pos_arg_kinds) free(pos_arg_kinds);
        error_add(SERR_MEMORY_ALLOC);
        return NULL;
    }
    desc->min_args = min_args;
    desc->max_args = max_args ? max_args : -1;
    desc->pos_count = pos_count;
    desc->allowed_kinds_mask = allowed_kinds_mask;
    desc->pos_arg_kinds = pos_arg_kinds;
    desc->ret_kind_mask = ret_kind_mask;
    return desc;
}

ArgKindMask* arg_pos(size_t pos_count, ...) {
    if (!pos_count) return NULL;
    ArgKindMask* pos = calloc(pos_count, sizeof(ArgKindMask));
    if (!pos) {
        error_add(SERR_MEMORY_ALLOC);
        return NULL;
    }
    va_list args;
    va_start(args, pos_count);
    for (size_t i = 0; i < pos_count; i++) {
        pos[i] = va_arg(args, ArgKindMask);
    }
    va_end(args);
    return pos;
}

void functions_cleanup(void) {
    for (FuncEntry* e = func_registry; e; ) {
        FuncEntry* nx = e->next;
        if (e->name) {
            free(e->name);
            e->name = NULL;
        }
        if (e->desc) {
            if (e->desc->pos_arg_kinds) {
                free(e->desc->pos_arg_kinds);
                e->desc->pos_arg_kinds = NULL;
            }
            free(e->desc);
            e->desc = NULL;
        }
        free(e);
        e = nx;
    }
    func_registry = NULL;
}
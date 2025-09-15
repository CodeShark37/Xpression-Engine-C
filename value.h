#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>

typedef enum { V_NULL, V_STR, V_NUM, V_LIST } ValKind;

typedef struct Value {
    ValKind kind;
    char *s;
    double num;
    struct Value **items;
    size_t n_items;
} Value;

Value *val_null(void);
Value *val_str(const char *s);
Value *val_num(double x);
Value *val_list(void);
void val_list_append(Value *L, Value *it);
void val_free(Value *v);
Value *val_dup(const Value *src);
char *value_to_string(const Value *v);

#endif

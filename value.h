#ifndef VALUE_H
#define VALUE_H

#include <stddef.h>

typedef enum { 
	VALUE_NULL,
	VALUE_STRING,
	VALUE_NUMBER,
	VALUE_BOOL,
	VALUE_LIST
} ValKind;

typedef struct Value {
    ValKind kind;
    char *str;
    double num;
    struct Value **items;
    size_t n_items;
} Value;

Value *val_null(void);
Value *val_str(const char *s);
Value *val_num(double x);
Value *val_bool(unsigned int b);
Value *val_list(void);
int val_equals(Value *v1,Value *v2);
int val_compare(const Value *a, const Value *b);
void val_list_append(Value *L, Value *it);
void val_free(Value *v);
Value *val_dup(const Value *src);
char *value_to_string(const Value *v);

#endif

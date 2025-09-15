#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "value.h"

typedef Value *(*FuncCB)(Value *this_val, Value **args, size_t nargs);
typedef struct FuncEntry { char *name; FuncCB cb; struct FuncEntry *next; } FuncEntry;


void register_func_cb(const char *name, FuncCB cb);
FuncCB find_func_cb(const char *name);
void free_funcs(void);

void register_builtins_default(void);

#endif

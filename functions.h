#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "value.h"

typedef Value *(*Func)(Value *this, Value **args, size_t argc);

typedef struct FuncEntry { 
	char *name;
	Func cb;
	struct FuncEntry *next;
} FuncEntry;


void register_function(const char *name, Func cb);
Func find_function(const char *name);
void free_functions(void);

void register_builtins_default(void);

#endif

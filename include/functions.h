/**
 * @defgroup functions Functions(builtins)
 * @brief functions.
 *
 * This module
 * 
 *
 * @{
 */

/**
 * @file functions.h
 * @brief functions.
 * @author Joshua (Codeshark37) - labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>



typedef Value (*Func)(Value this, Value* args, size_t argc);
typedef void  (*ReducerFn)(Value val, size_t argc, va_list args);


/* Basic argument kind mask used by descriptors */
#define ARG_KIND_LIST \
    X(AK_NULL,     0) \
    X(AK_NUMBER,   1) \
    X(AK_STRING,   2) \
    X(AK_BOOL,     3) \
    X(AK_LIST,     4) \
    X(AK_OBJECT,   5)

typedef enum {
    AK_NONE = 0,
#define X(name, bit) name = 1u << (bit),
    ARG_KIND_LIST
#undef X
    AK_ANY =
#define X(name, bit) | name
   0 ARG_KIND_LIST
#undef X
} ArgKindMask;

enum {
    AK_KIND_COUNT =
#define X(name, bit) +1
    0 ARG_KIND_LIST
#undef X
};

typedef struct FuncDescriptor{
	int8_t min_args;          /* minimum required args */
	int8_t max_args;          /* maximum allowed args (-1 => variadic/unbounded) */
    size_t pos_count;
	uint8_t allowed_kinds_mask; /* AK_* mask indicating allowed types for args (0 => permissive) */
	ArgKindMask* pos_arg_kinds; /* optional per-position kinds (NULL if not used) */
    uint8_t ret_kind_mask; /* return type */
}FuncDescriptor;

typedef struct FuncEntry {
	char* name;
	Func cb;
	struct FuncEntry* next;
	FuncDescriptor* desc; /* Descriptor metadata (optional) */
} FuncEntry;


void register_function(const char* name, Func cb, FuncDescriptor* desc);

Func find_function(const char* name);

FuncEntry* find_function_entry(const char* name);

FuncDescriptor* function_descriptor_new(int8_t min_args, int8_t max_args,
                             uint8_t allowed_kinds_mask,
                             size_t pos_count,
                             ArgKindMask *pos_arg_kinds,
                             uint8_t ret_kind_mask);

ArgKindMask* arg_pos(size_t pos_count,...);

void functions_cleanup(void);

void register_builtins_default(void);
void register_builtins_default_ex(void);

bool ak_valid(ArgKindMask m);

bool ak_has(ArgKindMask got,ArgKindMask need);

#ifdef __cplusplus
}
#endif

#endif

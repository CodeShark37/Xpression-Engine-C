/**
	XPression Engine C (c)2025

	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com

	value.h
*/

#ifndef VALUE_H
#define VALUE_H


#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>

#include "list.h"

/* ════════════════════════════════════════════════════════
   TIPOS
   ════════════════════════════════════════════════════════ */

typedef enum {
    VAL_NULL,
    VAL_BOOL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_LIST,
} ValKind;

/* ════════════════════════════════════════════════════════
   ARQUITECTURA
   ════════════════════════════════════════════════════════ */

#if UINTPTR_MAX == 0xFFFFFFFFu
    #define VALBOX_32
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
    #define VALBOX_64
#else
    #error "value.h: arquitectura não suportada (requer 32 ou 64 bits)"
#endif

/* ════════════════════════════════════════════════════════
   VALUE — representação e tags
   ════════════════════════════════════════════════════════ */

#ifdef VALBOX_32

typedef struct {
    uint32_t payload;
    uint32_t tag;
} Value;

#define _TAG_NULL    0xFFFFFFF0u
#define _TAG_BOOL    0xFFFFFFF1u
#define _TAG_STRING  0xFFFFFFF2u
#define _TAG_LIST    0xFFFFFFF3u

#elif defined(VALBOX_64)

typedef uint64_t Value;

#define _NAN_MASK    ((uint64_t)0x7FF8000000000000u)
#define _TAG_MASK    ((uint64_t)0xFFFF000000000000u)
#define _TAG_NULL    ((uint64_t)0x7FF9000000000000u)
#define _TAG_BOOL    ((uint64_t)0x7FFA000000000000u)
#define _TAG_STRING  ((uint64_t)0x7FFB000000000000u)
#define _TAG_LIST    ((uint64_t)0x7FFC000000000000u)
#define _PTR_MASK    ((uint64_t)0x0000FFFFFFFFFFFFu)
 
#endif

_Static_assert(sizeof(Value) == 8, "Value deve ter 8 bytes em qualquer arquitectura");

/* ════════════════════════════════════════════════════════
   VALLIST — lista intrusiva com freeze implícito
   ════════════════════════════════════════════════════════ */

typedef struct ValNode {
    ListNode node;
    Value    val;
} ValNode;

typedef enum {
    _VL_BUILDING,
    _VL_FROZEN,
} _ValListState;

typedef struct ValList {
    _ValListState _state;
    size_t        len;
    union {
        List   _list;
        Value *_data;
    };
} ValList;

/* ════════════════════════════════════════════════════════
   DETECÇÃO DE TIPO
   ════════════════════════════════════════════════════════ */

bool    val_is_number(Value v);
bool    val_is_null(Value v);
bool    val_is_bool(Value v);
bool    val_is_string(Value v);
bool    val_is_list(Value v);
ValKind val_kind(Value v);

/* ════════════════════════════════════════════════════════
   BOXING / UNBOXING
   ════════════════════════════════════════════════════════ */

Value  val_null(void);
Value  val_bool(bool b);
Value  val_num(double d);
Value  val_box_str(void *p);
Value  val_box_list(void *p);

double unbox_num(Value v);
bool   unbox_bool(Value v);
void  *unbox_ptr(Value v);

/* ════════════════════════════════════════════════════════
   CONSTRUTORES — API pública (alloc)
   ════════════════════════════════════════════════════════ */

Value val_str(const char *s);
Value val_list(void);

/* ════════════════════════════════════════════════════════
   VALLIST — operações
   ════════════════════════════════════════════════════════ */

bool   val_list_append(Value list, Value item);
size_t val_list_len(Value list);
Value  val_list_get(Value list, size_t i);
Value *val_list_raw(Value list, size_t *out_len);
bool   val_list_contains(Value list, Value item);

/* ════════════════════════════════════════════════════════
   OPERAÇÕES GERAIS
   ════════════════════════════════════════════════════════ */

Value  val_dup(Value src);
bool   val_equals(Value a, Value b);
int    val_compare(Value a, Value b);
int    val_compare_sort(const void *a, const void *b);
char  *value_to_string(Value v);
void   val_free(Value *v);

#ifdef __cplusplus
}
#endif

#endif /* VALUE_H */
/**
 * @file context.h
 * @brief Context tree and scoped key-value storage.
 *
 * This module implements a hierarchical context system composed of
 * named nodes (`CtxNode`) with:
 *  - arbitrary key-value properties
 *  - child contexts (tree structure)
 *
 * It is typically used to represent scoped environments such as
 * execution contexts, configuration trees, or semantic scopes.
 *
 * @ingroup context
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */


#ifndef CONTEXT_H
#define CONTEXT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "value.h"

/**
 * @brief Property key-value pair.
 *
 * Represents a single property attached to a context node.
 * Properties are stored as a singly linked list.
 */
typedef struct PropKV {
    char* key;              /**< Property key (heap-allocated string) */
    Value val;             /**< Associated value */
    struct PropKV* next;    /**< Next property in the list */
} PropKV;

/**
 * @brief Context node.
 *
 * A context node may:
 *  - have a name
 *  - store a list of properties
 *  - contain child context nodes
 *
 * Contexts form a tree structure.
 */
typedef struct CtxNode {
    char* name;             /**< Context name (may be NULL) */
    PropKV* props;          /**< Linked list of properties */
    struct CtxNode** children; /**< Dynamic array of child contexts */
    size_t n_children;      /**< Number of children */
} CtxNode;

/**
 * @brief Create a new context node.
 *
 * Allocates and initializes a new context node.
 *
 * @param name Optional context name (copied internally).
 * @return Pointer to the new context node, or NULL on allocation failure.
 *
 * @note On allocation failure, an error is registered via error_add().
 */
CtxNode* ctx_new(const char* name);

/**
 * @brief Add a child context to a parent context.
 *
 * The child context becomes owned by the parent.
 *
 * @param parent Parent context node.
 * @param child  Child context node to attach.
 *
 * @return true on success, false on failure.
 *
 * @note On allocation failure, the child context is freed.
 */
bool ctx_add_child(CtxNode* parent, CtxNode* child);

/**
 * @brief Set a property on a context node.
 *
 * Properties are stored as key-value pairs.
 * If a property with the same key already exists,
 * it is replaced.
 *
 * @param n   Target context node.
 * @param key Property key (copied internally).
 * @param v   Property value (ownership transferred).
 *
 * @return true on success, false on failure.
 */
bool ctx_set_prop(CtxNode* n, const char* key, Value v);

/**
 * @brief Retrieve a property value by key.
 *
 * @param n   Context node.
 * @param key Property key.
 *
 * @return Pointer to the associated Value, or NULL if not found.
 */
Value ctx_get_prop(CtxNode* n, const char* key);

/**
 * @brief Find a child context by name.
 *
 * Performs a linear search among direct children.
 *
 * @param n    Parent context.
 * @param name Child context name.
 *
 * @return Pointer to the child context, or NULL if not found.
 */
CtxNode* ctx_find_child(CtxNode* n, const char* name);

/**
 * @brief Free a context node and all its contents.
 *
 * @param n Pointer to the context node pointer.
 */
void ctx_free(CtxNode** n);

#ifdef __cplusplus
}
#endif

#endif

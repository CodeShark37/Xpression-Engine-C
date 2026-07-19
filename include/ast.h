/**
 * @defgroup ast Abstract Syntax Tree (AST)
 * @brief Data structures and functions for AST node manipulation.
 *
 * This module defines the NodeKind enum, Node struct, and related functions
 * for creating, managing, and freeing nodes of the Abstract Syntax Tree.
 *
 * @{
 */

/**
 * @file ast.h
 * @brief AST node definitions and public API.
 * @author Joshua (Codeshark37) � labprogramming7@gmail.com
 * @version 2.0.0
 * @date 2025
 */


#ifndef AST_H
#define AST_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>


/** @cond INTERNAL */
#define NODE_KIND_LIST \
    X(NODE_UNKNOWN)   \
    X(NODE_OBJECT)    \
    X(NODE_FUNCTION)  \
    X(NODE_LIST)      \
    X(NODE_STRING)    \
    X(NODE_NUMBER)    \
    X(NODE_BOOL)      \
    X(NODE_NULL)      \
    X(NODE_ACCESS)    \
    X(NODE_MULTI_ACCESS)
/** @endcond */

/**
 * @brief Semantic classification of an AST node.
 *
 * Each value represents a logical type of node produced by the parser.
 */

typedef enum {
#define X(name) name,
    NODE_KIND_LIST
#undef X
} NodeKind;

/**
 * @brief Total number of node types defined in NodeKind.
 *
 * Can be used for iteration, validation, or tables indexed by NodeKind.
 */

 enum {
    NODE_KIND_COUNT =
#define X(name) +1
    0 NODE_KIND_LIST
#undef X
};

/**
 * @brief Abstract Syntax Tree (AST) Node.
 *
 * Represents a semantic unit of the analyzed code, and may contain
 * a textual value, position in the source code, and child nodes.
 */
typedef struct Node {
    NodeKind kind; /**< Semantic type of the node */
    char* value; /**< Duplicate textual value; freed by node_free() */
	size_t len; /**< Length of value */
    const char* start; /**< Pointer to the beginning of the token in the external buffer */
    size_t line, col; /**< Position in the source code (base 1) */
    size_t n_children; /**< Number of child nodes */
    struct Node** children; /**< Own array of children; freed by node_free() */
} Node;

/**
 * @brief Creates a new AST node.
 *
 * Duplicates the text value, if provided. The returned node must be
 * freed with node_free().
 *
 * @param k Node type (NodeKind).
 * @param value Text associated with the node (can be NULL).
 * @return Pointer to created node or NULL in case of allocation error.
 */
Node* node_new(NodeKind k, const char* value);
/**
 * @brief Creates a node and sets its position (start, row, column).
 *
 * The start pointer references an external buffer and is not duplicated.
 * The returned node must be freed with node_free().
 *
 * @param k Node type.
 * @param value Node text (can be NULL).
 * @param start Pointer to the beginning of the token in the buffer (not duplicated).
 * @param line Row (base 1).
 * @param col Column (base 1).
 * @return Pointer to Node or NULL in case of error.
 */
Node* node_new_pos(NodeKind k, const char* value,
                   const char* start, size_t line, size_t col);
/**
 * @brief Assigns position (start, row, column) to a node.
 * @param n Node to update (if NULL, do nothing).
 * @param start Pointer to the beginning of the token.
 * @param line Row (base 1).
 * @param col Column (base 1).
 */
void node_set_pos(Node* n, const char* start, size_t line, size_t col);

/**
 * @brief Assigns length of the node.
 * @param n Node to update (if NULL, do nothing).
 * @param len the length in source.
 */
void node_set_len(Node *n, size_t len);

/**
 * @brief Adds a child to the parent node (reallocates the array of children).
 * @param parent Parent node (required).
 * @param child Child to add (required).
 * @return true if successful; false on error (releases the child in case of failure).
 */
bool node_add(Node* parent, Node* child);

/**
 * @brief Adds a child to the parent node (reallocates the array of children).
 * @param n1 first node (required).
 * @param n2 seconde node (required).
 * @return true if are equal; false if not.
 */
bool node_equal(Node* n1, Node* n2);

/**
 * @brief Frees a node and all its resources recursively.
 *
 * @param np Pointer to the pointer of the node to free.
 */
void node_free(Node** n);

/**
 * @brief Gets the textual name of a node type.
 *
 * @param k Node type.
 * @return Constant string representing the type.
 */
const char* nodekind_name(NodeKind k);

/**
 * @brief Checks if a node is of a specific type.
 *
 * @param n Node to check.
 * @param k Expected type.
 * @return true if the node exists and is of the indicated type; false otherwise.
 */
bool check_nodekind(Node* n, NodeKind k);

#ifdef __cplusplus
}
#endif

#endif

/** @} */ // end of ast

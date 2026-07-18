/**
 * @file List.h
 * @author your name (you@domain.com)
 * @brief Intrusive Doubly Linked List
 *        embed a ListNode inside your struct, then use
 *        container_of() to recover the outer struct.
 * @version 2.0.0
 * @date 2026-05-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */


#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;


#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct {
    ListNode head; /* head.next == first element, head.prev == last */
} List;

static inline void list_init(List *l)
{
    l->head.next = &l->head;
    l->head.prev = &l->head;
}

static inline void list_node_init(ListNode *n)
{
    n->next = n;
    n->prev = n;
}

/* ------------------------------------------------------------------ */
/* Queries                                                              */
/* ------------------------------------------------------------------ */

static inline int list_empty(const List *l)
{
    return l->head.next == &l->head;
}

/* ------------------------------------------------------------------ */
/* Internal: insert 'n' between 'prev' and 'next'                      */
/* ------------------------------------------------------------------ */

static inline void _list_insert(ListNode *n, ListNode *prev, ListNode *next)
{
    n->prev    = prev;
    n->next    = next;
    prev->next = n;
    next->prev = n;
}

/* ------------------------------------------------------------------ */
/* Insert                                                               */
/* ------------------------------------------------------------------ */

/* Push to front */
static inline void list_push_front(List *l, ListNode *n)
{
    _list_insert(n, &l->head, l->head.next);
}

/* Push to back */
static inline void list_push_back(List *l, ListNode *n)
{
    _list_insert(n, l->head.prev, &l->head);
}

/* Insert before an existing node */
static inline void list_insert_before(ListNode *existing, ListNode *n)
{
    _list_insert(n, existing->prev, existing);
}

/* Insert after an existing node */
static inline void list_insert_after(ListNode *existing, ListNode *n)
{
    _list_insert(n, existing, existing->next);
}

/* ------------------------------------------------------------------ */
/* Remove                                                               */
/* ------------------------------------------------------------------ */

static inline void list_remove(ListNode *n)
{
    n->prev->next = n->next;
    n->next->prev = n->prev;
    /* poison to catch use-after-remove */
    n->next = n;
    n->prev = n;
}

/* ------------------------------------------------------------------ */
/* Peek                                                                 */
/* ------------------------------------------------------------------ */

/* Returns NULL if empty */
static inline ListNode *list_front(const List *l)
{
    return list_empty(l) ? NULL : l->head.next;
}

static inline ListNode *list_back(const List *l)
{
    return list_empty(l) ? NULL : l->head.prev;
}

/* ------------------------------------------------------------------ */
/* Pop                                                                  */
/* ------------------------------------------------------------------ */

static inline ListNode *list_pop_front(List *l)
{
    ListNode *n = list_front(l);
    if (n) list_remove(n);
    return n;
}

static inline ListNode *list_pop_back(List *l)
{
    ListNode *n = list_back(l);
    if (n) list_remove(n);
    return n;
}

/* ------------------------------------------------------------------ */
/* Iteration                                                            */
/* ------------------------------------------------------------------ */

/*
 * list_for_each(node, list)
 *   Iterates over raw ListNode*. Do NOT remove during iteration.
 */
#define list_for_each(it, l) \
    for (ListNode *it = (l)->head.next; it != &(l)->head; it = it->next)

/*
 * list_for_each_safe(node, tmp, list)
 *   Safe for removal of 'it' during iteration.
 */
#define list_for_each_safe(it, tmp, l) \
    for (ListNode *it = (l)->head.next, *tmp = it->next; \
         it != &(l)->head; \
         it = tmp, tmp = it->next)

/*
 * list_for_each_entry(ptr, list, type, member)
 *   Iterates directly as the containing struct.
 */
#define list_for_each_entry(ptr, l, type, member) \
    for (type *ptr = container_of((l)->head.next, type, member); \
         &ptr->member != &(l)->head; \
         ptr = container_of(ptr->member.next, type, member))

/**
 * @brief 
 * 
 */
#define list_for_each_entry_safe(ptr,tmp,l,type,member) \
    for (type *ptr = container_of((l)->head.next,type,member), \
              *tmp = container_of(ptr->member.next,type,member); \
         &ptr->member != &(l)->head; \
         ptr = tmp, \
         tmp = container_of(tmp->member.next,type,member))
         
#ifdef __cplusplus
}
#endif

#endif /* LIST_H */

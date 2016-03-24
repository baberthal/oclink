#ifndef OCL_UTIL_H
#define OCL_UTIL_H 1

#include <stdlib.h>
#include "clink_export.h"

/**
 *  @file
 *  @brief Various Utilities, including data structures and algorithms
 */

/// @struct OCL_ListNode
typedef struct OCL_ListNode OCL_ListNode;

/**
 *  @brief The basic structure of a node in the list
 */
struct OCL_ListNode {
    OCL_ListNode *next;  ///< A pointer to the next list node
    OCL_ListNode *prev;  ///< A pointer to the previous list node
    void *value;         ///< A pointer to the node's associated value
};

/**
 *  @brief The basic structure of a list
 */
typedef struct OCL_List {
    uint32_t count;       ///< The number of nodes in this list
    OCL_ListNode *first;  ///< A pointer to the first node in the list
    OCL_ListNode *last;   ///< A pointer to the last node in the list
} OCL_List;

/**
 *  @brief Create an empty list
 *
 *  @return The new list
 */
CLINK_EXPORT OCL_List *ocl_list_create(void);

/**
 *  @brief Destroy a list
 *
 *  @param list The list to destroy
 */
CLINK_EXPORT void ocl_list_destroy(OCL_List *list);

/**
 *  @brief Clean all the elements from a list
 *
 *  @param list The list to clear
 */
CLINK_EXPORT void ocl_list_clear(OCL_List *list);

/**
 *  @brief Clear all elements and destroy a list
 *
 *  @param list The list to clear and destroy
 */
CLINK_EXPORT void ocl_list_clear_destroy(OCL_List *list);

/**
 *  @brief Convenience macro to get the count of a list
 *
 *  @param A The list
 *
 *  @return The number of elements in the list
 */
#define ocl_list_count(A) ((A)->count)

/**
 *  @brief Get the value of the first element in the list
 *
 *  @param A The list
 *
 *  @return The value of the first element, or NULL
 */
#define ocl_list_first(A) ((A)->first != NULL ? (A)->first->value : NULL)

/**
 *  @brief Get the last element from a list
 *
 *  @param A The list
 *
 *  @return The value of the last element, or NULL
 */
#define ocl_list_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

/**
 *  @brief Push an element onto the end of a list
 *
 *  @param list  The list
 *  @param value The value to push onto the list
 */
CLINK_EXPORT void ocl_list_push(OCL_List *list, void *value);

/**
 *  @brief Pop the last element off of the list
 *
 *  @param list The list
 *
 *  @return The last element
 */
CLINK_EXPORT void *ocl_list_pop(OCL_List *list);

/**
 *  @brief Unshift (push an element to the front of) a list
 *
 *  @param list  The list
 *  @param value The value to unshift onto the list
 */
CLINK_EXPORT void ocl_list_unshift(OCL_List *list, void *value);

/**
 *  @brief Shift an element off of the front of a list
 *
 *  @param list The list
 *
 *  @return The first element. The list no longer contains that element
 */
CLINK_EXPORT void *ocl_list_shift(OCL_List *list);

/**
 *  @brief Remove an element from a list
 *
 *  @param list The list
 *  @param node The node to remove from the list
 *
 *  TODO: Fix return value maybe?
 *  @return The value of the node
 */
CLINK_EXPORT void *ocl_list_remove(OCL_List *list, OCL_ListNode *node);

/**
 *  @brief Convenience macro to perform foreach functions on a list
 *
 *  @param L The list
 *  @param S the starting iteration point. must be a member variable of \ref
 *           OCL_List
 *  @param M the step of iteration. must be a member variable of \ref OCL_List
 *  @param V the name of the variable to use for each step of iteration. this
 *           variable is available within the block of the macro.
 */
#define JLIST_FOREACH(L, S, M, V) \
    OCL_ListNode *_node = NULL;   \
    OCL_ListNode *V = NULL;       \
    for (V = _node = L->S; _node != NULL; V = _node = _node->M)

/**
 *  @brief A convenience typedef for a compare function to be passed to sorting
 *   functions
 */
typedef int (*ocl_list_compare_cb)(const void *a, const void *b);

/**
 *  @brief Bubble sort, using the comparison function passed in
 *
 *  @param list The list to sort
 *  @param cb   The comparison callback
 */
CLINK_EXPORT int ocl_list_bubble_sort(OCL_List *list, ocl_list_compare_cb cmp);

CLINK_EXPORT OCL_List *ocl_list_merge_sort(OCL_List *list,
                                           ocl_list_compare_cb cmp);

#endif /* ifndef OCL_UTIL_H */

/* vim: set ts=8 sw=4 tw=0 ft=cpp et :*/

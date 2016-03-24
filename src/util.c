#include "util.h"
#include <stdbool.h>
#include "jml_debug.h"

OCL_List *ocl_list_create(void)
{
    return calloc(1, sizeof(OCL_List));
}

void ocl_list_destroy(OCL_List *list)
{
    JLIST_FOREACH(list, first, next, cur)
    {
        if (cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
}

void ocl_list_clear(OCL_List *list)
{
    JLIST_FOREACH(list, first, next, cur)
    {
        free(cur->value);
    }
}

void ocl_list_clear_destroy(OCL_List *list)
{
    JLIST_FOREACH(list, first, next, cur)
    {
        if (cur->value) {
            free(cur->value);
        }

        if (cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
}

void ocl_list_push(OCL_List *list, void *value)
{
    OCL_ListNode *node = calloc(1, sizeof(OCL_ListNode));
    check_mem_jmp(node);

    node->value = value;

    if (list->last == NULL) {
        list->first = node;
        list->last = node;
    }
    else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

error:
    return;
}

void *ocl_list_pop(OCL_List *list)
{
    OCL_ListNode *node = list->last;
    return node != NULL ? ocl_list_remove(list, node) : NULL;
}

void ocl_list_unshift(OCL_List *list, void *value)
{
    OCL_ListNode *node = calloc(1, sizeof(OCL_ListNode));
    check_mem_jmp(node);

    node->value = value;

    if (list->first == NULL) {
        list->first = node;
        list->last = node;
    }
    else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;

error:
    return;
}

void *ocl_list_shift(OCL_List *list)
{
    OCL_ListNode *node = list->first;
    return node != NULL ? ocl_list_remove(list, node) : NULL;
}

void *ocl_list_remove(OCL_List *list, OCL_ListNode *node)
{
    void *result = NULL;

    cl_check_jmp(list->first && list->last, "List is empty!");
    cl_check_jmp(node, "node can't be NULL");

    if (node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    }
    else if (node == list->first) {
        list->first = node->next;
        cl_check_jmp(list->first != NULL,
                     "Invalid list! Somehow, the first element was NULL.");
        list->first->prev = NULL;
    }
    else if (node == list->last) {
        list->last = node->prev;
        cl_check_jmp(list->last != NULL,
                     "Invalid list! Somehow, the next element was NULL.");
        list->last->next = NULL;
    }
    else {
        OCL_ListNode *after = node->next;
        OCL_ListNode *before = node->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    result = node->value;
    free(node);

error:
    return result;
}

static inline void ocl_list_node_swap(OCL_ListNode *a, OCL_ListNode *b)
{
    void *tmp = a->value;
    a->value = b->value;
    b->value = tmp;
}

int ocl_list_bubble_sort(OCL_List *list, ocl_list_compare_cb cmp)
{
    int sorted = 1;
    if (ocl_list_count(list) <= 1) {
        return 0;
    }

    do {
        sorted = 1;
        JLIST_FOREACH(list, first, next, cur)
        {
            if (cur->next) {
                if (cmp(cur->value, cur->next->value) > 0) {
                    ocl_list_node_swap(cur, cur->next);
                    sorted = 0;
                }
            }
        }
    } while (!sorted);

    return 0;
}

static inline OCL_List *ocl_list_merge(OCL_List *left, OCL_List *right,
                                       ocl_list_compare_cb cmp)
{
    OCL_List *result = ocl_list_create();
    void *val = NULL;

    while (ocl_list_count(left) > 0 || ocl_list_count(right) > 0) {
        if (ocl_list_count(left) > 0 && ocl_list_count(right) > 0) {
            if (cmp(ocl_list_first(left), ocl_list_first(right)) <= 0) {
                val = ocl_list_shift(left);
            }
            else {
                val = ocl_list_shift(right);
            }

            ocl_list_push(result, val);
        }
        else if (ocl_list_count(left) > 0) {
            val = ocl_list_shift(left);
            ocl_list_push(result, val);
        }
        else if (ocl_list_count(right) > 0) {
            val = ocl_list_shift(right);
            ocl_list_push(result, val);
        }
    }

    return result;
}

OCL_List *ocl_list_merge_sort(OCL_List *list, ocl_list_compare_cb cmp)
{
    if (ocl_list_count(list) <= 1) {
        return list;
    }

    OCL_List *left = ocl_list_create();
    OCL_List *right = ocl_list_create();

    int mid = ocl_list_count(list) / 2;

    JLIST_FOREACH(list, first, next, cur)
    {
        if (mid > 0) {
            ocl_list_push(left, cur->value);
        }
        else {
            ocl_list_push(right, cur->value);
        }

        mid--;
    }

    OCL_List *sort_left = ocl_list_merge_sort(left, cmp);
    OCL_List *sort_right = ocl_list_merge_sort(right, cmp);

    if (sort_left != left) {
        ocl_list_destroy(left);
    }

    if (sort_right != right) {
        ocl_list_destroy(right);
    }

    return ocl_list_merge(sort_left, sort_right, cmp);
}

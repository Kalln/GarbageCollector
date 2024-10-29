
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "linked_list.h"

typedef bool (*ioopm_list_predicate)(size_t index, elem_t value, void *extra);
typedef void (*ioopm_list_apply_func)(size_t index, elem_t *value, void *extra);
typedef bool (*ioopm_eq_func)(elem_t a, elem_t b);

// Private declarations
/**
 * @brief Create a node object
 *
 * @return ioopm_node_t*
 */
static ioopm_node_t *create_node(void);
/**
 * @brief Check if index is out of bounds of linked list, i.e outside [0, n-1]
 *
 * @param list linked list
 * @param index index
 * @return true if index is out of bounds
 * @return false if index is not out of bounds
 */
static bool index_out_of_bounds(ioopm_list_t *list, size_t index);
/**
 * @brief Adds first element to a empty linked list
 *
 * @param list linked list
 * @param new_element element to be added first to list
 */
static void add_first_element(ioopm_list_t *list, ioopm_node_t *new_element);
/**
 * @brief Returns the previous node for a index in a linked list.
 *        NULL means index is 0, no previous node.
 *        To be used by IOOPM Linked list
 *
 *@param list - linked list
 *@param index - index from where prev node is calculated
 *@return The previous node for the index
 */
static ioopm_node_t *find_prev_node_for_index(ioopm_list_t *list, size_t index);
/**
 * @brief Deletes the node by free().
 *        To be used by IOOPM Linked list
 *@param node - node to be freed up from heap
 */
static void destroy_node(ioopm_node_t *node);

/**
 * @brief Swaps values of two nodes
 * @param node1 node 1
 * @param node2 node 2
 * @return
 */
static ioopm_node_t *swap(ioopm_node_t *node1, ioopm_node_t *node2);

// Public functions

ioopm_list_t *ioopm_linkedlist_create(ioopm_eq_func eq_fun)
{
    ioopm_list_t *list = allocate(sizeof(ioopm_list_t), NULL);
    retain(list);
    list->size = 0;
    list->eq_fun = eq_fun;
    return list;
}

void ioopm_linkedlist_destroy(ioopm_list_t *list)
{
    ioopm_linkedlist_clear(list);
    release(list);
    list = NULL;
}

void ioopm_linkedlist_append(ioopm_list_t *list, elem_t value)
{
    // Create new element
    ioopm_node_t *new_element = create_node();
    retain(new_element);
    // Assign value to new_element
    new_element->value = value;

    if (ioopm_linkedlist_isempty(list))
    {
        add_first_element(list, new_element);
    }
    else
    {
        // Switch pointers around: (old last element -> new element) AND (last -> new element)
        ioopm_node_t *prev_element = list->last;
        retain(prev_element);
        prev_element->tail = new_element;
        list->last = new_element;
        list->size += 1;
        release(prev_element);
    }
    release(new_element);
}

void ioopm_linkedlist_prepend(ioopm_list_t *list, elem_t value)
{
    // Create new element
    ioopm_node_t *new_element = create_node();
    retain(new_element);
    new_element->value = value;

    if (ioopm_linkedlist_isempty(list))
    {
        add_first_element(list, new_element);
        release(new_element);
    }
    else
    {
        // Switch pointers around: first -> new_element -> old list_first
        new_element->tail = list->first;
        list->first = new_element;
        release(new_element);
        list->size += 1;
    }
}

void ioopm_linkedlist_insert(ioopm_list_t *list, size_t index, elem_t value)
{
    ioopm_node_t *prev_node = find_prev_node_for_index(list, index);
    retain(prev_node);

    ioopm_node_t *new_node = create_node();
    retain(new_node);
    new_node->value = value;

    // If there is a previous node aka not insert in index 0
    if (prev_node != NULL)
    {
        ioopm_node_t *current_node = prev_node->tail;
        retain(current_node);
        new_node->tail = current_node;
        // retain(new_node->tail);
        release(current_node);
        prev_node->tail = new_node;
        // retain(prev_node->tail);

        // if new_node is last
        if (index == list->size)
        {
            list->last = new_node;
        }
    }

    // If there is no previous node
    if (prev_node == NULL)
    {
        // If there is another node that needs to be pointed at
        if (list->size >= 1)
        {
            new_node->tail = list->first;
        }
        else
        {
            list->last = new_node;
        }

        list->first = new_node;
    }

    // Node is inserted, so the size of the list is increased
    list->size++;
    release(prev_node);
    release(new_node);
}

elem_t ioopm_linkedlist_remove(ioopm_list_t *list, size_t index, bool *error)
{
    // Return empty element if index is out of range, set error to true
    if (index_out_of_bounds(list, index))
    {
        *error = true;
        return (elem_t){.i = 0};
    }

    // No error if element is in bounds
    *error = false;

    elem_t removed_value;

    // If index is 0, special case as we cant find a previous node
    if (index < 1)
    {
        ioopm_node_t *current_node = list->first;
        retain(current_node);
        removed_value = current_node->value;

        // The only node is removed, assign pointers to NULL
        if (list->size == 1)
        {
            list->first = NULL;
            list->last = NULL;
            destroy_node(current_node);
            release(current_node);
        }
        else if (list->size >= 2)
        {
            list->first = list->first->tail;
            retain(list->first);
            destroy_node(current_node);
            release(current_node);
        }
    }
    else
    {
        // If index is bigger than 1, current node has a previous node
        ioopm_node_t *prev_node = find_prev_node_for_index(list, index);
        retain(prev_node);
        ioopm_node_t *current_node = prev_node->tail;
        retain(current_node);
        removed_value = current_node->value;
        release(current_node);

        // The last node is being removed ()
        if (index == list->size - 1)
        {
            destroy_node(current_node);
            prev_node->tail = NULL;
            list->last = prev_node;
            release(prev_node);
        }
        else
        {
            prev_node->tail = current_node->tail;
            retain(prev_node->tail);
            destroy_node(current_node);
            release(prev_node);
        }
    }

    list->size -= 1;

    return removed_value;
}

elem_t ioopm_linkedlist_get(ioopm_list_t *list, size_t index, bool *error)
{
    // return empty element if index is out of range, set error to true
    if (index_out_of_bounds(list, index))
    {
        *error = true;
        return (elem_t){.i = 0};
    }

    // No error if element is in bounds
    *error = false;

    ioopm_node_t *current_entry = list->first;
    retain(current_entry);

    for (int i = 0; i < index; i += 1)
    {
        release(current_entry);
        current_entry = current_entry->tail;
        retain(current_entry);
    }
    release(current_entry);
    return current_entry->value;
}

bool ioopm_linkedlist_contains(ioopm_list_t *list, elem_t value)
{
    size_t list_len = ioopm_linkedlist_size(list);
    ioopm_node_t *current = list->first;

    // Iterate over list and use equality function to check if equal
    for (int i = 0; i < list_len; i += 1)
    {
        retain(current);
        if (list->eq_fun(current->value, value))
        {
            release(current);
            return true;
        }
        else
        {
            release(current);
            current = current->tail;
        }
    }
    // If loop iterate over all list and value isnt in the list
    return false;
}

size_t ioopm_linkedlist_size(ioopm_list_t *list)
{
    // Size is a field in the sentinel
    return list->size;
}

bool ioopm_linkedlist_isempty(ioopm_list_t *list)
{
    // List is empty if size == 0
    return list->size == 0 ? true : false;
}

void ioopm_linkedlist_clear(ioopm_list_t *list)
{
    ioopm_node_t *current_node = list->first;
    retain(current_node);

    while (current_node != NULL)
    {

        ioopm_node_t *next_node = current_node->tail;
        retain(next_node);

        release(current_node);
        release(current_node); // Instead of free

        current_node = next_node;
        retain(current_node);

        release(next_node);
    }

    list->first = NULL;
    list->last = NULL;
    list->size = 0;
}

bool ioopm_linkedlist_all(ioopm_list_t *list, ioopm_list_predicate pred, void *extra)
{
    size_t list_len = ioopm_linkedlist_size(list);
    ioopm_node_t *current = list->first;

    // If list is empty list
    if (list_len <= 0)
    {
        return false;
    }
    // Iterate over all elements in list
    for (int i = 0; i < list_len; i += 1)
    {
        retain(current);
        // If predicate is not met we return false.
        if (!(pred(i, current->value, extra)))
        {
            release(current);
            return false;
        }
        release(current);

        current = current->tail;
    }
    // If predicate is  met for all
    return true;
}

bool ioopm_linkedlist_any(ioopm_list_t *list, ioopm_list_predicate pred, void *extra)
{
    size_t list_len = ioopm_linkedlist_size(list);
    ioopm_node_t *current = list->first;

    // Iterate over all elements in list
    for (int i = 0; i < list_len; i += 1)
    {
        retain(current);
        // If predicate is met we return true.
        if (pred(i, current->value, extra))
        {
            release(current);
            return true;
        }
        release(current);

        current = current->tail;
    }
    // If predicate is never met we return false
    return false;
}

void ioopm_linkedlist_apply_all(ioopm_list_t *list, ioopm_list_apply_func fun, void *extra)
{
    size_t list_len = ioopm_linkedlist_size(list);
    ioopm_node_t *current = list->first;

    for (int i = 0; i < list_len; i++)
    {
        retain(current);
        fun(i, &current->value, extra);
        release(current);
        current = current->tail;
    }
}

void ioopm_linkedlist_bubblesort(ioopm_list_t *list, int (*compare_fun)(elem_t arg1, elem_t arg2))
{

    size_t listlen = ioopm_linkedlist_size(list);
    int i, j;

    for (i = 0; i <= listlen; i++)
    {
        ioopm_node_t *h = list->first;
        bool swapped = false;

        for (j = 0; j < listlen - i - 1; j++)
        {
            retain(h);
            ioopm_node_t *node1 = h;
            retain(node1);
            ioopm_node_t *node2 = node1->tail;
            retain(node2);

            if (compare_fun(node1->value, node2->value))
            {
                swap(node1, node2);

                swapped = true;
            }
            release(h);
            h = h->tail;
            release(node1);
            release(node2);
        }
        if (!swapped)
        {
            return;
        }
    }
}

// Private functions

static ioopm_node_t *create_node(void)
{
    ioopm_node_t *new_node = allocate(sizeof(ioopm_node_t), NULL);
    retain(new_node);
    new_node->tail = NULL;
    return new_node;
}

static bool index_out_of_bounds(ioopm_list_t *list, size_t index)
{
    return (ioopm_linkedlist_size(list) <= index || index < 0);
}

static void add_first_element(ioopm_list_t *list, ioopm_node_t *new_element)
{
    list->first = new_element;
    list->last = new_element;
    list->size += 1;
}

static ioopm_node_t *find_prev_node_for_index(ioopm_list_t *list, size_t index)
{
    if (index == 0)
    {
        return NULL;
    }
    else
    {
        // Since index != 0, we know that there is at least 1 node, and can access its tail
        ioopm_node_t *prev_node = list->first;
        ioopm_node_t *current_node = prev_node->tail;
        // Loop if index > 1
        for (int i = 0; i < index - 1; ++i)
        {
            prev_node = current_node;
            current_node = prev_node->tail;
        }

        return prev_node;
    }
}

static void destroy_node(ioopm_node_t *node)
{
    release(node);
}

static ioopm_node_t *swap(ioopm_node_t *node1, ioopm_node_t *node2)
{
    elem_t tmp = node2->value;
    node2->value = node1->value;
    node1->value = tmp;
    return node2;
}

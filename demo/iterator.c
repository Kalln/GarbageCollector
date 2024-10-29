
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iterator.h"
#include "../src/refmem.h"

// Public functions

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
    ioopm_list_iterator_t *result = allocate(sizeof(ioopm_list_t), NULL);
    retain(result);

    // Assign current and list to result
    result->current = list->first;
    retain(result->current);
    result->previous = NULL;
    result->list = list;
    retain(result->list);

    return result;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return iter->current == NULL ? false : iter->current->tail != NULL;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter, bool *error)
{
    if (ioopm_iterator_has_next(iter))
    {
        // Update current and previous
        *error = false;
        release(iter->previous);
        iter->previous = iter->current;
        retain(iter->previous);
        release(iter->current);
        iter->current = iter->current->tail;
        retain(iter->current);
        return iter->current->value;
    }
    else
    {
        *error = true;
        return (elem_t){.i = 0};
    }
}

// Optional
elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter)
{
    ioopm_node_t *to_remove = iter->current;
    elem_t result = to_remove->value;

    // if last element in list
    if (!ioopm_iterator_has_next(iter))
    {
        release(iter->current);
        iter->current = iter->previous;
        retain(iter->current);
        iter->list->last = iter->current;
        retain(iter->list->last);
    }

    // Set current to next element
    release(iter->current);
    iter->current = to_remove->tail;
    retain(iter->current);

    // First element in list
    if (iter->previous == NULL)
    {
        release(iter->list->first);
        iter->list->first = iter->current;
        retain(iter->list->first);
    }
    else
    {
        // Neither first or last element
        release(iter->previous->tail);
        iter->previous->tail = iter->current;
        retain(iter->previous->tail);
    }

    // Update size
    iter->list->size -= 1;

    // Free element
    release(to_remove);

    return result;
}

// Optional
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element)
{
    ioopm_node_t *new_node = allocate(sizeof(ioopm_node_t), NULL);
    retain(new_node); // 1
    new_node->value = element;

    // If there is a previous node
    if (iter->previous != NULL)
    {
        release(new_node->tail);
        new_node->tail = iter->current;
        retain(new_node->tail);
        release(iter->previous->tail);
        iter->previous->tail = new_node;
        retain(iter->previous->tail);
    }

    // If new element is first (no previous node)
    if (iter->previous == NULL)
    {
        // If there is another node that needs to be pointed at
        if (iter->current != NULL)
        {
            release(new_node->tail);
            new_node->tail = iter->list->first;
            retain(new_node->tail);
        }
        else
        {
            release(iter->list->last);
            iter->list->last = new_node;
            
        }
        // This is done after the if-statement so we don't lose the pointer to the first node
        release(iter->list->first);
        iter->list->first = new_node;

    }

    // Node is inserted, so the size of the list is increased
    iter->list->size++;
    release(iter->current);
    iter->current = new_node;
    retain(iter->current);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    release(iter->current);
    iter->current = iter->list->first;
    retain(iter->current);
}

elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->current->value;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
    release(iter);
    iter = NULL;
}

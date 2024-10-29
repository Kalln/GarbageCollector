#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct iter ioopm_list_iterator_t;

struct iter {
    ioopm_node_t *current;
    ioopm_node_t *previous;
    ioopm_list_t *list;
};

/// @brief Create an iterator for a given list. Current value is first value in list
///        Previous value is NULL until we step forward in our iterator.
/// @param list the list to be iterated over
/// @return an iteration positioned at the start of list
ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list);

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is at least one more element
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step.
/// If iterator has no next element, an element with int 0 is returned, and the error argument is set to true.
/// @param iter the iterator
/// @return the next element
elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter, bool *error);

/// NOTE: REMOVE IS OPTIONAL TO IMPLEMENT
/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element
elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter);

/// NOTE: INSERT IS OPTIONAL TO IMPLEMENT
/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element
elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);

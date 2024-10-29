#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"
#include "../src/refmem.h"

typedef struct list ioopm_list_t;
typedef struct node ioopm_node_t;

/**
 * @brief Creates a linked list of elem_t elements (union)
 * The sentinel of the list contains metadata as size, first and last element and
 * a function to check elements for equality.
 *
 * @param eq_fun user provided function to check for equality between elements
 * @return an empty linked list
 */
ioopm_list_t *ioopm_linkedlist_create(ioopm_eq_func eq_fun);

/**
 * @brief Destroys a linked list
 *
 * @param list list that will be destroyed
 * @return void*
 */
void ioopm_linkedlist_destroy(ioopm_list_t *list);

/**
 * @brief Adds a value to the end of the list.
 *
 * @param list List that item will be added to
 * @param value value to add
 * @return void*
 */
void ioopm_linkedlist_append(ioopm_list_t *list, elem_t value);

/**
 * @brief Adds a value to the beginning of the list
 *
 * @param list List that item will be added to
 * @param value value to add
 * @return void*
 */
void ioopm_linkedlist_prepend(ioopm_list_t *list, elem_t value);

/**
 * @brief Inserts a value in a list at given index.
 * Valid values are between 0 and n for a list with n elements.
 * 0 will insert at the beginning of the list.
 * n will insert at the end of the list
 *
 * @param list List that item will be added to
 * @param index Index to position in list
 * @param value Value to insert
 * @return void*
 */
void ioopm_linkedlist_insert(ioopm_list_t *list, size_t index, elem_t value);

/** @brief Removes an element from a linked list in O(n) time.
 * The valid values of index are [0,n-1] for a list of n elements,
 * where 0 is the first element and n-1 is the last element.
 * If index is out of range, an element with int 0 is returned, and the error argument is set to true.
 *
 * @param list the linked list
 * @param index the position in the list
 * @param error pointer to error detection
 * @return the value removed
 */
elem_t ioopm_linkedlist_remove(ioopm_list_t *list, size_t index, bool *error);

/** @brief Retrieve an element from a linked list in O(n) time.
 * The valid values of index are [0,n-1] for a list of n elements,
 * where 0 means the first element and n-1 means the last element.
 * If index is out of range, returns an element with int 0, and sets errordetection to true
 *
 * @param list the linked list that will be extended
 * @param index the position in the list
 * @param error pointer to error detection
 * @return the value at the given position
 */
elem_t ioopm_linkedlist_get(ioopm_list_t *list, size_t index, bool *error);

/**
 * @brief Test if elements is in list
 *
 * @param list linked list
 * @param value value to look for
 * @return true if value is found
 * @return false if value is not found
 */
bool ioopm_linkedlist_contains(ioopm_list_t *list, elem_t value);

/**
 * @brief Gives us the size of the list in O(1) time.
 *
 * @param list Linked list
 * @return the size of the list
 */
size_t ioopm_linkedlist_size(ioopm_list_t *list);

/**
 * @brief Tests if the list is empty
 *
 * @param list linked list
 * @return true if list is empty
 * @return false if list not empty
 */
bool ioopm_linkedlist_isempty(ioopm_list_t *list);

/**
 * @brief Remove all elements in a list
 *
 * @param list list to be cleared
 * @return true
 * @return false
 */
void ioopm_linkedlist_clear(ioopm_list_t *list);

/**
 * @brief Test if a supplied predicate is met in all elements of the list
 * The function returns directly as any false predicate is met, or if list is an empty list.
 *
 * @param list linked list
 * @param pred ioopm_int_predicate predicate function to be tested
 * @param extra additional argument (possibly NULL) that will be casted to all calls for pred
 * @return true if predicate holds for all elements
 * @return false as soon as any element dont hold the predicate
 */
bool ioopm_linkedlist_all(ioopm_list_t *list, ioopm_list_predicate pred, void *extra);

/**
 * @brief Test if a supplied predicate is met in any element of the list
 * The function returns directly as any true predicate is met.
 *
 * @param list linked list
 * @param pred ioopm_int_predicate predicate function to be tested
 * @param extra additional argument (possibly NULL) that will be casted to all calls for pred
 * @return true if predicate holds for any element
 * @return false if all predicate dont hold for all elements
 */
bool ioopm_linkedlist_any(ioopm_list_t *list, ioopm_list_predicate pred, void *extra);

/**
 * @brief Apply a supplied function of ioopm_int_apply_func type to all elements in list
 *
 * @param list linked list
 * @param fun ioopm_int_apply_func function to apply to all elements in list
 * @param extra extra argument to pass into applied function
 * @return
 */
void ioopm_linkedlist_apply_all(ioopm_list_t *list, ioopm_list_apply_func fun, void *extra);

/**
 * @brief Bubble sort of a IOOPM Linked list
 * @param list list to sort
 * @param compare_fun comparefunction, arg1 < arg2
 */
void ioopm_linkedlist_bubblesort(ioopm_list_t *list, int (*compare_fun)(elem_t arg1, elem_t arg2));

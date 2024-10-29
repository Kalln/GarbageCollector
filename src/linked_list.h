#pragma once

#include <stdbool.h>
#include <stdlib.h>

/**
 * @file linked_list.h
 * @author Emmeli och Ella
 * @date 12 Oct 2023
 * @brief Simple sorted linked list that contain links with an ref_elem_t value 
 * and a pointer to the next value in the list. 
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gb54499f3b7b264e3af3b68c756090f52
 */


typedef struct list ref_list_t;

typedef union ref_elem ref_elem_t;
union ref_elem
{
    int i;
    unsigned int u;
    bool b;
    float f;
    void *p;
};

typedef bool(*ref_eq_function)(ref_elem_t a, ref_elem_t b);

typedef bool(*ref_elem_predicate)(ref_list_t *list, ref_elem_t value,
                                    void *extra);

typedef void(*ref_apply_elem_function)(ref_elem_t *value, void *extra);


/// @brief Creates a new empty list
/// @param value_eq_fun function used to compare the list elements
/// @return an empty linked list
ref_list_t *ref_linked_list_create(ref_eq_function value_eq_fun);

/// @brief Tear down the linked list and return all its memory
/// @param list the list to be destroyed
void ref_linked_list_destroy(ref_list_t *list);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ref_linked_list_size(ref_list_t *list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ref_linked_list_append(ref_list_t *list, ref_elem_t value);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param value the value to be prepended
void ref_linked_list_prepend(ref_list_t *list, ref_elem_t value);

/// @brief Insert an element into a linked list in O(n) time. The valid values
/// of index are [0,n] for a list of n elements, where 0 means before the first
/// element and n means after the last element. If index is out of bound, it
/// will be adjusted to a valid index (index % (size+1)).
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be inserted 
void ref_linked_list_insert(ref_list_t *list, int index, ref_elem_t value);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements, where 0
/// means the first element and n-1 means the last element. If index is out of
/// bound, it will be adjusted to a valid index (index % size).
/// @param list the linked list
/// @param index the position in the list
/// @return the value removed
ref_elem_t ref_linked_list_remove(ref_list_t *list, int index);

/// @brief Retrieve an element from a linked list in O(n) time. The valid
/// values of index are [0,n-1] for a list of n elements, where 0 means the
/// first element and n-1 means the last element. If index is out of bound, it
/// will be adjusted to a valid index (index % size).
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return the value at the given position
ref_elem_t ref_linked_list_get(ref_list_t *list, int index);

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param value the element sought
/// @return true if element is in the list, else false
bool ref_linked_list_contains(ref_list_t *list, ref_elem_t value);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ref_linked_list_is_empty(ref_list_t *list);

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ref_linked_list_clear(ref_list_t *list);

/// @brief Test if a supplied property holds for all elements in a list. The
/// function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to
/// all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ref_linked_list_all(ref_list_t *list, ref_elem_predicate prop,
                           void *extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to
/// all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ref_linked_list_any(ref_list_t *list, ref_elem_predicate prop,
                           void *extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param fun the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to
/// all internal calls of fun
void ref_linked_list_apply_to_all(ref_list_t *list,
                                    ref_apply_elem_function fun,
                                    void *extra);

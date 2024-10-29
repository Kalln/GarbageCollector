#include "linked_list.h"


typedef struct link link_t;
struct link
{
    ref_elem_t value;
    link_t *next;
};

struct list
{
    link_t *first;
    link_t *last;
    ref_eq_function eq_function; 
    size_t size;
};


ref_list_t *ref_linked_list_create(ref_eq_function eq_function)
{
    ref_list_t *list = calloc(1, sizeof(ref_list_t));
    list->first = NULL;
    list->last = NULL;
    list->eq_function = eq_function; 
    list->size = 0;
    return list;
}


void ref_linked_list_destroy(ref_list_t *list)
{
    link_t *current = list->first;

    /* If there exists a (first) link; set first to the next link and free the
       link, then destroy the remaining list */
    if (current != NULL) {
        list->first = current->next;
        free(current);
        ref_linked_list_destroy(list);
    }
    /* If the list is empty, free the list */
    else {
        free(list);
    }
}


size_t ref_linked_list_size(ref_list_t *list)
{
    return list->size;
}


/* Helper function that creates a new link */
static link_t *link_create(ref_elem_t value, link_t *next)
{
    link_t *new_link = calloc(1, sizeof(link_t));
    new_link->value = value;
    new_link->next = next;
    return new_link;
}


void ref_linked_list_append(ref_list_t *list, ref_elem_t value)
{
    link_t *new_link = link_create(value, NULL);

    /* If list is empty, set first to new link */
    if (list->first == NULL) {
        list->first = new_link;
    }
    /* If list is not empty, link last link to new link */
    else {
        (list->last)->next = new_link;
    }

    /* Set last to new link and increse size of list */
    list->last = new_link;
    list->size++;
}


void ref_linked_list_prepend(ref_list_t *list, ref_elem_t value)
{
    link_t *new_link = link_create(value, NULL);

    /* If list is empty, set last to new link */
    if (list->first == NULL) {
        list->last = new_link;
    }
    /* If list is not empty, link new link to first link */
    else {
        new_link->next = list->first;
    }

    /* Set first to new link and increse size of list */
    list->first = new_link;
    list->size++;
}


/* Helper function that adjusts an index to be within [0, upper bound) */
static int adjust_index(int index, int upper_bound)
{
    /* If index is negative, add upper_bound to the remainder after division by
       upper_bound in order to be within [0, upper bound) before returning */
    if (index < 0) {
        return (index % upper_bound) + upper_bound; 
    }
    /* If the index is not negative, just return the remainder after division
       by upper_bound */
    else {
        return index % upper_bound;
    }
}


/* Helper function that finds the previous link to the link at a certain index
   index (no case for index=0) */
static link_t *linked_list_find_previous(ref_list_t *list, int index)
{
    link_t *current = list->first;

    /* Go to next link until at index-1 */
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }

    return current; 
}


void ref_linked_list_insert(ref_list_t *list, int index, ref_elem_t value)
{
    /* Adjust index */
    int adjusted_index = adjust_index(index, ref_linked_list_size(list) + 1);

    /* If link is to be inserted at the start of the list, use the prepend
       function */
    if (adjusted_index == 0) {
        ref_linked_list_prepend(list, value);
    }
    /* If link is not to be inserted at the start of the list; create link,
       link new link to previous and next link, set last to new link if there
       is no next link, and increase size of list */
    else {
        link_t *new_link = link_create(value, NULL);
        link_t *prev = linked_list_find_previous(list, adjusted_index); 
        link_t *next = prev->next;
        prev->next = new_link;
        new_link->next = next;
        
        if (new_link->next == NULL) {
            list->last = new_link;
        }

        list->size++;
    }
}


ref_elem_t ref_linked_list_remove(ref_list_t *list, int index)
{
    ref_elem_t removed_value; 

    /* Adjust index */
    int adjusted_index = adjust_index(index, ref_linked_list_size(list));

    /* If the first link is to be removed; save the value, free the first link,
       and set first to the second link */
    if (adjusted_index == 0) {
        link_t *next = list->first->next;
        removed_value = list->first->value; 
        free(list->first); 
        list->first = next; 
    }
    /* If the link to be removed is not the first link; save the value, free
       the link, link the previous link to the next, and set last to the
       previous link if there is no next link */
    else {
        link_t *prev = linked_list_find_previous(list, adjusted_index);

        link_t *next = prev->next->next;
        removed_value = prev->next->value; 
        free(prev->next); 
        prev->next = next;

        if (prev->next == NULL) {
            list->last = prev;
        }
    }
    /* Decrease the size of the list */
    list->size--; 

    return removed_value;
}


ref_elem_t ref_linked_list_get(ref_list_t *list, int index)
{
    /* Adjust index */
    int adjusted_index = adjust_index(index, ref_linked_list_size(list));
    
    /* If the wanted link is the first, return first */
    if (adjusted_index == 0) {
        return list->first->value; 
    }
    /* If the wanted link is not the first, find the previous link and return
       the value of the next link */
    else {
        link_t *prev = linked_list_find_previous(list, adjusted_index);
        return prev->next->value; 
    }
}


bool ref_linked_list_contains(ref_list_t *list, ref_elem_t value)
{
    link_t *current = list->first;

    /* Process all of the links in the list */
    for (int i = 0; i < ref_linked_list_size(list); i++) {
        /* If the value is found in the current link, return true */
        if (list->eq_function(current->value, value)) {
            return true;
        }
        /* If the value is not found in the current link, go to the next link */
        else {
            current = current->next;
        }
    }
    /* If the value was not found in any of the links in the list, return false */
    return false;
}


bool ref_linked_list_is_empty(ref_list_t *list)
{
    return ref_linked_list_size(list) == 0;
}


void ref_linked_list_clear(ref_list_t *list)
{
    /* Remove the first link in the list until the list is empty */
    while (list->first != NULL) {
        ref_linked_list_remove(list, 0);
    }
}


bool ref_linked_list_all(ref_list_t *list,
                           ref_elem_predicate prop,
                           void *extra)
{
    link_t *current = list->first;

    /* If the current link exists; return false if the predicate function
       returns false, otherwise go to the next link and continue to check */
    while (current != NULL) {
        if (prop(list, current->value, extra) == false) {
            return false;
        }
        current = current->next;
    }

    /* If the predicate function did not return false for any of the links in
       the list, return true */
    return true;
}

bool ref_linked_list_any(ref_list_t *list,
                           ref_elem_predicate prop,
                           void *extra)
{
    link_t *current = list->first;

    /* If the current link exists; return true if the predicate function
       returns true, otherwise go to the next link and continue to check */
    while (current != NULL) {
        if (prop(list, current->value, extra) == true) {
            return true;
        }
        current = current->next;
    }

    /* If the predicate function did not return true for any of the links in
       the list, return false */
    return false;
}


void ref_linked_list_apply_to_all(ref_list_t *list,
                                    ref_apply_elem_function fun,
                                    void *extra)
{
    link_t *current = list->first;

    /* If the current link exists; apply the function and go to the next link */
    while (current != NULL) {
        fun(&current->value, extra);
        current = current->next;
    }
}

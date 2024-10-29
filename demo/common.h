#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef struct list ioopm_list_t;
typedef struct node ioopm_node_t;
typedef union elem elem_t;

typedef bool (*ioopm_list_predicate)(size_t index, elem_t value, void *extra);
typedef void (*ioopm_list_apply_func)(size_t index, elem_t *value, void *extra);
typedef bool (*ioopm_eq_func)(elem_t a, elem_t b);

// Unions

union elem
{
    int i;
    size_t uns_int;
    bool b;
    float f;
    void *p;
};

// Structs

/**
 * @brief The sentinel of ioopm_linked_list, contains meta values
 */
struct list
{
    ioopm_node_t *first; // First entry of list
    ioopm_node_t *last;  // Last entry of list
    size_t size;
    ioopm_eq_func eq_fun; // Function for equality
};

/**
 * @brief An element in a linked list
 */
struct node
{
    elem_t value;       // Value of list index
    ioopm_node_t *tail; // Next element in list
};

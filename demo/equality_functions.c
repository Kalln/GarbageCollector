
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "equality_functions.h"

bool ioopm_int_equal(elem_t a, elem_t b) { return a.i == b.i; }

bool ioopm_bool_equal(elem_t a, elem_t b) { return a.b == b.b; }

bool ioopm_float_equal(elem_t a, elem_t b) { return a.f == b.f; }

bool ioopm_unsint_equal(elem_t a, elem_t b) { return a.uns_int == b.uns_int; }

bool ioopm_string_equal(elem_t a, elem_t b)
{
    return strcmp((char *) a.p, (char *) b.p) == 0;
}

size_t ioopm_djb2_hash(elem_t key)
{
    char *str = (char *) key.p;
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

size_t ioopm_simple_unsint_hash(elem_t key) { return key.uns_int; }

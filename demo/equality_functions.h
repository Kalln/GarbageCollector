#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

/** @brief Compares the int fields of two elem_t:s
 */
bool ioopm_int_equal(elem_t a, elem_t b);

/** @brief Compares the bool fields of two elem_t:s
 */
bool ioopm_bool_equal(elem_t a, elem_t b);

/** @brief Compares the float fields of two elem_t:s
 */
bool ioopm_float_equal(elem_t a, elem_t b);

/** @brief Compares the unsint fields of two elem_t:s
 */
bool ioopm_unsint_equal(elem_t a, elem_t b);

/** @brief Compares the string fields of two elem_t:s
 */
bool ioopm_string_equal(elem_t a, elem_t b);

/**
 * @brief djb2 copied from http://www.cse.yorku.ca/~oz/hash.html
 *        string hash function (and yes, I've read how it works)
 *
 * @param key string key
 * @return hashed size_t
 */
size_t ioopm_djb2_hash(elem_t key);

/**
 * @brief Simple unsigned int hash
 *
 * @param key unsigned int key
 * @return hashed size_t
 */
size_t ioopm_simple_unsint_hash(elem_t key);

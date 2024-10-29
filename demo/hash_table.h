#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "linked_list.h"
/**
 * @brief IOOPM Hash table
 */
typedef struct hash_table ioopm_hash_table_t;

/**
 * @brief Predicate function for IOOPM hashtable, to be used with ioopm_hash_table_any() and
 * ioopm_hash_table_all()
 */
typedef bool (*ioopm_predicate)(elem_t key, elem_t value, void *extra, void *extra2);

/**
 * @brief Apply function for IOOPM hashtable, to be used with ioopm_hash_table_apply_all()
 */
typedef void (*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);

/**
 * @brief IOOPM hashtable hash function
 */
typedef size_t (*ioopm_hash_function)(elem_t key);

/**
 * @brief Creates an IOOPM Hashtable, that uses provided hash function for hashes and
 * equivalance functions for keys and values.
 *
 * @param hash_function Function for hashing keys
 * @param key_eq_function Function for key equivalance, e.g int x == int y
 * @param value_eq_function Function for value equivalance
 * @return an empty hashtable with eq_functions for key and value
 */
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_function,
                                            ioopm_eq_func key_eq_function,
                                            ioopm_eq_func value_eq_function);

/**
 * @brief Deletes an IOOPM style Hashtable and deallocates the memory used by the
 *        hashtable
 *
 * @param ht IOOPM style Hashtable to be deleted
 */
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/**
 * @brief Inserts a value in hashtable with provided key, if key is already in hashtable we only update the value
 *
 * @param ht IOOPM style hashtable
 * @param key key to hashtable
 * @param value value to be inserted/updated
 */
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/**
 * @brief Lookup a key in an IOOPM style Hashtable. If key is found, it is loaded into ret
 *
 * @param ht IOOPM style hashtable
 * @param key key to hashtable
 * @param[out] ret if key is found, ret will be pointed to that entry
 * @return true if key is found, else false
 */
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *ret);

/**
 * @brief Removes an entry in IOOPM style hashtable if key (and therefore entry) exists.
 *        If entry exists, true is returned and value of deleted entry is stored in ret[out]
 *
 * @param ht IOOPM style hashtable
 * @param key key to hashtable
 * @param[out] ret if key is found, ret will be pointed to value of removed entry
 * @return true if the entry is found and removed, false if the entry wasn't found
 */
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *ret);

/**
 * @brief Checks the size of an IOOPM style hashtable
 *
 * @param ht IOOPM style hashtable
 * @return size of IOOPM style hashtable
 */
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/**
 * @brief Checks if an IOOPM style hashtable is empty
 *
 * @param ht IOOPM style hashtable
 * @return true if empty, false if not empty
 */
bool ioopm_hash_table_isempty(ioopm_hash_table_t *ht);

/**
 * @brief Deletes all entries in an IOOPM style hashtable
 *
 * @param ht IOOPM style hashtable
 */
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/**
 * @brief Get all keys from an IOOPM style hashtable and returns a IOOPM linked list
 *        with all keys. Returns an empty list if hashtable is empty.
 *
 * @param ht IOOPM style hashtable
 * @return IOOPM linked list of keys
 */
ioopm_list_t *ioopm_hash_table_getkeys(ioopm_hash_table_t *ht);

/**
 * @brief Get all values from an IOOPM style hashtable, and return an IOOPM linked list of pointers
 * to the values
 *
 * @param ht IOOPM style hashtable
 * @return an IOOPM linked list of pointers to values
 */
ioopm_list_t *ioopm_hash_table_getvalues(ioopm_hash_table_t *ht);

/**
 * @brief Checks if a string is in the IOOPM hashtable as a value
 *
 *
 * @param ht IOOPM style hashtable
 * @param value value to be looked for
 * @return true if string is in hashtable, else false
 */
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

/**
 * @brief  Check if a key is in IOOPM hashtable.
 *
 * @param ht IOOPM style hashtable
 * @param key elem_t key to be looked for
 * @return true if key is in hashtable, else false
 */
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/**
 * @brief Takes a predicate and check this predicate on all keys and values in hashtable
 *        Returns true if predicate is true in any key and/or value
 *
 * @param ht IOOPM style hashtable
 * @param pred ioopm_predicate predicate to check hashtable entrys for
 * @param arg Passable argument to predicate function
 */
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/**
 * @brief Takes a predicate and check this predicate on all keys and values in hashtable
 *        Returns true if predicate is true in all keys and/or value
 *        Returns false if the hashtable is empty
 *
 * @param ht IOOPM style hashtable
 * @param pred ioopm_predicate predicate that checks the hashtable entries
 * @param arg Passable argument to predicate function
 */
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/**
 * @brief Apply an ioopm_apply_function on all keys and/or values in a IOOPM style hashtable
 *
 * @param ht IOOPM style hashtable
 * @param fun ioopm_apply_function - Function to apply to all keys and/or values in hashtable
 * @param arg Passable argument to apply function
 */
void ioopm_hash_table_apply_all(ioopm_hash_table_t *ht, ioopm_apply_function fun,
                                void *arg);

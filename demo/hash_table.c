#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "hash_table.h"
#include "../src/refmem.h"

#define Num_buckets 33

// Type definitions
typedef struct entry entry_t;
typedef struct hash_table ioopm_hash_table_t;
typedef void (*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);
typedef bool (*ioopm_predicate)(elem_t key, elem_t value, void *extra1, void *extra2);
typedef size_t (*ioopm_hash_function)(elem_t key);

// Structs
/// @brief One linked-list entry for a hashtable
struct entry
{
    elem_t key;    // holds the key
    elem_t value;  // holds the value
    entry_t *next; // points to the next entry (possibly NULL)
};

/// @brief an IOOPM style Hashtable struct
struct hash_table
{
    size_t size;                  // Size of hashtable
    entry_t buckets[Num_buckets]; // Num of different buckets to put hashes
    ioopm_hash_function hash_fun; // Hash function
    ioopm_eq_func key_eq_fun;     // Function for key equality
    ioopm_eq_func value_eq_fun;   // Function for value equality
};

// Prototypes

/**
 * @brief Creates a new entry to the IOOPM style hashtable
 *
 * @param ht Hashtable that entry will be created in
 * @param key - key of the entry
 * @param value - value of the entry
 * @param next_entry - pointer to the next entry
 * @return entry_t* - New entry
 */
static entry_t *entry_create(ioopm_hash_table_t *ht, elem_t key, elem_t value, entry_t *next_entry);
/**
 * @brief Destroys an entry in an IOOPM style hashtable
 *
 * @param ht Hashtable that entry will be destroyed in
 * @param entry - entry to be destroyed
 */
static void entry_destroy(ioopm_hash_table_t *ht, entry_t *entry);
/**
 * @brief Find the entry before the entry of provided key.
 * If provided key does not exist, provides the entry previous to where it should be
 *
 * @param ht Hashtable to find entry in
 * @param key - Key that we want to find the previous entry to
 * @return entry_t* - the entry previous to the key
 */
static entry_t *find_prev_entry_for_key(ioopm_hash_table_t *ht, elem_t key);

/**
 * @brief Checks entries in a hashtable with a predicate. Depending on the bool argument, it checks if any or all entries satisfies predicate.
 *
 * @param ht IOOPM style hashtable
 * @param pred ioopm_predicate - Predicate
 * @param arg1 argument 1 for predicate function
 * @param arg2  argument 2 for predicate function
 * @param ret_if_pred_is_met boolean - false if all entries have to satisfy pred. true if any.
 */
static bool ht_for_each(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg1, void *arg2, bool ret_if_pred_is_met);

/**
 * @brief Predicate function for ioopm_hash_table_has_value. Compares two elem_t:s with the hashtables own value_eq_function
 *
 * @param value the value that is searched for
 * @param value_compare used as the value that 'value' is compared against
 * @param hash_table the hashtable in which the value is checked
 */
static bool hash_table_has_value(elem_t key_ignored, elem_t value, void *value_compare, void *hash_table);

/**
 * @brief Predicate function for ioopm_hash_table_has_key. Compares two elem_t:s with the hashtables own value_eq_function
 *
 * @param key the key that is searched for
 * @param key_compare used as the key that 'key' is compared against
 * @param hash_table the hashtable in which the key is checked
 */
static bool hash_table_has_key(elem_t key, elem_t value_ignored, void *key_compare, void *hash_table);

/**
 * @brief Get the value object
 *
 * @param current_entry
 * @return elem_t
 */
static elem_t get_value(entry_t *current_entry);

/**
 * @brief Check if key of current entry is equal to provided key
 *
 * @param ht IOOPM Hashtable
 * @param current_entry current entry in hashtable
 * @param key Key to check if equal to current entrys key
 * @return true
 * @return false
 */
static bool current_key_is_equal(ioopm_hash_table_t *ht, entry_t *current_entry, elem_t key);

/**
 * @brief Removes an entry in hashtable
 *
 * @param ht IOOPM Hashtable
 * @param current_entry Entry to remove
 * @param prev_entry Entry previous to the one removed
 */
static void remove_entry(ioopm_hash_table_t *ht, entry_t *current_entry, entry_t *prev_entry);

/**
 * @brief Get the current entry object
 *
 * @param ht IOOPM hashtable
 * @param bucket bucket to get first entry from
 * @return entry_t*
 */
entry_t *get_first_entry_in_bucket(ioopm_hash_table_t *ht, size_t bucket);

// Public Functions

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_function, ioopm_eq_func key_eq_function, ioopm_eq_func value_eq_function)
{
    // Allocate memory for ioopm_hash_table_t (Num_buckets entries)
    ioopm_hash_table_t *ht = allocate(sizeof(ioopm_hash_table_t), NULL);
    retain(ht);
    ht->key_eq_fun = key_eq_function;
    ht->value_eq_fun = value_eq_function;
    ht->size = 0;

    ht->hash_fun = hash_function;
    return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    // Clears all entries from hashtable
    ioopm_hash_table_clear(ht);

    // Free ioopm_hash_table
    release(ht);
    ht = NULL;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{

    // Find previous entry, that will point to NULL if last in buckets
    entry_t *prev_entry = find_prev_entry_for_key(ht, key);
    retain(prev_entry);
    entry_t *current_entry = prev_entry->next;
    retain(current_entry);

    //
    if (current_key_is_equal(ht, current_entry, key))
    {
        // Update existing entry value
        current_entry->value = value;
    }
    else
    {
        prev_entry->next = entry_create(ht, key, value, current_entry);
    }
    release(current_entry);
    release(prev_entry);
}

bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *ret)
{

    entry_t *prev_entry = find_prev_entry_for_key(ht, key);
    retain(prev_entry);
    entry_t *current_entry = prev_entry->next;
    retain(current_entry);

    if (current_key_is_equal(ht, current_entry, key))
    {
        *ret = get_value(current_entry);
        release(prev_entry);
        release(current_entry);
        return true;
    }
    else
    {
        release(prev_entry);
        release(current_entry);
        return false;
    }
}

bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *ret)
{

    entry_t *prev_entry = find_prev_entry_for_key(ht, key);
    entry_t *current_entry = prev_entry->next;
    retain(current_entry);

    if (current_key_is_equal(ht, current_entry, key))
    {

        *ret = get_value(current_entry);

        remove_entry(ht, current_entry, prev_entry);

        release(current_entry);

        return true;
    }
    else
    {
        release(prev_entry);
        release(current_entry);
        return false;
    }
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
    return ht->size;
}

bool ioopm_hash_table_isempty(ioopm_hash_table_t *ht)
{
    return ioopm_hash_table_size(ht) == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    for (size_t bucket = 0; bucket < Num_buckets; bucket += 1)
    {
        entry_t *current_bucket = &ht->buckets[bucket];
        entry_t *current_entry = current_bucket->next;

        while (current_entry != NULL)
        {
            retain(current_entry);
            // Store pointer to next entry before deleting it in entry
            entry_t *next_entry = current_entry->next;
            retain(next_entry);

            entry_destroy(ht, current_entry);
            // Update current entry with the pointer we previously saved
            release(current_entry);
            current_entry = next_entry;
            retain(next_entry);
            release(next_entry);
        }
        // Remove pointer to next to avoid dangling pointer
        current_bucket->next = NULL;
    }
    return;
}

ioopm_list_t *ioopm_hash_table_getkeys(ioopm_hash_table_t *ht)
{

    ioopm_list_t *result = ioopm_linkedlist_create(ht->key_eq_fun);
    retain(result);

    // Check all buckets for keys
    for (int bucket = 0; bucket < Num_buckets; bucket += 1)
    {
        entry_t *current_entry = get_first_entry_in_bucket(ht, bucket);

        while (current_entry != NULL)
        {
            retain(current_entry);
            ioopm_linkedlist_append(result, current_entry->key);
            release(current_entry);
            current_entry = current_entry->next;
        }
    }
    release(result);
    return result;
}

ioopm_list_t *ioopm_hash_table_getvalues(ioopm_hash_table_t *ht)
{
    ioopm_list_t *result = ioopm_linkedlist_create(ht->value_eq_fun);
    retain(result);

    // Check all buckets for values
    for (size_t bucket = 0; bucket < Num_buckets; bucket += 1)
    {
        entry_t *current_entry = get_first_entry_in_bucket(ht, bucket);

        while (current_entry != NULL)
        {
            retain(current_entry);
            ioopm_linkedlist_append(result, current_entry->value);
            release(current_entry);
            current_entry = current_entry->next;
        }
    }
    release(result);
    return result;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
    return ht_for_each(ht, hash_table_has_key, &key, ht, true);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
    return ht_for_each(ht, hash_table_has_value, &value, ht, true);
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    return ht_for_each(ht, pred, arg, NULL, true);
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    // Always false if hashtable is empty
    if (ht->size <= 0)
    {
        return false;
    }

    return ht_for_each(ht, pred, arg, NULL, false);
}

void ioopm_hash_table_apply_all(ioopm_hash_table_t *ht, ioopm_apply_function fun, void *arg)
{

    for (int bucket = 0; bucket < Num_buckets; bucket += 1)
    {
        entry_t *current_entry = get_first_entry_in_bucket(ht, bucket);

        // Loop over entries in bucket and apply function
        while (current_entry != NULL)
        {
            retain(current_entry);
            fun(current_entry->key, &current_entry->value, arg);
            release(current_entry);
            current_entry = current_entry->next;
        }
    }

    return;
}

// Private functions

static entry_t *entry_create(ioopm_hash_table_t *ht, elem_t key, elem_t value, entry_t *next_entry)
{
    // Allocate memory for new entry
    entry_t *new_entry = allocate(sizeof(entry_t), NULL);
    retain(new_entry);
    *new_entry = (entry_t){.key = key, .next = next_entry, .value = value};
    ht->size += 1;
    return new_entry;
}

static void entry_destroy(ioopm_hash_table_t *ht, entry_t *entry)
{

    ht->size -= 1;
    release(entry);
}

static entry_t *find_prev_entry_for_key(ioopm_hash_table_t *ht, elem_t key)
{
    // Abs and modulo to get hash in range of buckets
    size_t bucket = ht->hash_fun(key) % Num_buckets;
    // Initialize previous entry as the first entry passed by function
    entry_t *prev_entry = &ht->buckets[bucket];
    retain(prev_entry);
    // Initialize current entry as the next entry
    entry_t *current_entry = prev_entry->next;
    retain(current_entry);

    // If current entry is null or key equal to current entrys key, break loop and return previous entry
    while (current_entry != NULL && !ht->key_eq_fun(current_entry->key, key))
    {
        release(prev_entry);
        prev_entry = current_entry;
        retain(prev_entry);
        release(current_entry);
        current_entry = prev_entry->next;
        retain(current_entry);
    }
    release(current_entry);
    release(prev_entry);
    return prev_entry;
}

static bool ht_for_each(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg1, void *arg2, bool ret_if_pred_is_met)
{
    // Iterate over all buckets in hashtable
    for (size_t bucket = 0; bucket < Num_buckets; bucket += 1)
    {
        entry_t *current_entry = get_first_entry_in_bucket(ht, bucket);
        retain(current_entry);

        // Iterate over entrys in current bucket, if some key/value are equal we return the bool argument, else update current entry
        while (current_entry != NULL)
        {
            // Only ioopm_hashtable_all sends a false param, and needs a ! before the predicate, hence the 2 cases.
            // If the predicate is met, return false, else continue. If no entry meets the pred, return true
            if (!ret_if_pred_is_met)
            {
                if (!pred(current_entry->key, current_entry->value, arg1, arg2))
                {
                    release(current_entry);
                    return ret_if_pred_is_met;
                }
                else
                {
                    release(current_entry);
                    current_entry = current_entry->next;
                    retain(current_entry);
                }
            }
            // If the predicate is met, return true, else continue. If no entry meets the pred, return false
            else
            {
                if (pred(current_entry->key, current_entry->value, arg1, arg2))
                {
                    release(current_entry);
                    return ret_if_pred_is_met;
                }
                else
                {
                    release(current_entry);
                    current_entry = current_entry->next;
                    retain(current_entry);
                }
            }
        }
        release(current_entry);
    }
    return !ret_if_pred_is_met;
}

static bool hash_table_has_value(elem_t key_ignored, elem_t value, void *value_compare, void *hash_table)
{
    ioopm_hash_table_t *ht = (ioopm_hash_table_t *)hash_table;
    retain(ht);
    release(ht);

    return ht->value_eq_fun(value, *(elem_t *)value_compare);
}

static bool hash_table_has_key(elem_t key, elem_t value_ignored, void *key_compare, void *hash_table)
{
    ioopm_hash_table_t *ht = (ioopm_hash_table_t *)hash_table;
    retain(ht);
    release(ht);

    return ht->key_eq_fun(key, *(elem_t *)key_compare);
}

static bool current_key_is_equal(ioopm_hash_table_t *ht, entry_t *current_entry, elem_t key)
{
    return current_entry != NULL && ht->key_eq_fun(current_entry->key, key);
}

static elem_t get_value(entry_t *current_entry)
{
    return current_entry->value;
}

static void remove_entry(ioopm_hash_table_t *ht, entry_t *current_entry, entry_t *prev_entry)
{
    entry_t *next_entry = current_entry->next;
    retain(next_entry);
    prev_entry->next = next_entry;
    //release(next_entry);
    entry_destroy(ht, current_entry);
}

entry_t *get_first_entry_in_bucket(ioopm_hash_table_t *ht, size_t bucket)
{
    entry_t *current_bucket = &ht->buckets[bucket];
    return current_bucket->next;
}

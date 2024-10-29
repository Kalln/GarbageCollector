
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../demo/linked_list.h"
#include "../demo/hash_table.h"
#include "../demo/equality_functions.h"

#define Num_buckets 33

#define int_elem(x) \
    (elem_t) { .i = (x) }
#define ptr_elem(x) \
    (elem_t) { .p = (x) }
#define uns_int_elem(x) \
    (elem_t) { .uns_int = (x) }

int init_suite(void)
{
    // Change this function if you want to do something *before* you
    // run a test suite
    return 0;
}

int clean_suite(void)
{
    // Change this function if you want to do something *after* you
    // run a test suite
    return 0;
}

/// @brief Simple hashfunction for tests
/// @param key
/// @return
size_t ioopm_std_hash_function(elem_t key)
{
    return key.i;
}

// Test that we creates something with ioopm_hash_table_create
void test_create_destroy(void)
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    CU_ASSERT_PTR_NOT_NULL(ht);
    ioopm_hash_table_destroy(ht);
}

// Test lookup on an empty hashtable
void test_lookup_empty(void)
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    elem_t buf;
    for (int bucket = 0; bucket < Num_buckets; bucket += 1) /// 17 is a bit magical
    {
        CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, int_elem(bucket), &buf));
    }
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, int_elem(-1), &buf));
    ioopm_hash_table_destroy(ht);
}

// Tests for inserts
void single_insert_helper(ioopm_hash_table_t *ht, int key, char *teststr)
{
    elem_t buf;

    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, int_elem(key), &buf));
    ioopm_hash_table_insert(ht, int_elem(key), ptr_elem(teststr));
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, int_elem(key), &buf));
    CU_ASSERT_TRUE(ioopm_string_equal(buf, ptr_elem(teststr)));
}

void test_single_insert(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    // Test insert of one item in a hashtable
    single_insert_helper(ht1, 3, "foo");
    // Test insert of one negative item in a hashtable
    single_insert_helper(ht2, -2, "bar");
    // Test insert of one item at index 0 in a hashtable
    single_insert_helper(ht3, 0, "foo");

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

void test_several_insert(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    char *string_array[] = {"foo", "bar", "foobar", "fusball", "makaron", "couscous", "pizza"};
    int string_len = 7;
    for (int i = 0; i < string_len; i += 1)
    {
        single_insert_helper(ht1, i * 3, string_array[i]);
    }

    for (int i = 0; i < string_len; i += 1)
    {
        CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht1, int_elem(i * 3)));
        CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht1, ptr_elem(string_array[i])));
    }

    // Test that inserting with already existing key will update value
    for (int i = 0; i < string_len; i += 1)
    {
        ioopm_hash_table_insert(ht1, (elem_t){.i = i * 3}, (elem_t){.p = "another value"});
        CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht1, ptr_elem(string_array[i])));
    }

    ioopm_hash_table_destroy(ht1);
}

// Tests for remove
void single_remove_helper(ioopm_hash_table_t *ht, int key, char *str)
{
    elem_t buf;
    ioopm_hash_table_insert(ht, int_elem(key), ptr_elem(str));
    ioopm_hash_table_lookup(ht, int_elem(key), &buf);

    elem_t removed_value;

    // Checks if removed, value is correct, and that we dont find key in hashtable
    CU_ASSERT_TRUE(ioopm_hash_table_remove(ht, int_elem(key), &removed_value));
    CU_ASSERT_TRUE(ioopm_string_equal(removed_value, ptr_elem(str)));
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, int_elem(key), &buf));
}

void test_remove_single_entry(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    single_remove_helper(ht1, 3, "foo");
    single_remove_helper(ht1, 17, "bar");
    single_remove_helper(ht1, 0, "baz");
    single_remove_helper(ht1, -3, "bajz");

    ioopm_hash_table_destroy(ht1);
}

void test_remove_nonexisting_entry(void)
{
    ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    elem_t removed_value;
    for (int bucket = 0; bucket < Num_buckets; bucket += 1)
    {
        // Removes entry
        CU_ASSERT_FALSE(ioopm_hash_table_remove(ht, int_elem(bucket), &removed_value));
    }

    ioopm_hash_table_destroy(ht);
}

void test_remove_several_entry(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    char *string_array[] = {"foo", "bar", "foobar", "fusball", "makaron", "couscous", "pizza"};
    int string_len = 7;
    for (int i = 0; i < string_len; i += 1)
    {
        single_remove_helper(ht1, i * 3, string_array[i]);
    }
    ioopm_hash_table_destroy(ht1);
}

// Tests for size
void fill_hashtable(ioopm_hash_table_t *ht, int qty, char *str)
{
    for (int i = 0; i < qty; i += 1)
    {
        ioopm_hash_table_insert(ht, int_elem(i), ptr_elem(str));
    }
}

void test_hashtable_size(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    fill_hashtable(ht1, 100, "foo");
    fill_hashtable(ht3, 1, "bar");

    // check ht1 for 100 entries, ht2 for 0 entries(edgecase), ht3 for 1 entry(edgecase)
    CU_ASSERT_EQUAL(ioopm_hash_table_size(ht1), 100);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(ht2), 0);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(ht3), 1);

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

// Tests for is_empty
void test_hashtable_isempty(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    fill_hashtable(ht1, 10, "bar");
    fill_hashtable(ht2, 1, "bar");

    CU_ASSERT_FALSE(ioopm_hash_table_isempty(ht1));
    CU_ASSERT_FALSE(ioopm_hash_table_isempty(ht2));
    CU_ASSERT_TRUE(ioopm_hash_table_isempty(ht3));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

// Tests for clear
void test_hashtable_clear(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    fill_hashtable(ht1, 10, "foo");
    fill_hashtable(ht3, 1, "bar");

    // Check if filled and empty before clearing
    CU_ASSERT_FALSE(ioopm_hash_table_isempty(ht1));
    CU_ASSERT_TRUE(ioopm_hash_table_isempty(ht2));
    CU_ASSERT_FALSE(ioopm_hash_table_isempty(ht3));

    ioopm_hash_table_clear(ht1);
    ioopm_hash_table_clear(ht2);
    ioopm_hash_table_clear(ht3);

    // Check if cleared, ht1 filled before clear, ht2 empty before clear (edgecase), ht3 single entry before clear
    CU_ASSERT_TRUE(ioopm_hash_table_isempty(ht1));
    CU_ASSERT_TRUE(ioopm_hash_table_isempty(ht2));
    CU_ASSERT_TRUE(ioopm_hash_table_isempty(ht3));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

// Tests for keys and values
void insert_keys_values_from_array(ioopm_hash_table_t *ht, int keys[], char *values[], int len)
{
    for (int i = 0; i < len; i += 1)
    {
        ioopm_hash_table_insert(ht, int_elem(keys[i]), ptr_elem(values[i]));
    }
}

void test_hashtable_keys(void)
{
    int keys[6] = {5, 65, 23, 98, 0, 10};
    char *values[6] = {"five", "sixtyfive", "twentythree", "ninetyeight", "zero", "ten"};
    bool found_keys[6] = {false};

    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    insert_keys_values_from_array(ht1, keys, values, 6);

    ioopm_list_t *hash_keys = ioopm_hash_table_getkeys(ht1);

    // For each element in hash_keys, see if key is in keys
    for (int i = 0; i < 6; i += 1)
    {
        if (ioopm_linkedlist_contains(hash_keys, int_elem(keys[i])))
        {
            found_keys[i] = true;
        }
    }

    // Check each found if true
    for (int i = 0; i < 6; i += 1)
    {
        CU_ASSERT_TRUE(found_keys[i]);
    }

    // Frees up memory on heap
    ioopm_linkedlist_destroy(hash_keys);
    ioopm_hash_table_destroy(ht1);
}

void test_hashtable_values(void)
{
    int keys[6] = {5, 65, 23, 98, 0, 10};
    char *values[6] = {"five", "sixtyfive", "twentythree", "ninetyeight", "zero", "ten"};
    bool found_keys[6] = {false};

    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    insert_keys_values_from_array(ht1, keys, values, 6);

    ioopm_list_t *hash_values = ioopm_hash_table_getvalues(ht1);
    bool error_check = false;
    // For each value in hash_values, see if value is in values
    for (int i = 0; i < 6; i += 1)
    {
        for (int j = 0; j < 6; j += 1)
        {
            if (ioopm_string_equal(ioopm_linkedlist_get(hash_values, i, &error_check), ptr_elem(values[j])) && !error_check)
            {
                found_keys[i] = true;
            }
        }
    };
    // Check each found if true
    for (int i = 0; i < 6; i += 1)
    {
        CU_ASSERT_TRUE(found_keys[i]);
    }

    // Frees up memory on heap
    ioopm_linkedlist_destroy(hash_values);
    ioopm_hash_table_destroy(ht1);
}

void test_hashtable_keys_to_values(void)
{
    int keys[6] = {5, 65, 23, 98, 0, 10};
    char *values[6] = {"five", "sixtyfive", "twentythree", "ninetyeight", "zero", "ten"};

    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    insert_keys_values_from_array(ht1, keys, values, 6);

    ioopm_list_t *hash_keys = ioopm_hash_table_getkeys(ht1);
    ioopm_list_t *hash_values = ioopm_hash_table_getvalues(ht1);

    // For each key in hash_keys, check that hash_value has the corresponding value
    for (int i = 0; i < 6; i += 1)
    {
        bool key_out_bound = false;
        elem_t key = ioopm_linkedlist_get(hash_keys, i, &key_out_bound);
        for (int j = 0; j < 6; j += 1)
        {
            if (!key_out_bound && key.i == keys[j])
            {
                elem_t value = ioopm_linkedlist_get(hash_values, i, &key_out_bound);
                CU_ASSERT_TRUE(ioopm_string_equal(value, ptr_elem(values[j])));
            }
        }
    }

    // Frees up memory on heap
    ioopm_linkedlist_destroy(hash_keys);
    ioopm_linkedlist_destroy(hash_values);
    ioopm_hash_table_destroy(ht1);
}

void test_empty_hashtable_keys_values(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    ioopm_list_t *hash_keys = ioopm_hash_table_getkeys(ht1);
    ioopm_list_t *hash_values = ioopm_hash_table_getvalues(ht1);

    // Checks if first index in arrays are null
    CU_ASSERT_TRUE(ioopm_linkedlist_isempty(hash_keys));
    CU_ASSERT_TRUE(ioopm_linkedlist_isempty(hash_values));

    // Frees up memory on heap
    ioopm_linkedlist_destroy(hash_keys);
    ioopm_linkedlist_destroy(hash_values);
    ioopm_hash_table_destroy(ht1);
}

// Tests for has_key and has_value (and implicitly hash_table_any)
void test_hashtable_has_key(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    ioopm_hash_table_insert(ht1, int_elem(5), ptr_elem("foo"));
    ioopm_hash_table_insert(ht2, int_elem(0), ptr_elem("bar"));
    ioopm_hash_table_insert(ht3, int_elem(-1), ptr_elem("baz"));

    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht1, int_elem(5)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht1, int_elem(6)));
    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht2, int_elem(0)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht2, int_elem(1)));
    CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht3, int_elem(-1)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht3, int_elem(1)));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

void test_hashtable_has_value(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    char *teststring1 = "foo bar baz";
    char *teststring2 = "baz bar foo";
    char *teststring1_dup = strdup(teststring1);
    char *teststring2_dup = strdup(teststring2);

    ioopm_hash_table_insert(ht1, int_elem(5), ptr_elem(teststring1));
    ioopm_hash_table_insert(ht2, int_elem(0), ptr_elem(teststring2));
    ioopm_hash_table_insert(ht3, int_elem(-1), ptr_elem("foo"));

    CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht1, ptr_elem(teststring1)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht1, ptr_elem(teststring2)));
    CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht2, ptr_elem(teststring2)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht2, ptr_elem(teststring1)));
    CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht1, ptr_elem(teststring1_dup)));
    CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht2, ptr_elem(teststring2_dup)));

    free(teststring1_dup);
    free(teststring2_dup);
    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

void test_hashtable_dont_have_key(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    ioopm_hash_table_insert(ht1, int_elem(5), ptr_elem("foo"));
    ioopm_hash_table_insert(ht2, int_elem(0), ptr_elem("bar"));

    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht1, int_elem(3)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht2, int_elem(3)));
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht3, int_elem(3)));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

void test_hashtable_dont_have_value(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht3 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    ioopm_hash_table_insert(ht1, int_elem(5), ptr_elem("foo"));
    ioopm_hash_table_insert(ht2, int_elem(0), ptr_elem("bar"));

    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht1, ptr_elem("bar")));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht2, ptr_elem("foo")));
    CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht3, ptr_elem("foo")));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
    ioopm_hash_table_destroy(ht3);
}

// Tests for ioopm_hash_table_any and ioopm_hash_table_all

static bool key_equiv(elem_t key, elem_t value_ignored, void *x, void *y)
{
    return (key.i == *((int *)x));
}

static bool string_value_equiv(elem_t key_ignored, elem_t value, void *x, void *y)
{
    return (ioopm_string_equal(value, *((elem_t *)x)));
}

static bool int_key_less_than(elem_t key, elem_t value_ignored, void *x, void *y)
{
    return (key.i < *((int *)x));
}
// Dumb function only for test_hashtable_all_several_entries
static bool string_value_in_array(elem_t key_ignored, elem_t value, void *x, void *y)
{
    // Compare all values in x to see if value is any of them
    for (int i = 0; i < 6; i += 1)
    {
        if (strcmp((char *)value.p, ((char **)x)[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// Tests for ioopm_hash_table_any

void test_hashtable_any_empty(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    for (int key = 0; key <= 100; key += 1)
    {
        CU_ASSERT_FALSE(ioopm_hash_table_any(ht1, key_equiv, &key));
    }

    ioopm_hash_table_destroy(ht1);
}

void test_hashtable_any_single(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    // keys and values to be called to with pointers
    int key1 = 0;
    int key2 = 5;
    char *value1 = "far";
    char *value2 = "bar";

    ioopm_hash_table_insert(ht1, int_elem(key1), ptr_elem(value1));
    ioopm_hash_table_insert(ht2, int_elem(key2), ptr_elem(value2));

    // Tests for keys
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht1, key_equiv, &key1));
    CU_ASSERT_FALSE(ioopm_hash_table_any(ht1, key_equiv, &key2));
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht2, key_equiv, &key2));
    CU_ASSERT_FALSE(ioopm_hash_table_any(ht2, key_equiv, &key1));

    // Tests for values
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht1, string_value_equiv, &value1));
    CU_ASSERT_FALSE(ioopm_hash_table_any(ht1, string_value_equiv, &value2));
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht2, string_value_equiv, &value2));
    CU_ASSERT_FALSE(ioopm_hash_table_any(ht2, string_value_equiv, &value1));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
}

void test_hashtable_any_several_entries(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    // Fill ht1 with 100 elements with same value
    int num_entries = 100;
    char *teststring = "Foo";
    fill_hashtable(ht1, num_entries, teststring);

    // Fill ht2 with 6 elements, all unique values
    int keys[6] = {5, 65, 23, 98, 0, 10};
    char *values[6] = {"five", "sixtyfive", "twentythree", "ninetyeight", "zero", "ten"};
    insert_keys_values_from_array(ht2, keys, values, 6);

    // Tests for ht1, check if all keys are less than 100 (highest should be 99)
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht1, int_key_less_than, &num_entries));
    num_entries = 98; // We have 99 as highest key
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht1, int_key_less_than, &num_entries));

    // Tests for ht1, check if all values are "Foo"
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht1, string_value_equiv, &teststring))
    char *wrong_string = "Bar";
    CU_ASSERT_FALSE(ioopm_hash_table_any(ht1, string_value_equiv, &wrong_string))

    // Tests for ht2, check if all keys are less than 100
    int max_key = 99;
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht2, int_key_less_than, &max_key));
    max_key = 23; // Make max lower to test failure
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht2, int_key_less_than, &max_key));

    // Tests for ht2, check if strings
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht2, string_value_in_array, &values));
    ioopm_hash_table_insert(ht2, int_elem(3), ptr_elem("three")); // Add value to test failure
    CU_ASSERT_TRUE(ioopm_hash_table_any(ht2, string_value_in_array, &values));

    // Free hashtables
    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
}

// Tests for ioopm_hash_table_all

void test_hashtable_all_several_entries(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    // Fill ht1 with 100 elements with same value
    int num_entries = 100;
    char *teststring = "Foo";
    fill_hashtable(ht1, num_entries, teststring);

    // Fill ht2 with 6 elements, all unique values
    int keys[6] = {5, 65, 23, 98, 0, 10};
    char *values[6] = {"five", "sixtyfive", "twentythree", "ninetyeight", "zero", "ten"};
    insert_keys_values_from_array(ht2, keys, values, 6);

    // Tests for ht1, check if all keys are less than 100 (highest should be 99)
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, int_key_less_than, &num_entries));
    num_entries = 98; // We have 99 as highest key
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, int_key_less_than, &num_entries));

    // Tests for ht1, check if all values are "Foo"
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, string_value_equiv, &teststring))
    char *wrong_string = "Bar";
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, string_value_equiv, &wrong_string))

    // Tests for ht2, check if all keys are less than 100
    int max_key = 99;
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht2, int_key_less_than, &max_key));
    max_key = 23; // Make max lower to test failure
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht2, int_key_less_than, &max_key));

    // Tests for ht2, check if strings
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht2, string_value_in_array, &values));
    ioopm_hash_table_insert(ht2, int_elem(3), ptr_elem("three")); // Add value to test failure
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht2, string_value_in_array, &values));

    // Free hashtables
    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
}

void test_hashtable_all_single(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);
    ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    // keys and values to be called to with pointers
    int key1 = 0;
    int key2 = 5;
    char *value1 = "far";
    char *value2 = "bar";

    ioopm_hash_table_insert(ht1, int_elem(key1), ptr_elem(value1));
    ioopm_hash_table_insert(ht2, int_elem(key2), ptr_elem(value2));

    // Tests for keys
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, key_equiv, &key1));
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, key_equiv, &key2));
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht2, key_equiv, &key2));
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht2, key_equiv, &key1));

    // Tests for values
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, string_value_equiv, &value1));
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, string_value_equiv, &value2));
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht2, string_value_equiv, &value2));
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht2, string_value_equiv, &value1));

    ioopm_hash_table_destroy(ht1);
    ioopm_hash_table_destroy(ht2);
}

void test_hashtable_all_empty(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    for (int key = 0; key <= 100; key += 1)
    {
        CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, key_equiv, &key));
    }

    ioopm_hash_table_destroy(ht1);
}

// Tests for ioopm_hash_table_apply_all
static void change_value(elem_t key_ignored, elem_t *value, void *new_value)
{
    char *new_string = *(char **)new_value;
    value->p = new_string;
}

void test_hashtable_applyall(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    char *teststring = "foo";
    char *newstring = "FOO";
    fill_hashtable(ht1, 5, teststring);
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, string_value_equiv, &teststring));
    ioopm_hash_table_apply_all(ht1, change_value, &newstring);
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, string_value_equiv, &teststring));
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, string_value_equiv, &newstring));

    ioopm_hash_table_destroy(ht1);
}

void test_hashtable_applyall_single(void)
{
    ioopm_hash_table_t *ht1 = ioopm_hash_table_create(ioopm_std_hash_function, ioopm_int_equal, ioopm_string_equal);

    char *teststring = "foo";
    char *newstring = "FOO";
    fill_hashtable(ht1, 1, teststring);
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, string_value_equiv, &teststring));
    ioopm_hash_table_apply_all(ht1, change_value, &newstring);
    CU_ASSERT_FALSE(ioopm_hash_table_all(ht1, string_value_equiv, &teststring));
    CU_ASSERT_TRUE(ioopm_hash_table_all(ht1, string_value_equiv, &newstring));

    ioopm_hash_table_destroy(ht1);
}

// MAIN
int main(void)
{
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
    {
        return CU_get_error();
    }

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite ioopm_hashtable_tests = CU_add_suite("Hashtable test suite", init_suite, clean_suite);
    CU_pSuite ioopm_hashtable_tests_any_all_apply = CU_add_suite("Hashtable test any, all, apply suite", init_suite, clean_suite);
    if (ioopm_hashtable_tests == NULL || ioopm_hashtable_tests_any_all_apply == NULL)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // This is where we add the test functions to our test suite.
    // For each call to CU_add_test we specify the test suite, the
    // name or description of the test, and the function that runs
    // the test in question. If you want to add another test, ju€st
    // copy a line below and change the information
    if (
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: created and destroyed", test_create_destroy) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: lookup on empty hashtable", test_lookup_empty) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: single insert", test_single_insert) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: several inserts", test_several_insert) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: single remove", test_remove_single_entry) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: several remove", test_remove_several_entry) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: nonexistent remove ", test_remove_nonexisting_entry) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: hashtable size ", test_hashtable_size) == NULL) || 
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: hashtable is empty ", test_hashtable_isempty) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: hashtable is cleared ", test_hashtable_clear) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: getkeys finds all keys ", test_hashtable_keys) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: getvalues finds all values", test_hashtable_values) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: getkeys and getvalues map to the same index", test_hashtable_keys_to_values) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: getkeys and getvalues returns empty", test_empty_hashtable_keys_values) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests, "Test IOOPM hash: has_keys has keys", test_hashtable_has_key) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: has_values has value", test_hashtable_has_value) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: has_values dont have key", test_hashtable_dont_have_key) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: has_values dont have value", test_hashtable_dont_have_key) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_any works on empty", test_hashtable_any_empty) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_any works on single entry", test_hashtable_any_single) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_any works on several entries", test_hashtable_any_several_entries) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_all works on several entries", test_hashtable_all_several_entries) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_all works on one entry", test_hashtable_all_single) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_all works on empty hashtables", test_hashtable_all_empty) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_apply_all works on several entries", test_hashtable_applyall) == NULL) ||
        (CU_add_test(ioopm_hashtable_tests_any_all_apply, "Test IOOPM hash: ioopm_hash_table_apply_all works on single entry", test_hashtable_applyall_single) == NULL) || 
        0)
    {
        // If adding any of the tests fails, we tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_NORMAL);

    // This is where the tests are actually run!
    CU_basic_run_tests();

    // Get the number of failed asserts
    unsigned int failures = CU_get_number_of_failures();

    // Tear down CUnit before exiting
    CU_cleanup_registry();

    return failures > 0
        ? 1
        : 0;
}

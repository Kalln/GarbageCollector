
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../demo/linked_list.h"
#include "../demo/iterator.h"
#include "../demo/equality_functions.h"
#include "../src/refmem_testing.h"

#define int_elem(x) \
    (elem_t) { .i = (x) }
#define ptr_elem(x) \
    (elem_t) { .p = (x) }
#define uns_int_elem(x) \
    (elem_t) { .uns_int = (x) }

#define ioopm_int_str_ll_insert(list, i, s) \
    ioopm_linkedlist_insert(list, (size_t)i, ptr_elem(s))

#define ioopm_int_str_ll_append(list, s) \
    ioopm_linkedlist_append(list, ptr_elem(s))

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

void fill_list(ioopm_list_t *list, int num_items)
{
    for (int i = 1; i <= num_items; i += 1)
    {
        ioopm_linkedlist_append(list, uns_int_elem(i));
    }
}

// Tests for create & destroy, to be runned together with valgrind'

void test_create_destroy(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_TRUE(list->size == 0);
    ioopm_linkedlist_destroy(list);
}

// Tests for size
void test_size(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    int num_items = 100;
    fill_list(list, num_items);
    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), num_items);

    ioopm_linkedlist_destroy(list);
}

// Tests for insert

void test_insert_once(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    size_t testnum = 234567;

    ioopm_linkedlist_insert(list, 0, uns_int_elem(testnum));

    // Size and value are correct
    CU_ASSERT_TRUE(list->size == 1);
    bool flag = list->eq_fun(list->last->value, uns_int_elem(testnum));
    CU_ASSERT_TRUE(flag);

    // First and last pointers are updated correctly
    CU_ASSERT_PTR_NULL(list->first->tail);
    CU_ASSERT_TRUE(list->eq_fun(list->first->value, uns_int_elem(testnum)));
    CU_ASSERT_PTR_NULL(list->last->tail);
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, uns_int_elem(testnum)));
    CU_ASSERT_TRUE(list->first == list->last);

    ioopm_linkedlist_destroy(list);
}

void test_insert_several(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);

    // Insert first
    ioopm_linkedlist_insert(list, 0, int_elem(-7));
    ioopm_linkedlist_insert(list, 0, int_elem(8));
    CU_ASSERT_TRUE(list->eq_fun(list->first->value, int_elem(8)));
    CU_ASSERT_PTR_NOT_NULL(list->first->tail);
    CU_ASSERT_TRUE(list->eq_fun(list->first->tail->value, int_elem(-7)));
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, uns_int_elem(-7)));
    CU_ASSERT_PTR_NULL(list->last->tail);
    CU_ASSERT_TRUE(list->size == 2);

    // Insert last
    ioopm_linkedlist_insert(list, 2, int_elem(6));
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, int_elem(6)));
    CU_ASSERT_TRUE(list->eq_fun(list->first->tail->tail->value, int_elem(6)));
    CU_ASSERT_TRUE(list->size == 3);

    // Insert middle
    ioopm_linkedlist_insert(list, 2, int_elem(2));
    CU_ASSERT_TRUE(list->eq_fun(list->first->tail->tail->value, int_elem(2)));
    CU_ASSERT_TRUE(list->size == 4);
    CU_ASSERT_TRUE(list->eq_fun(list->first->value, int_elem(8)));
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, int_elem(6)));
    CU_ASSERT_TRUE(list->first->tail->tail->tail == list->last);
    CU_ASSERT_PTR_NULL(list->first->tail->tail->tail->tail);

    ioopm_linkedlist_destroy(list);
}

// Tests for remove

void test_remove_once(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    bool error_detect = false;

    ioopm_linkedlist_insert(list, 0, uns_int_elem(1));
    CU_ASSERT_TRUE(list->size == 1);

    // Remove the only node and check that sentinel is correct
    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 0, &error_detect), uns_int_elem(1)));
    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), 0);
    CU_ASSERT_PTR_NULL(list->first);
    CU_ASSERT_PTR_NULL(list->last);

    ioopm_linkedlist_destroy(list);
}

void test_remove_several(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    bool error_detect = false;

    ioopm_linkedlist_insert(list, 0, int_elem(3));
    ioopm_linkedlist_insert(list, 0, int_elem(1));
    ioopm_linkedlist_insert(list, 1, int_elem(2));

    CU_ASSERT_TRUE(list->size == 3);
    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 2, &error_detect), int_elem(3)));
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, int_elem(2)));

    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 1, &error_detect), int_elem(2)));
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, int_elem(1)));
    CU_ASSERT_TRUE(list->size == 1);
    CU_ASSERT_TRUE(list->last == list->first);

    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 0, &error_detect), int_elem(1)));
    CU_ASSERT_PTR_NULL(list->last);
    CU_ASSERT_PTR_NULL(list->first);
    CU_ASSERT_TRUE(list->size == 0);

    // Test with index 0 and size 2
    ioopm_linkedlist_insert(list, 0, int_elem(3));
    ioopm_linkedlist_insert(list, 0, int_elem(1));
    ioopm_linkedlist_insert(list, 1, int_elem(2));

    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 0, &error_detect), int_elem(1)));

    // Index >0, not last node
    ioopm_linkedlist_insert(list, 0, int_elem(4));
    ioopm_linkedlist_insert(list, 1, int_elem(5));

    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 2, &error_detect), int_elem(2)));

    ioopm_linkedlist_destroy(list);
}

void test_remove_outofbounds(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    bool error_detect = false;

    // Insert at index 0
    ioopm_linkedlist_insert(list, 0, int_elem(1));
    // Remove from index 1 which is out of bounds causes error
    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_remove(list, 1, &error_detect), int_elem(0)));
    CU_ASSERT_TRUE(error_detect);

    ioopm_linkedlist_destroy(list);
}

// Tests for get

void test_get_first(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    bool error_detect = false;

    // Add single element to empty list
    ioopm_linkedlist_append(list, uns_int_elem(1));

    // Tests that we get first value, and no errors
    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_get(list, 0, &error_detect), uns_int_elem(1)));
    CU_ASSERT_FALSE(error_detect);

    // Tests that we get 0 if index is over n - 1, and get an error
    CU_ASSERT_TRUE(ioopm_int_equal(ioopm_linkedlist_get(list, 3, &error_detect), int_elem(0)));
    CU_ASSERT_TRUE(error_detect);

    ioopm_linkedlist_destroy(list);
}

void test_get_empty(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    bool error_detect = false;

    // Tests that we get 0 if we dont find any value, and get an error.
    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_get(list, 0, &error_detect), int_elem(0)));
    CU_ASSERT_TRUE(error_detect);

    ioopm_linkedlist_destroy(list);
}

// Tests for append, prepend and insert

void test_append_once(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);

    ioopm_linkedlist_append(list, uns_int_elem(1));
    ioopm_node_t *first_entry = list->first;
    ioopm_node_t *last_entry = list->last;

    // Tests if first value is 1, and next entry is null
    CU_ASSERT_TRUE(list->eq_fun(first_entry->value, uns_int_elem(1)));
    CU_ASSERT_PTR_NULL(first_entry->tail);

    // Tests that metadata in list is updated
    CU_ASSERT_EQUAL(last_entry, first_entry);
    CU_ASSERT_EQUAL(list->size, 1);

    ioopm_linkedlist_destroy(list);
}

void test_append_several(void)
{

    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    bool error_detect = false;

    // Fill_list() uses append to append n-1 items
    fill_list(list, 100);

    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), 100);
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, uns_int_elem(100)));
    CU_ASSERT_TRUE(list->eq_fun(list->first->value, uns_int_elem(1)));
    CU_ASSERT_TRUE(list->eq_fun(ioopm_linkedlist_get(list, 49, &error_detect), uns_int_elem(50)));
    CU_ASSERT_FALSE(error_detect);

    ioopm_linkedlist_destroy(list);
}

void test_prepend_once(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    elem_t single_value = {.uns_int = 666};
    // Prepend once on a empty list
    ioopm_linkedlist_prepend(list, single_value);
    ioopm_node_t *first_entry = list->first;
    ioopm_node_t *last_entry = list->last;

    // Tests if first value is 1, and next entry is null
    CU_ASSERT_TRUE(list->eq_fun(first_entry->value, single_value));
    CU_ASSERT_PTR_NULL(first_entry->tail);

    // Tests that metadata in list is updated
    CU_ASSERT_EQUAL(last_entry, first_entry);
    CU_ASSERT_EQUAL(list->size, 1);

    ioopm_linkedlist_destroy(list);
}

void test_prepend_several(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    int no_insert = 3;
    elem_t values[3] = {uns_int_elem(-1), uns_int_elem(0), uns_int_elem(42)};

    for (int i = 0; i < no_insert; i += 1)
    {
        ioopm_linkedlist_prepend(list, values[i]);
    }

    int first_index = 0;
    int last_index = no_insert - 1;
    int middle_index = last_index / 2;

    // Test that values are in list
    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, values[first_index]));
    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, values[middle_index]));
    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, values[last_index]));

    // Tests that metadata is updated (first should point to last index in values)
    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), no_insert);
    CU_ASSERT_TRUE(list->eq_fun(list->first->value, values[last_index]));
    CU_ASSERT_TRUE(list->eq_fun(list->last->value, values[first_index]));

    ioopm_linkedlist_destroy(list);
}

// Tests for any

static bool int_value_equal(size_t index, elem_t x, void *y)
{
    int z = *(int *)y;
    return ioopm_int_equal(x, int_elem(z));
}

void test_any_single(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);

    int correct = 1;
    int incorrect = 2;

    fill_list(list, 1);

    CU_ASSERT_TRUE(ioopm_linkedlist_any(list, int_value_equal, &correct));
    CU_ASSERT_FALSE(ioopm_linkedlist_any(list, int_value_equal, &incorrect));

    ioopm_linkedlist_destroy(list);
}

void test_any_several(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    fill_list(list, 100);
    int one = 1;
    int fifty = 50;
    int hundred = 100;
    int hundredone = 101;
    CU_ASSERT_TRUE(ioopm_linkedlist_any(list, int_value_equal, &one));
    CU_ASSERT_TRUE(ioopm_linkedlist_any(list, int_value_equal, &fifty));
    CU_ASSERT_TRUE(ioopm_linkedlist_any(list, int_value_equal, &hundred));
    CU_ASSERT_FALSE(ioopm_linkedlist_any(list, int_value_equal, &hundredone));

    ioopm_linkedlist_destroy(list);
}

// Tests for contains

void test_contains_single(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    ioopm_linkedlist_append(list, uns_int_elem(5));

    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, uns_int_elem(5)));
    CU_ASSERT_FALSE(ioopm_linkedlist_contains(list, uns_int_elem(3)));

    ioopm_linkedlist_destroy(list);
}

void test_contains_several(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);

    fill_list(list, 100);

    // Tests that list contains 1, 50, 100, but not 0 or 101 (edgecase)
    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, uns_int_elem(1)));
    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, uns_int_elem(50)));
    CU_ASSERT_TRUE(ioopm_linkedlist_contains(list, uns_int_elem(100)));
    CU_ASSERT_FALSE(ioopm_linkedlist_contains(list, uns_int_elem(0)));
    CU_ASSERT_FALSE(ioopm_linkedlist_contains(list, uns_int_elem(101)));

    ioopm_linkedlist_destroy(list);
}

void test_contains_empty(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);

    CU_ASSERT_FALSE(ioopm_linkedlist_contains(list, uns_int_elem(0)));
    CU_ASSERT_FALSE(ioopm_linkedlist_contains(list, uns_int_elem(1)));
    CU_ASSERT_FALSE(ioopm_linkedlist_contains(list, uns_int_elem(5)));

    ioopm_linkedlist_destroy(list);
}

// Tests for all

static bool int_value_less_than(size_t index, elem_t x, void *y)
{
    return (x.i < *((int *)y));
}

static void multiply(size_t index_ignore, elem_t *x, void *y)
{
    elem_t b = *(elem_t *)y;
    x->i = x->i * b.i;
}

void test_all_single(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    ioopm_linkedlist_append(list, uns_int_elem(10));

    // Test that single value is less than 11 but not less than 10
    int test = 11;
    CU_ASSERT_TRUE(ioopm_linkedlist_all(list, int_value_less_than, &test));
    test = 10;
    CU_ASSERT_FALSE(ioopm_linkedlist_all(list, int_value_less_than, &test));

    ioopm_linkedlist_destroy(list);
}

void test_all_several(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    fill_list(list, 10);

    // Test that several value is less than 11 but not less than 10
    int test = 11;
    CU_ASSERT_TRUE(ioopm_linkedlist_all(list, int_value_less_than, &test));
    test = 10;
    CU_ASSERT_FALSE(ioopm_linkedlist_all(list, int_value_less_than, &test));

    ioopm_linkedlist_destroy(list);
}

void test_all_empty(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);

    // Test that empty list returns false
    int test = 20;
    CU_ASSERT_FALSE(ioopm_linkedlist_all(list, int_value_less_than, &test));

    ioopm_linkedlist_destroy(list);
}

// Apply all
void test_applyall(void)
{
    elem_t factor = int_elem(3);
    bool error_detect = false;

    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_int_equal);
    ioopm_linkedlist_append(list, int_elem(-1));
    ioopm_linkedlist_append(list, int_elem(0));
    ioopm_linkedlist_append(list, int_elem(3));
    ioopm_linkedlist_append(list, int_elem(4));

    ioopm_linkedlist_apply_all(list, &multiply, &factor);

    CU_ASSERT_EQUAL(ioopm_linkedlist_get(list, 0, &error_detect).i, -3);
    CU_ASSERT_EQUAL(ioopm_linkedlist_get(list, 1, &error_detect).i, 0);
    CU_ASSERT_EQUAL(ioopm_linkedlist_get(list, 2, &error_detect).i, 9);
    CU_ASSERT_EQUAL(ioopm_linkedlist_get(list, 3, &error_detect).i, 12);

    ioopm_linkedlist_destroy(list);
}

// TESTS FOR ITERATOR

void test_iterator_create_destroy(void)
{
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Checks that iter not empty
    CU_ASSERT_PTR_NOT_NULL(iter);

    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);
}

void test_iterator_next(void)
{
    // Creates list and fill with 10 element from 1 to 10
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    int elems = 10;
    fill_list(list, elems);

    bool error = false;

    // Create iterator
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Check if has next element for the next 9 elements
    for (int i = 1; i < elems; i += 1)
    {
        CU_ASSERT_TRUE(ioopm_iterator_has_next(iter));
        ioopm_iterator_next(iter, &error);
    }

    // Check that there is no 11th element
    CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));

    // Frees everyting
    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);
}

void test_iterator_reset(void)
{
    // Creates list and fill with 10 element from 1 to 10
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    int elems = 10;
    fill_list(list, elems);

    bool error = false;

    // Create iterator and step forward 3 steps
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    for (int i = 1; i < 4; i += 1)
    {
        ioopm_iterator_next(iter, &error);
    }

    // Checks that the current value isnt the first value
    CU_ASSERT_FALSE(list->eq_fun(ioopm_iterator_current(iter), uns_int_elem(1)));

    // Reset and check that the current value is the first value.
    ioopm_iterator_reset(iter);
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), uns_int_elem(1)));

    // Frees everyting
    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);
}

void test_iterator_current(void)
{
    // Create list and fill list with 10 elements 1-10
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    fill_list(list, 10);

    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    bool error = false;

    // Count up 1, 2, 3, 4, 5 and compare with value in current node
    for (int i = 0; i < list->size - 1; i++)
    {
        CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), uns_int_elem(i + 1)));
        ioopm_iterator_next(iter, &error);
    }

    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);
}

void test_iterator_remove(void)
{
    // Create list and fill list with 10 elements 1-10
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    int elems = 10;
    fill_list(list, elems);

    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    bool error = false;

    // Remove removes first element
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_remove(iter), uns_int_elem(1)));

    // Step forward 3 steps
    for (int i = 1; i <= 3; i += 1)
    {
        ioopm_iterator_next(iter, &error);
    }
    // Removes 4th element in list(5)
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_remove(iter), uns_int_elem(5)));

    // Step forward to end
    for (int i = 1; i <= 4; i += 1)
    {
        ioopm_iterator_next(iter, &error);
    }

    // Removes last element in list(5)
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_remove(iter), uns_int_elem(10)));

    // Checks that the list is 7 elements big
    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), 7);

    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);
}

void test_iterator_insert(void)
{
    // Test values
    elem_t testfirst = {.uns_int = 666};
    elem_t testmiddle = {.uns_int = 777};
    elem_t testlast = {.uns_int = 888};

    // Create list and fill list with 10 elements 1-10
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    int elems = 10;
    fill_list(list, elems);

    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
    bool error = false;

    // Insert first and test
    ioopm_iterator_insert(iter, testfirst);
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), testfirst));
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_next(iter, &error), uns_int_elem(1)));

    // Step forward to middle
    for (int i = 1; i < 5; i += 1)
    {
        ioopm_iterator_next(iter, &error);
    }

    // Insert middle and test
    ioopm_iterator_insert(iter, testmiddle);
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), testmiddle));
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_next(iter, &error), uns_int_elem(5)));

    // Step forward to end
    for (int i = 5; i < 10; i += 1)
    {
        ioopm_iterator_next(iter, &error);
    }
    // Insert last and test
    ioopm_iterator_insert(iter, testlast);
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), testlast));
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_next(iter, &error), uns_int_elem(10)));
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), list->last->value));

    // Checks that size is updated, i.e size 13
    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), 13);

    // ioopm_iterator_next(iter);

    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);
}

void test_iterator_insert_empty(void)
{
    // Create empty list
    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);

    ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

    // Insert first element
    elem_t testfirst = {.uns_int = 1};
    ioopm_iterator_insert(iter, testfirst);
    CU_ASSERT_TRUE(list->eq_fun(ioopm_iterator_current(iter), testfirst));
    CU_ASSERT_EQUAL(ioopm_linkedlist_size(list), 1);

    ioopm_iterator_destroy(iter);
    ioopm_linkedlist_destroy(list);

}

// Tests bubblesort

int intcomp(elem_t x, elem_t y)
{
    return x.i > y.i;
}

int strcomp(elem_t x, elem_t y)
{
    return strcmp((char *)x.p, (char *)y.p) > 0;
}

void test_bubblesort(void)
{
    bool error = false;

    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    ioopm_linkedlist_append(list, int_elem(2));
    ioopm_linkedlist_append(list, int_elem(5));
    ioopm_linkedlist_append(list, int_elem(4));
    ioopm_linkedlist_append(list, int_elem(1));
    ioopm_linkedlist_append(list, int_elem(3));

    for (size_t i = 0; i < 5; i += 1)
    {
        CU_ASSERT_NOT_EQUAL(ioopm_linkedlist_get(list, i, &error).i, i + 1);
    }

    ioopm_linkedlist_bubblesort(list, intcomp);

    for (size_t i = 0; i < 5; i += 1)
    {
        CU_ASSERT_EQUAL(ioopm_linkedlist_get(list, i, &error).i, i + 1);
    }

    ioopm_linkedlist_destroy(list);
}

void test_bubblesort_strings(void)
{
    bool error = false;

    ioopm_list_t *list = ioopm_linkedlist_create(ioopm_unsint_equal);
    char *teststrings[5] = {"A", "CD", "a", "b", "cd"};

    ioopm_linkedlist_append(list, ptr_elem(teststrings[3]));
    ioopm_linkedlist_append(list, ptr_elem(teststrings[2]));
    ioopm_linkedlist_append(list, ptr_elem(teststrings[0]));
    ioopm_linkedlist_append(list, ptr_elem(teststrings[4]));
    ioopm_linkedlist_append(list, ptr_elem(teststrings[1]));

    for (size_t i = 0; i < 5; i += 1)
    {
        char *str = (char *)ioopm_linkedlist_get(list, i, &error).p;
        CU_ASSERT_FALSE(strcmp(str, teststrings[i]) == 0);
    }

    ioopm_linkedlist_bubblesort(list, strcomp);

    for (size_t i = 0; i < 5; i += 1)
    {
        char *str = (char *)ioopm_linkedlist_get(list, i, &error).p;
        CU_ASSERT_TRUE(strcmp(str, teststrings[i]) == 0);
    }

    ioopm_linkedlist_destroy(list);
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
    CU_pSuite ioopm_linkedlists_testsuite = CU_add_suite("Linked list test suite", init_suite, clean_suite);
    CU_pSuite ioopm_iterator_testsuite = CU_add_suite("Iterator test suite", init_suite, clean_suite);
    if (ioopm_linkedlists_testsuite == NULL)
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
        // Linked list testsuite
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: create and destroy", test_create_destroy) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Get first item", test_get_first) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Get empty return false", test_get_empty) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Append once", test_append_once) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Append several, and get middle item", test_append_several) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Prepend once", test_prepend_once) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Prepend several", test_prepend_several) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Any on single element list", test_any_single) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Any on several element list", test_any_several) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Contains on single element list", test_contains_single) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Contains on several element list", test_contains_several) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Contains on empty list", test_contains_empty) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: All on single element list", test_all_single) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: All on several element list", test_all_several) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: All on empty list", test_all_empty) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Insert once", test_insert_once) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Insert several", test_insert_several) == NULL) ||
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Remove once", test_remove_once) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Remove several", test_remove_several) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: remove out of bounds", test_remove_outofbounds) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Apply all", test_applyall) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: Size, original", test_size) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: bubblesort", test_bubblesort) == NULL) || 
        (CU_add_test(ioopm_linkedlists_testsuite, "Test linked list: bubblesort strings", test_bubblesort_strings) == NULL) || 
        // Iterator testsuite
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_create_destroy) == NULL) || 
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_current) == NULL) || 
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_next) == NULL) || 
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_reset) == NULL) || 
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_remove) == NULL) ||
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_insert) == NULL) || 
        (CU_add_test(ioopm_iterator_testsuite, "Test Iterator", test_iterator_insert_empty) == NULL) || 
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

#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "../src/refmem.h"
#include "../src/refmem_testing.h"

struct cell
{
    struct cell *cell;
    int i;
    char *string;
};

int init_suite(void)
{
    // Change this function if you want to do something *before* you
    // run a test suite
    return 0;
}

void cell_destructor(obj *c)
{
    release(((struct cell *)c)->cell);
}

int clean_suite(void)
{
    return 0;
}

void test_set_get_cascade_limit(void)
{
    // The cascade limit starts out at SIZE_MAX, which is functionally unlimited
    CU_ASSERT_EQUAL(get_cascade_limit(), SIZE_MAX);

    // TODO: decide what to do with a zero cascade limit, as it
    // would render the garbage collection totally inoperable...
    set_cascade_limit(0);
    CU_ASSERT_EQUAL(get_cascade_limit(), 0);

    set_cascade_limit(1);
    CU_ASSERT_EQUAL(get_cascade_limit(), 1);

    set_cascade_limit(9);
    CU_ASSERT_EQUAL(get_cascade_limit(), 9);

    set_cascade_limit(1000000);
    CU_ASSERT_EQUAL(get_cascade_limit(), 1000000);

    set_cascade_limit(12345);
    CU_ASSERT_EQUAL(get_cascade_limit(), 12345);

    // 2^32
    set_cascade_limit(UINT32_MAX);
    CU_ASSERT_EQUAL(get_cascade_limit(), UINT32_MAX);

    set_cascade_limit(SIZE_MAX);
    CU_ASSERT_EQUAL(get_cascade_limit(), SIZE_MAX);

    set_cascade_limit(SIZE_MAX - 1);
    CU_ASSERT_EQUAL(get_cascade_limit(), SIZE_MAX - 1);

    // 2^33, Might not work on 32-bit systems
    set_cascade_limit(8589934592UL);
    CU_ASSERT_EQUAL(get_cascade_limit(), 8589934592UL);

    shutdown();
}

void test_allocate_array(void)
{
    struct cell *c = allocate_array(10, sizeof(struct cell), cell_destructor);
    retain(c);
    c->cell = allocate_array(35, sizeof(struct cell), cell_destructor);
    retain(c->cell);
    CU_ASSERT_PTR_NOT_NULL(c);
    CU_ASSERT_PTR_NOT_NULL(c->cell);
    release(c);

    // Trying to allocate an array with a total size larger than SIZE_MAX should fail and return NULL
    CU_ASSERT_PTR_NULL(allocate_array(SIZE_MAX, SIZE_MAX, NULL));
    CU_ASSERT_PTR_NULL(allocate_array(SIZE_MAX / 2, SIZE_MAX / 2, NULL));
    CU_ASSERT_PTR_NOT_NULL(allocate_array(0, SIZE_MAX / 2, NULL));

    shutdown();
}

void test_allocate_deallocate(void)
{
    struct cell *c = allocate(sizeof(struct cell), cell_destructor);
    retain(c);
    c->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell);
    CU_ASSERT_PTR_NOT_NULL(c);
    CU_ASSERT_PTR_NOT_NULL(c->cell);

    /*Release calls on deallocate*/
    release(c);

    /* Test that allocate create object_list if it was NULL prev */
    allocate(sizeof(int), NULL);
}

void test_rc(void)
{
    struct test_object *obj = allocate(8, NULL);

    CU_ASSERT_EQUAL(rc(obj), 0);

    retain(obj);
    CU_ASSERT_EQUAL(rc(obj), 1);

    for (int i = 0; i < 1000; i++)
    {
        retain(obj);
    }
    CU_ASSERT_EQUAL(rc(obj), 1001);

    CU_ASSERT_FALSE(rc(NULL));

    shutdown();
}

void test_retain(void)
{
    obj *object = allocate(8, NULL);
    retain(object);
    CU_ASSERT_EQUAL(rc(object), 1);

    for (int i = 0; i < 1000; i++)
    {
        retain(object);
    }
    CU_ASSERT_EQUAL(rc(object), 1001);

    cleanup();

    shutdown();
}

void test_release(void)
{
    struct cell *c = allocate(sizeof(struct cell), cell_destructor);

    // If reference count is 0 it will remain 0 after release
    CU_ASSERT_EQUAL(rc(c), 0);

    retain(c);
    retain(c);
    c->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell);

    CU_ASSERT_EQUAL(rc(c), 2);
    CU_ASSERT_EQUAL(rc(c->cell), 1);

    // If reference count > 1 release decreases it by 1
    release(c);
    CU_ASSERT_EQUAL(rc(c), 1);
    CU_ASSERT_EQUAL(rc(c->cell), 1);

    /* If reference count is 1 release deallocates the object (since
    cell_destructor calls release on c->cell and the reference count of
    c->cell is also 1, c->cell is deallocated as well), no memory leaks */
    release(c);

    shutdown();
}

void test_cleanup(void)
{
    // Test for a lot of allocations
    for (int i = 0; i < 100; i++)
    {
        struct cell *c = allocate_array(i, sizeof(struct cell), cell_destructor);
        retain(c);
    }
    shutdown();
    /* Test cleanup when rc count is 0 */
    allocate(4, NULL);
    allocate(5, NULL);
    cleanup();

}

void test_shutdown(void)
{
    /* Test for a lot of allocations */
    for (int i = 0; i < 100; i++)
    {
        obj *object = allocate(i, NULL);
        retain(object);
    }
    obj *test = allocate(5, NULL);
    CU_ASSERT_PTR_NOT_NULL(test);
    /* Cleanup, no memory leaks */
    shutdown();

    /* Should be able to shutdown multiple times without consequences */
    shutdown();

    /* Test that shutdown destroys ptr_list correctly */
    obj *ptr_add = allocate(5, NULL);
    retain(ptr_add);
    shutdown();
}

void cascade_test(void)
{
    struct cell *c = allocate(sizeof(struct cell), cell_destructor);
    retain(c);
    c->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell);
    c->cell->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell->cell);
    c->cell->cell->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell->cell->cell);
    c->cell->cell->cell->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell->cell->cell->cell);
    c->cell->cell->cell->cell->cell = allocate(sizeof(struct cell), cell_destructor);
    retain(c->cell->cell->cell->cell->cell);

    set_cascade_limit(1);
    struct cell *next_cell = c->cell;
    release(c);

    /*Without changing the cascade limit this test leaks memory, meaning it was not deallocated due to respecting the cascade limit as we want!*/
    set_cascade_limit(SIZE_MAX);
    release(next_cell);

    CU_ASSERT_PTR_NOT_NULL(c);
    CU_ASSERT_PTR_NOT_NULL(next_cell);

    shutdown();
}

void test_default_destructor(void)
{

    // Test 0
    struct cell *c = allocate(sizeof(struct cell), NULL);
    retain(c);

    c->cell = allocate(sizeof(struct cell), NULL);
    retain(c->cell);

    c->cell->string = allocate_array(4, sizeof(char), NULL);
    retain(c->cell->string);

    c->string = allocate_array(7, sizeof(char), NULL);
    retain(c->string);
    // heello
    c->string[0] = 'h';
    c->string[1] = 'e';
    c->string[2] = 'e';
    c->string[3] = 'l';
    c->string[4] = 'l';
    c->string[5] = 'o';
    c->string[6] = '\0';

    release(c);
    // Test 1
    struct cell *c1 = allocate(sizeof(struct cell), NULL);
    retain(c1);

    c1->cell = allocate(sizeof(struct cell), NULL);
    retain(c1->cell);

    c1->cell->string = allocate_array(4, sizeof(char), NULL);
    retain(c1->cell->string);

    c1->string = allocate_array(2, sizeof(char), NULL);
    retain(c1->string);
    // heello
    c1->string[0] = 'h';
    c1->string[1] = '\0';

    // Test 2
    struct cell *c2 = allocate(sizeof(struct cell), NULL);
    retain(c2);

    c2->cell = allocate(sizeof(struct cell), NULL);
    retain(c2->cell);

    c2->cell->string = allocate_array(4, sizeof(char), NULL);
    retain(c2->cell->string);

    c2->string = allocate_array(13, sizeof(char), NULL);
    retain(c2->string);
    // heelloheello
    c2->string[0] = 'h';
    c2->string[1] = 'e';
    c2->string[2] = 'e';
    c2->string[3] = 'l';
    c2->string[4] = 'l';
    c2->string[5] = 'o';
    c2->string[6] = 'h';
    c2->string[7] = 'e';
    c2->string[8] = 'e';
    c2->string[9] = 'l';
    c2->string[10] = 'l';
    c2->string[11] = 'o';
    c2->string[12] = '\0';

    // Test 3
    struct cell *c3 = allocate(sizeof(struct cell), NULL);
    retain(c3);

    c3->cell = allocate(sizeof(struct cell), NULL);
    retain(c3->cell);

    c3->cell->string = allocate_array(4, sizeof(char), NULL);
    retain(c3->cell->string);

    c3->string = allocate_array(7, sizeof(char), NULL);
    retain(c3->string);
    // heello
    c3->string[0] = 'h';
    c3->string[1] = 'e';
    c3->string[2] = 'e';
    c3->string[3] = 'l';
    c3->string[4] = 'l';
    c3->string[5] = 'o';
    c3->string[6] = '\0';

    release(c2);
    release(c1);
    release(c3);

   if (ptr_list != NULL) {
    ref_linked_list_destroy(ptr_list);
   }

   default_destructor(NULL);
}

void test_cascade_limit_variable(void)
{
    // Check that set_cascade_limit is setting the variable correctly
    set_cascade_limit(0);
    CU_ASSERT_EQUAL(cascade_limit, 0);

    set_cascade_limit(1);
    CU_ASSERT_EQUAL(cascade_limit, 1);

    set_cascade_limit(9);
    CU_ASSERT_EQUAL(cascade_limit, 9);

    set_cascade_limit(1000000);
    CU_ASSERT_EQUAL(cascade_limit, 1000000);

    set_cascade_limit(12345);
    CU_ASSERT_EQUAL(cascade_limit, 12345);

    // 2^32
    set_cascade_limit(UINT32_MAX);
    CU_ASSERT_EQUAL(cascade_limit, UINT32_MAX);

    set_cascade_limit(SIZE_MAX);
    CU_ASSERT_EQUAL(cascade_limit, SIZE_MAX);

    set_cascade_limit(SIZE_MAX - 1);
    CU_ASSERT_EQUAL(cascade_limit, SIZE_MAX - 1);

    // 2^33, Might not work on 32-bit systems
    set_cascade_limit(8589934592UL);
    CU_ASSERT_EQUAL(cascade_limit, 8589934592UL);

    shutdown();
}

void test_eq_func(void)
{
    obj *object_1 = allocate(8, NULL);
    retain(object_1);
    obj *object_3 = object_1;
    retain(object_3);
    obj *object_2 = allocate(8, NULL);
    retain(object_2);
    CU_ASSERT_TRUE(eq_fun((ref_elem_t) { .p = object_1 }, (ref_elem_t) { .p = object_1 }));
    CU_ASSERT_TRUE(eq_fun((ref_elem_t) { .p = object_1 }, (ref_elem_t) { .p = object_3 }));
    CU_ASSERT_FALSE(eq_fun((ref_elem_t) { .p = object_1 }, (ref_elem_t) { .p = object_2 }));

    release(object_1);
    release(object_2);
    release(object_3);
    shutdown();
}

void test_get_struct_index(void)
{
    //If we have not allocated something the object list is NULL and we should get false.
    CU_ASSERT_PTR_NULL(object_list);
    int index = -1;
    CU_ASSERT_FALSE(get_struct_index(NULL, &index));

    obj *object_1 = allocate(8, NULL);
    retain(object_1);
    obj *object_2 = allocate(8, NULL);
    retain(object_2);

    CU_ASSERT_TRUE(get_struct_index(object_1, &index));
    CU_ASSERT_EQUAL(index, 0);
    CU_ASSERT_TRUE(get_struct_index(object_2, &index));
    CU_ASSERT_EQUAL(index, 1);

    release(object_1);
    release(object_1);

    CU_ASSERT_TRUE(get_struct_index(object_2, &index));
    CU_ASSERT_EQUAL(index, 0);

    shutdown();

    CU_ASSERT_FALSE(get_struct_index(NULL, &index));
    obj *object_10 = allocate(8, NULL);
    shutdown();
    CU_ASSERT_FALSE(get_struct_index(object_10, &index));
    add_ptr_to_memory(NULL);

}

void test_get_struct(void)
{
    CU_ASSERT_PTR_NULL(get_struct(NULL));

    obj *object_1 = allocate(8, NULL);
    retain(object_1);
    obj *object_2 = allocate(12, NULL);
    retain(object_2);
    retain(object_2);
    retain(object_2);
    struct cell *object_3 = allocate(sizeof(struct cell), cell_destructor);
    retain(object_3);
    retain(object_3);

    // a NULL object should still result in a NULL struct
    CU_ASSERT_PTR_NULL(get_struct(NULL));

    object_t* struct1 = get_struct(object_1);
    CU_ASSERT_EQUAL(struct1->destructor, default_destructor);
    CU_ASSERT_EQUAL(struct1->rc, 1);
    CU_ASSERT_EQUAL(struct1->object, object_1);
    CU_ASSERT_EQUAL(struct1->size, 8);

    object_t* struct2 = get_struct(object_2);
    CU_ASSERT_EQUAL(struct2->destructor, default_destructor);
    CU_ASSERT_EQUAL(struct2->rc, 3);
    CU_ASSERT_EQUAL(struct2->object, object_2);
    CU_ASSERT_EQUAL(struct2->size, 12);

    object_t* struct3 = get_struct(object_3);
    CU_ASSERT_EQUAL(struct3->destructor, cell_destructor);
    CU_ASSERT_EQUAL(struct3->rc, 2);
    CU_ASSERT_EQUAL(struct3->object, object_3);
    CU_ASSERT_EQUAL(struct3->size, sizeof(struct cell));

    release(object_1);
    release(object_2);
    release(object_2);

    // object 1 has been deleted so get_struct should return NULL
    CU_ASSERT_PTR_NULL(get_struct(object_1));

    struct2 = get_struct(object_2);
    CU_ASSERT_EQUAL(struct2->destructor, default_destructor);
    CU_ASSERT_EQUAL(struct2->rc, 1); // rc should be reduced to 1
    CU_ASSERT_EQUAL(struct2->object, object_2);
    CU_ASSERT_EQUAL(struct2->size, 12);

    release(object_2);

    // object 2 has now also been deleted
    CU_ASSERT_PTR_NULL(get_struct(object_1));
    CU_ASSERT_PTR_NULL(get_struct(object_2));

    release(object_3);
    release(object_3);

    CU_ASSERT_PTR_NULL(get_struct(object_3));

    shutdown();
}

void test_remove_ptr_list_null(void) {
    CU_ASSERT_PTR_NULL(ptr_list);

    int test[5];
    remove_ptr_from_memory(&test);
    shutdown();
}

int main(void)
{
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
    {
        return CU_get_error();
    }

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("Refmem test suite", init_suite, clean_suite);
    if (!my_test_suite)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // This is where we add the test functions to our test suite.
    // For each call to CU_add_test we specify the test suite, the
    // name or description of the test, and the function that runs
    // the test in question. If you want to add another test, just
    // copy a line below and change the information
    if (
        !CU_add_test(my_test_suite, "Set and get the cascade limit", test_set_get_cascade_limit)
        || !CU_add_test(my_test_suite, "Test allocate_array", test_allocate_array)
        || !CU_add_test(my_test_suite, "test allocation of memory", test_allocate_deallocate)
        || !CU_add_test(my_test_suite, "test rc()", test_rc)
        || !CU_add_test(my_test_suite, "Test retain", test_retain)
        || !CU_add_test(my_test_suite, "Test release", test_release)
        || !CU_add_test(my_test_suite, "Test cleanup", test_cleanup)
        || !CU_add_test(my_test_suite, "Test shutdown", test_shutdown)
        || !CU_add_test(my_test_suite, "Test cascade respect", cascade_test)
        || !CU_add_test(my_test_suite, "Test cascade_limit variable", test_cascade_limit_variable)
        || !CU_add_test(my_test_suite, "Test equaltiy function", test_eq_func)
        || !CU_add_test(my_test_suite, "Test get struct index function", test_get_struct_index)
        || !CU_add_test(my_test_suite, "Test get struct", test_get_struct)
        || !CU_add_test(my_test_suite, "Test default destructor", test_default_destructor)
        || !CU_add_test(my_test_suite, "Test remove ptr from null ptr_list", test_remove_ptr_list_null)
    ) {
        // If adding any of the tests fails, we tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_VERBOSE);

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

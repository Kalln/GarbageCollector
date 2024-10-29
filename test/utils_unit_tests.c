
#define _XOPEN_SOURCE   700
#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../demo/utils.h"
#include "../src/refmem.h"

int init_suite(void) { return 0; }

int clean_suite(void) { return 0; }

// Tests goes here

void test_is_number(void)
{
    CU_ASSERT_TRUE(is_number("-666"));
    CU_ASSERT_TRUE(is_number("-1"));
    CU_ASSERT_TRUE(is_number("-1"));
    CU_ASSERT_TRUE(is_number("0"));
    CU_ASSERT_TRUE(is_number("666"));

    CU_ASSERT_FALSE(is_number("-"));
    CU_ASSERT_FALSE(is_number("a"));
    CU_ASSERT_FALSE(is_number("A"));
    CU_ASSERT_FALSE(is_number("."));
}

void test_ask_question_int(void)
{
    FILE *file = fopen("test/test_question_int.txt", "w");

    // wrong inputs
    fputs("a\n", file);
    fputs("-\n", file);
    fputs("A\n", file);
    fputs(".\n", file);
    fputs("*\n", file);

    // Correct inputs
    fputs("0\n", file);
    fputs("1\n", file);

    fclose(file);

    freopen("test/test_question_int.txt", "r", stdin);

    CU_ASSERT_EQUAL(ask_question_int("0\n"), 0);
    CU_ASSERT_EQUAL(ask_question_int("1\n"), 1);

    return;
}

void test_ask_question_string(void)
{

    FILE *file = fopen("test/test_question_string.txt", "w");

    // wrong inputs
    fputs("\n", file);

    // Correct inputs
    fputs("This is a test sentence\n", file);
    fputs("testword\n", file);
    fputs("123456789\n", file);

    fclose(file);

    freopen("test/test_question_string.txt", "r", stdin);

    char *string1 = ask_question_string("Test sentence\n");
    char *string2 = ask_question_string("Test word\n");
    char *string3 = ask_question_string("Test num\n");

    CU_ASSERT_STRING_EQUAL(string1, "This is a test sentence");
    CU_ASSERT_STRING_EQUAL(string2, "testword");
    CU_ASSERT_STRING_EQUAL(string3, "123456789");

    release(string1);
    release(string2);
    release(string3);
}

void test_ask_question_float(void)
{
    FILE *file = fopen("test/test_question_float.txt", "w");

    // wrong inputs
    fputs("", file);
    fputs("Foo", file);
    fputs("Foo Bar Baz", file);
    fputs(".", file);
    fputs("\n", file);

    // Correct inputs
    fputs("0\n", file);
    fputs("1234\n", file);
    fputs("1234.0\n", file);
    fputs("1234.1\n", file);

    fclose(file);

    freopen("test/test_question_float.txt", "r", stdin);

    CU_ASSERT_EQUAL(ask_question_float("0"), 0);
    CU_ASSERT_EQUAL(ask_question_float("1234"), 1234);
    CU_ASSERT_EQUAL(ask_question_float("1234.0"), 1234.0);
    CU_ASSERT_EQUAL(ask_question_float("1234.1"), 1234.1);
}

void test_ask_question_long(void)
{
    FILE *file = fopen("test/test_question_int.txt", "w");

    // wrong inputs
    fputs("a\n", file);
    fputs("-\n", file);
    fputs("A\n", file);
    fputs(".\n", file);
    fputs("*\n", file);

    // Correct inputs
    fputs("9223372036854775807\n", file);
    fputs("1\n", file);
    fputs("0\n", file);
    fputs("-9223372036854775807\n", file);

    fclose(file);

    freopen("test/test_question_int.txt", "r", stdin);

    long max = 9223372036854775807;
    long min = -9223372036854775807;

    CU_ASSERT_EQUAL(ask_question_long("9223372036854775807\n"), max);
    CU_ASSERT_EQUAL(ask_question_int("1\n"), 1);
    CU_ASSERT_EQUAL(ask_question_long("0\n"), 0);
    CU_ASSERT_EQUAL(ask_question_long("-9223372036854775807\n"), min);

    return;
}

void test_ask_question_char(void)
{
    char *inputfile = "test/test_question_char.txt";
    FILE *file = fopen(inputfile, "w");

    // wrong inputs
    fputs("1\n", file);
    fputs("0\n", file);
    fputs("foo\n", file);
    fputs("Bar\n", file);
    fputs("f\n", file);
    fputs("B\n", file);
    fputs("+\n", file);
    fputs("-\n", file);
    fputs(".\n", file);

    fclose(file);

    freopen(inputfile, "r", stdin);

    CU_ASSERT_EQUAL(ask_question_char("f: \n"), 'f');
    CU_ASSERT_EQUAL(ask_question_char("B: \n"), 'B');
    CU_ASSERT_EQUAL(ask_question_char("+: \n"), '+');
    CU_ASSERT_EQUAL(ask_question_char("-: \n"), '-');
    CU_ASSERT_EQUAL(ask_question_char(".: \n"), '.');

    return;
}

int main(void)
{
    // Exit if fail to setup CUnit
    if (CU_initialize_registry() != CUE_SUCCESS)
    {
        return CU_get_error();
    }

    // Create empty test suite
    CU_pSuite ioopm_utils_test_suite =
        CU_add_suite("utils test suite", init_suite, clean_suite);
    if (ioopm_utils_test_suite == NULL)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        // Put tests here
        // clang-format off

        (CU_add_test(ioopm_utils_test_suite, "tests is_number()", test_is_number) == NULL) ||
        (CU_add_test(ioopm_utils_test_suite, "tests ask_question_int()", test_ask_question_int) == NULL) ||
        (CU_add_test(ioopm_utils_test_suite, "tests ask_question_string()", test_ask_question_string) == NULL) ||
        (CU_add_test(ioopm_utils_test_suite, "tests ask_question_float()", test_ask_question_float) == NULL) ||
        (CU_add_test(ioopm_utils_test_suite, "tests ask_question_long()", test_ask_question_long) == NULL) ||
        (CU_add_test(ioopm_utils_test_suite, "tests ask_question_char()", test_ask_question_char) == NULL) ||
        0)//clang-format on
    {
        // If adding any of the tests fails, we tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_NORMAL);

    CU_basic_run_tests();

    // Get the number of failed asserts
    unsigned int failures = CU_get_number_of_failures();

    // Tear down CUnit before exiting
    CU_cleanup_registry();

    return failures > 0
        ? 1
        : 0;
}

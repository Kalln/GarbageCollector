
#define _XOPEN_SOURCE   700
#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../demo/store_logic.h"

int init_suite(void) { return 0; }

int clean_suite(void) { return 0; }

// Tests goes here

FILE *create_file_stdin(char *filename, char *stringarray[], size_t strlen)
{
    FILE *fp = fopen(filename, "w");

    for (size_t i = 0; i < strlen; i += 1)
    {
        fputs(strcat(stringarray[i], "\n"), fp);
    }

    fclose(fp);

    FILE *fpout = freopen(filename, "r", stdin);

    return fpout;
}

FILE *create_file_stdout(char *filename)
{
    FILE *fp = freopen(filename, "w", stdout);
    return fp;
}

bool file_is_written(char *filename)
{
    FILE *fp = fopen(filename, "r");

    if ((fp) != NULL)
    {
        fseek(fp, 0L, SEEK_END);
        if (ftell(fp) > 0)
        {
            return true;
        }
    }

    return false;
}

// Takes an array of [item, desc, shelf] and stocks the item at shelf
char *helper_add_merch_to_store(ioopm_store_t *store, char *item[3], int cost, int amount)
{
    char *repl_item = item[0];
    ioopm_add_merch_to_store(store, item[0], item[1], cost);
    ioopm_replenish_item(store, repl_item, item[2], amount);
    return repl_item;
}

void test_store_create(void)
{
    ioopm_store_t *store = ioopm_store_create();

    CU_ASSERT_TRUE(ioopm_store_is_empty(store));
    CU_ASSERT_PTR_NOT_NULL(store);

    ioopm_store_destroy(store);
}

void test_is_merch(void)
{
    ioopm_store_t *store = ioopm_store_create();

    ioopm_add_merch_to_store(store, "Foo", "Bar bz", 1337);
    ioopm_add_merch_to_store(store, "Kokos", "Nötter", 666);

    CU_ASSERT_TRUE(ioopm_is_merch(store, "Foo"));
    CU_ASSERT_TRUE(ioopm_is_merch(store, "Kokos"));

    CU_ASSERT_FALSE(ioopm_is_merch(store, "Banan"));
    CU_ASSERT_FALSE(ioopm_is_merch(store, "2133"));

    ioopm_store_destroy(store);
}

void test_add_merch_to_store(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *item1[] = {"Foo", "Bar baz"};
    char *item2[] = {"Kokos", "Nötter"};

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);
    ioopm_add_merch_to_store(store, item2[0], item2[1], 666);

    CU_ASSERT_TRUE(ioopm_is_merch(store, item1[0]));
    merch_t *merch1 = ioopm_get_merch(store, item1[0]);
    CU_ASSERT_TRUE(strcmp(ioopm_get_merch_name(merch1), item1[0]) == 0);
    CU_ASSERT_TRUE(strcmp(ioopm_get_merch_desc(merch1), item1[1]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_price(merch1), 1337);

    CU_ASSERT_TRUE(ioopm_is_merch(store, item2[0]));
    merch_t *merch2 = ioopm_get_merch(store, item2[0]);
    CU_ASSERT_TRUE(strcmp(ioopm_get_merch_name(merch2), item2[0]) == 0);
    CU_ASSERT_TRUE(strcmp(ioopm_get_merch_desc(merch2), item2[1]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_price(merch2), 666);

    ioopm_store_destroy(store);
}

void test_remove_merch(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *item1[] = {"Foo", "Bar baz"};
    //char *item2[] = {"Kokos", "Nötter"};

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);
    //ioopm_add_merch_to_store(store, item2[0], item2[1], 666);

    ioopm_merch_remove(store, item1[0]);
    //CU_ASSERT_FALSE(ioopm_is_merch(store, item1[0]));
    //CU_ASSERT_TRUE(ioopm_is_merch(store, item2[0]));

    //ioopm_merch_remove(store, item2[0]);
    //CU_ASSERT_FALSE(ioopm_is_merch(store, item2[0]));

    ioopm_store_destroy(store);
}

void test_edit_merch(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *item1[] = {"Foo", "Bar baz"};
    char *item2[] = {"Kokos", "Nötter"};

    char *new_item1[] = {"Fuu", "Bur buz"};

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);
    ioopm_add_merch_to_store(store, item2[0], item2[1], 666);

    CU_ASSERT_TRUE(ioopm_is_merch(store, item1[0]));

    char *lookup = item1[0];
    ioopm_merch_edit(store, lookup, new_item1[0], new_item1[1], 3713);

    CU_ASSERT_FALSE(ioopm_is_merch(store, item1[0]));
    CU_ASSERT_TRUE(ioopm_is_merch(store, new_item1[0]));

    ioopm_store_destroy(store);
}

void test_empty_stores_replenish(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *item1[] = {"Foo", "Bar baz", "A1"};

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);

    // True if shelf dont exists
    char *item_to_replenish = item1[0];
    CU_ASSERT_TRUE(ioopm_replenish_item(store, item_to_replenish, item1[2], 10));

    shelf_t *shelf = ioopm_get_shelf(store, item1[2]);
    char *shelfname = ioopm_get_shelf_name(shelf);
    CU_ASSERT_TRUE(strcmp(shelfname, item1[2]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelf), 10);

    CU_ASSERT_FALSE(strcmp(ioopm_get_shelf_name(shelf), "foo") == 0);
    CU_ASSERT_NOT_EQUAL(ioopm_get_shelf_amount(shelf), 0);

    ioopm_store_destroy(store);
}

void test_shelves_replenish(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Kokos", "Nutter", "B5"};
    char *repl_item1 = item1[0];
    char *repl_item2 = item2[0];

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);
    ioopm_add_merch_to_store(store, item2[0], item2[1], 123);

    // True if shelf dont exists
    CU_ASSERT_TRUE(ioopm_replenish_item(store, repl_item1, item1[2], 10));
    CU_ASSERT_TRUE(ioopm_replenish_item(store, repl_item2, item2[2], 1));

    shelf_t *shelf1 = ioopm_get_shelf(store, item1[2]);
    CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(shelf1), item1[2]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelf1), 10);

    shelf_t *shelf2 = ioopm_get_shelf(store, item2[2]);
    CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(shelf2), item2[2]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelf2), 1);

    ioopm_store_destroy(store);
}

void test_replenish_different(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *shelves[] = {"A1", "B3", "C6"};
    char *item1[] = {"Foo", "Bar baz"};
    char *item_to_replenish = item1[0];
    bool error;

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);

    // True if shelf dont exists
    CU_ASSERT_TRUE(ioopm_replenish_item(store, item_to_replenish, shelves[0], 1));
    CU_ASSERT_TRUE(ioopm_replenish_item(store, item_to_replenish, shelves[1], 2));
    CU_ASSERT_TRUE(ioopm_replenish_item(store, item_to_replenish, shelves[2], 3));

    shelf_t *shelfa1 = ioopm_get_shelf(store, shelves[0]);
    shelf_t *shelfb3 = ioopm_get_shelf(store, shelves[1]);
    shelf_t *shelfc6 = ioopm_get_shelf(store, shelves[2]);

    // tests hashtable functions
    CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(shelfa1), shelves[0]) == 0);
    CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(shelfb3), shelves[1]) == 0);
    CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(shelfc6), shelves[2]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelfa1), 1);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelfb3), 2);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelfc6), 3);

    // test item lists functions
    ioopm_list_t *locations = NULL;
    ioopm_get_merch_locations(ioopm_get_merch(store, item1[0]), &locations);

    for (int i = 0; i < 3; i += 1)
    {

        shelf_t *current_shelf = ioopm_linkedlist_get(locations, i, &error).p;
        CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(current_shelf), shelves[i]) == 0);
        CU_ASSERT_EQUAL(ioopm_get_shelf_amount(current_shelf), i + 1);
    }

    ioopm_store_destroy(store);
}

void test_replenish_same(void)
{
    ioopm_store_t *store = ioopm_store_create();
    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Kokos", "Nutter", "B5"};
    char *repl_item1 = item1[0];
    char *repl_item2 = item2[0];
    char *repl_shelf1 = item1[2];   // As frontend free if location is busy

    ioopm_add_merch_to_store(store, item1[0], item1[1], 1337);
    ioopm_add_merch_to_store(store, item2[0], item2[1], 123);

    CU_ASSERT_TRUE(ioopm_replenish_item(store, repl_item1, item1[2], 10));
    CU_ASSERT_TRUE(ioopm_replenish_item(store, repl_item1, "A1", 1));
    CU_ASSERT_TRUE(ioopm_replenish_item(store, repl_item1, item1[2], 4));
    // Try to replenish same shelf with another item
    CU_ASSERT_FALSE(ioopm_replenish_item(store, repl_item2, repl_shelf1, 1));

    shelf_t *shelf1 = ioopm_get_shelf(store, item1[2]);
    CU_ASSERT_TRUE(strcmp(ioopm_get_shelf_name(shelf1), item1[2]) == 0);
    CU_ASSERT_EQUAL(ioopm_get_shelf_amount(shelf1), 15);

    ioopm_store_destroy(store);
}

void test_cart_add_remove(void)
{
    ioopm_store_t *store = ioopm_store_create();
    cart_t *cart = NULL;

    CU_ASSERT_FALSE(ioopm_is_cart(store, 0));
    CU_ASSERT_FALSE(ioopm_get_cart(store, 0, &cart));

    ioopm_add_cart_to_store(store);

    CU_ASSERT_TRUE(ioopm_is_cart(store, 0));
    CU_ASSERT_TRUE(ioopm_get_cart(store, 0, &cart));

    CU_ASSERT_TRUE(ioopm_remove_cart_from_store(store, 0));
    CU_ASSERT_FALSE(ioopm_remove_cart_from_store(store, 1));

    ioopm_store_destroy(store);
}

void test_cart_add_item(void)
{
    ioopm_store_t *store = ioopm_store_create();
    ioopm_add_cart_to_store(store);
    cart_t *cart = NULL;
    ioopm_get_cart(store, 0, &cart);

    //Test with merch that does not exist
    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 0, "Chocklad", 1));

    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Kokos", "Nutter", "B5"};

    helper_add_merch_to_store(store, item1, 666, 666);
    helper_add_merch_to_store(store, item2, 1337, 1337);

    //Test with cart that does not exist
    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 1, item1[0], 666));

    CU_ASSERT_TRUE(ioopm_add_to_cart(store, 0, item1[0], 666));

    //Should fail as stock should be empty
    CU_ASSERT_FALSE(ioopm_add_to_cart(store, 0, item1[0], 50));

    //Check that name and amount is correct
    CU_ASSERT_TRUE(ioopm_cart_has_ware(store, 0, item1[0]));
    CU_ASSERT_EQUAL(ioopm_get_ware_amount(store, 0, item1[0]), 666);

    //Check that the amount was reserved correctly
    merch_t *added_merch = ioopm_get_merch(store, item1[0]);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(added_merch), 666);

    ioopm_store_destroy(store);
}

void test_cart_remove_item(void)
{
    ioopm_store_t *store = ioopm_store_create();
    ioopm_add_cart_to_store(store);
    cart_t *cart = NULL;
    ioopm_get_cart(store, 0, &cart);

    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Kokos", "Nutter", "B5"};
    helper_add_merch_to_store(store, item1, 10, 10);
    helper_add_merch_to_store(store, item2, 20, 20);

    ioopm_add_to_cart(store, 0, item1[0], 10);

    merch_t *added_merch = ioopm_get_merch(store, item1[0]);

    //Test with merch that's not in the cart
    CU_ASSERT_FALSE(ioopm_remove_item_from_cart(store, 0, item2[0], 20));

    //Test with cart that does not exist
    CU_ASSERT_FALSE(ioopm_remove_item_from_cart(store, 1, item1[0], 10));

    CU_ASSERT_TRUE(ioopm_remove_item_from_cart(store, 0, item1[0], 5));
    CU_ASSERT_EQUAL(ioopm_get_ware_amount(store, 0, item1[0]), 5);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(added_merch), 5);

    //Test removing a higher quantity than what's in the cart removes cart and resets reservation
    CU_ASSERT_TRUE(ioopm_remove_item_from_cart(store, 0, item1[0], 20));
    CU_ASSERT_FALSE(ioopm_cart_has_ware(store, 0, item1[0]));

    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(added_merch), 0);

    ioopm_store_destroy(store);
}

void test_remove_item_from_store_in_cart(void)
{

    ioopm_store_t *store = ioopm_store_create();
    ioopm_add_cart_to_store(store);
    cart_t *cart = NULL;
    ioopm_get_cart(store, 0, &cart);

    char *item1[] = {"Foo", "Bar baz", "A1"};
    helper_add_merch_to_store(store, item1, 10, 10);

    ioopm_add_to_cart(store, 0, item1[0], 10);

    CU_ASSERT_FALSE(ioopm_merch_remove(store, item1[0]));
    CU_ASSERT_TRUE(ioopm_cart_has_ware(store, 0, item1[0]));

    ioopm_store_destroy(store);
}

void test_calculate_cost(void)
{
    ioopm_store_t *store = ioopm_store_create();
    ioopm_add_cart_to_store(store);
    cart_t *cart = NULL;
    ioopm_get_cart(store, 0, &cart);

    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Kokos", "Nutter", "B5"};
    helper_add_merch_to_store(store, item1, 50, 666);
    helper_add_merch_to_store(store, item2, 100, 1337);

    //Costs should be 0 for an empty catr
    CU_ASSERT_EQUAL(ioopm_calculate_cart_cost(store, 0), 0);
    //Should return 0 for a cart that does not exist
    CU_ASSERT_EQUAL(ioopm_calculate_cart_cost(store, 1), 0);

    ioopm_add_to_cart(store, 0, item1[0], 200);
    ioopm_add_to_cart(store, 0, item2[0], 1000);

    CU_ASSERT_EQUAL(ioopm_calculate_cart_cost(store, 0), 110000);

    ioopm_remove_item_from_cart(store, 0, item1[0], 200);

    CU_ASSERT_EQUAL(ioopm_calculate_cart_cost(store, 0), 100000);

    ioopm_store_destroy(store);
}

void test_edit_item_on_shelf(void)
{
    ioopm_store_t *store = ioopm_store_create();

    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Bar", "Foo baz", "A1"};
    helper_add_merch_to_store(store, item1, 10, 10);
    shelf_t *shelf = ioopm_get_shelf(store, item1[2]);

    CU_ASSERT_TRUE(ioopm_is_merch(store, item1[0]));
    CU_ASSERT_STRING_EQUAL(ioopm_get_shelf_itemname(shelf), item1[0]);
    ioopm_merch_edit(store, item1[0], item2[0], item2[1], 20);
    CU_ASSERT_FALSE(ioopm_is_merch(store, item1[0]));
    CU_ASSERT_TRUE(ioopm_is_merch(store, item2[0]));
    CU_ASSERT_STRING_EQUAL(ioopm_get_shelf_itemname(shelf), item2[0]);

    ioopm_store_destroy(store);
}

void test_edit_item_in_cart(void)
{
    ioopm_store_t *store = ioopm_store_create();

    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Bar", "Foo baz", "A1"};
    char *merchname = helper_add_merch_to_store(store, item1, 10, 10);
    cart_t *cart;
    size_t cartid = ioopm_add_cart_to_store(store);

    ioopm_add_to_cart(store, cartid, merchname, 3);
    ioopm_get_cart(store, cartid, &cart);

    CU_ASSERT_TRUE(ioopm_is_merch(store, item1[0]));
    ioopm_merch_edit(store, item1[0], item2[0], item2[1], 20);
    CU_ASSERT_FALSE(ioopm_is_merch(store, item1[0]));
    CU_ASSERT_TRUE(ioopm_is_merch(store, item2[0]));

    CU_ASSERT_FALSE(ioopm_cart_has_ware(store, cartid, item1[0]));
    CU_ASSERT_TRUE(ioopm_cart_has_ware(store, cartid, item2[0]));

    ioopm_store_destroy(store);
}

void test_checkout_single_ware_cart(void)
{
    ioopm_store_t *store = ioopm_store_create();

    int cost = 0;
    char *itemarr[] = {"Foo", "Bar baz", "A1"};
    char *merchname = helper_add_merch_to_store(store, itemarr, 10, 10);
    merch_t *merch = ioopm_get_merch(store, merchname);

    size_t cartid1 = ioopm_add_cart_to_store(store);
    size_t cartid2 = ioopm_add_cart_to_store(store);
    size_t emptycart = ioopm_add_cart_to_store(store);

    ioopm_add_to_cart(store, cartid1, merchname, 3);
    ioopm_add_to_cart(store, cartid2, merchname, 7);

    // Checkout nonexisting cart
    CU_ASSERT_FALSE(ioopm_checkout_cart(store, 10, &cost));

    // Checkout empty cart
    CU_ASSERT_TRUE(ioopm_checkout_cart(store, emptycart, &cost));
    CU_ASSERT_EQUAL(cost, 0);

    CU_ASSERT_TRUE(ioopm_checkout_cart(store, cartid1, &cost));
    CU_ASSERT_EQUAL(cost, 30);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch), 7);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch), 7);

    CU_ASSERT_TRUE(ioopm_checkout_cart(store, cartid2, &cost));
    CU_ASSERT_EQUAL(cost, 70);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch), 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch), 0);

    CU_ASSERT_FALSE(ioopm_is_shelf(store, itemarr[2]));

    ioopm_store_destroy(store);
}

void test_checkout_multi_ware_cart(void)
{
    ioopm_store_t *store = ioopm_store_create();

    // Three items for edgecases first, middle, last
    char *item1[] = {"Foo", "Bar baz", "A1"};
    char *item2[] = {"Kokos", "Nutter", "B5"};
    char *item3[] = {"Katt", "Mat", "C8"};
    char *repl_item1 = helper_add_merch_to_store(store, item1, 1, 3);
    char *repl_item2 = helper_add_merch_to_store(store, item2, 5, 10);
    char *repl_item3 = helper_add_merch_to_store(store, item3, 10, 10);
    merch_t *merch1 = ioopm_get_merch(store, repl_item1);
    merch_t *merch2 = ioopm_get_merch(store, repl_item2);
    merch_t *merch3 = ioopm_get_merch(store, repl_item3);

    char *extra_shelf = "D9";
    ioopm_replenish_item(store, repl_item3, extra_shelf, 10);

    size_t cart1 = ioopm_add_cart_to_store(store);
    size_t cart2 = ioopm_add_cart_to_store(store);
    int cost = 0;

    ioopm_add_to_cart(store, cart1, repl_item1, 1);
    ioopm_add_to_cart(store, cart2, repl_item1, 2);

    ioopm_add_to_cart(store, cart1, repl_item2, 3);
    ioopm_add_to_cart(store, cart2, repl_item2, 6);

    ioopm_add_to_cart(store, cart1, repl_item3, 11);
    ioopm_add_to_cart(store, cart2, repl_item3, 9);

    //Checkout cart 1
    CU_ASSERT_TRUE(ioopm_checkout_cart(store, cart1, &cost));
    CU_ASSERT_EQUAL(cost, 126);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch1), 2);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch1), 2);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch2), 7);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch2), 6);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch3), 9);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch3), 9);

    CU_ASSERT_TRUE(ioopm_is_shelf(store, item1[2]));
    CU_ASSERT_TRUE(ioopm_is_shelf(store, item2[2]));
    CU_ASSERT_FALSE(ioopm_is_shelf(store, item3[2]));
    CU_ASSERT_TRUE(ioopm_is_shelf(store, extra_shelf));

    //Checkout cart2
    CU_ASSERT_TRUE(ioopm_checkout_cart(store, cart2, &cost));
    CU_ASSERT_EQUAL(cost, 122);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch1), 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch1), 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch2), 1);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch2), 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_amount(merch3), 0);
    CU_ASSERT_EQUAL(ioopm_get_merch_reserved(merch3), 0);

    CU_ASSERT_FALSE(ioopm_is_shelf(store, item1[2]));
    CU_ASSERT_TRUE(ioopm_is_shelf(store, item2[2]));
    CU_ASSERT_FALSE(ioopm_is_shelf(store, item3[2]));
    CU_ASSERT_FALSE(ioopm_is_shelf(store, "D9"));

    ioopm_store_destroy(store);
}

int main(void)
{
    // Exit if fail to setup CUnit
    if (CU_initialize_registry() != CUE_SUCCESS)
    {
        return CU_get_error();
    }

    // Create empty test suite
    CU_pSuite ioopm_backend_test_suite =
        CU_add_suite("General test suite", init_suite, clean_suite);
    if (ioopm_backend_test_suite == NULL)
    {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        // Put tests here
        // clang-format off
        (CU_add_test(ioopm_backend_test_suite, "Test store create and destroy", test_store_create) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test is merch", test_is_merch) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test add merch", test_add_merch_to_store) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test remove merch", test_remove_merch) == NULL) || /*
        (CU_add_test(ioopm_backend_test_suite, "Test edit merch", test_edit_merch) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test replenish empty store", test_empty_stores_replenish) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test replenish store", test_shelves_replenish) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test replenish different shelves", test_replenish_different) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test replenish same shelf", test_replenish_same) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test carts add", test_cart_add_remove) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test removing ware from cart", test_cart_remove_item) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test cart add item", test_cart_add_item) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test cart edit item that is on shelf", test_edit_item_on_shelf) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test cart edit item that is in cart", test_edit_item_in_cart) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test remove an item from store that is reserved", test_remove_item_from_store_in_cart) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test calculate cost of wares in cart", test_calculate_cost) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test checkout single ware cart", test_checkout_single_ware_cart) == NULL) || 
        (CU_add_test(ioopm_backend_test_suite, "Test checkout multi ware cart", test_checkout_multi_ware_cart) == NULL) || */
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

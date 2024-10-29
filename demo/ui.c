#define _XOPEN_SOURCE   700
#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "equality_functions.h"
#include "hash_table.h"
#include "iterator.h"
#include "linked_list.h"
#include "utils.h"

#include "store_logic.h"
#include "ui.h"
#include "../src/refmem.h"

#define BUFSIZE 255

#define STRDUP_FREE(str) release(str)

// Prototypes
static void print_menu(void);

static bool valid_char(char buffert, char *valid_characters);

static bool ask_question_yes_no(char *question);

static char *ask_question_search_name(ioopm_store_t *store, char *question);

static char *ask_question_new_name(ioopm_store_t *store, char *question);

static char *ask_question_shelf(char *question);

static bool string_is_shelf(char *str);

static bool print_iter_list(ioopm_list_iterator_t *items, int *linenumber, int lines);

static char *stringcat(char *string1, char *string2);

static void pause_button(void);

static bool store_is_empty_check(ioopm_store_t *store);

static char *ref_strdup(char *src);

// Public functions
char ioopm_ask_question_menu(char *question)
{
    char *valid_chars = "LADESPCR+-=OQU";

    print_menu();
    char answer = toupper(ask_question_char(question));

    while (!valid_char(answer, valid_chars))
    {
        answer = ask_question_char("Skriv in ett giltigt val");
    }

    return answer;
}

void ioopm_store_list_merch(ioopm_store_t *store)
{
    if (store_is_empty_check(store)) return;

    ioopm_list_t *items = ioopm_get_merch_list(store);
    ioopm_list_iterator_t *items_iter = ioopm_list_iterator(items);
    int linenumber = 0;
    int lines = 20;

    // print_iter_list returns false if there is no more items to list
    while (print_iter_list(items_iter, &linenumber, lines))
    {
        if (valid_char(ask_question_char("[n] to quit, or any key to continue.\n"), "nN"))
        {
            break;
        }
    }

    ioopm_linkedlist_destroy(items);
    ioopm_iterator_destroy(items_iter);

    // Just to not return directly
    pause_button();

    return;
}

void ioopm_store_add_item(ioopm_store_t *store)
{
    char *name = ask_question_new_name(store, "Name of Item: ");

    char *description = ask_question_string("Item description: ");
    int cost = ask_question_int("Cost of item: ");

    ioopm_add_merch_to_store(store, name, description, cost);

    STRDUP_FREE(name);
    STRDUP_FREE(description);

    return;
}

void ioopm_store_remove_item(ioopm_store_t *store)
{
    if (store_is_empty_check(store)) return;

    char *name =
        ask_question_search_name(store, "Write the name of the item to remove: ");
    char *question = stringcat("Are you sure you want to delete ", name);

    if (ask_question_yes_no(question))
    {
        if (ioopm_merch_remove(store, name))
        {
            printf("Merch has been removed\n");
        }
        else
        {
            printf("Merch is currently added to a cart and could not be removed\n");
        }
    }

    STRDUP_FREE(question);
    STRDUP_FREE(name);
    return;
}

void ioopm_store_edit_item(ioopm_store_t *store)
{
    if (store_is_empty_check(store)) return;

    char *name =
        ask_question_search_name(store, "Write the name of the item you want to edit:");
    char *new_name = ask_question_new_name(store, "Write the new name of the item:");
    char *new_description = ask_question_string("Write a new description for the item:");
    int new_price = ask_question_int("Write a new price for the item:");

    if (ask_question_yes_no("Are you sure you want to proceed with the edit?"))
    {
        ioopm_merch_edit(store, name, new_name, new_description, new_price);
    }
    // As name only is used as a identifier, we free name
    STRDUP_FREE(name);
    STRDUP_FREE(new_name);
    STRDUP_FREE(new_description);

    return;
}

void ioopm_store_show_stock(ioopm_store_t *store)
{
    if (store_is_empty_check(store)) return;

    char *name = ask_question_search_name(store, "Show stock for item: ");
    bool error = false;
    ioopm_list_t *shelves = NULL;
    merch_t *merch = ioopm_get_merch(store, name);

    if (ioopm_get_merch_locations(merch,
                                  &shelves))   // true if item is in stock
    {
        ioopm_list_iterator_t *shelves_iter = ioopm_list_iterator(shelves);

        do
        {
            shelf_t *current_shelf = ioopm_iterator_current(shelves_iter).p;

            printf("%s : %d \n", ioopm_get_shelf_name(current_shelf),
                   ioopm_get_shelf_amount(current_shelf));

            ioopm_iterator_next(shelves_iter, &error);

        } while (!error);

        ioopm_iterator_destroy(shelves_iter);
    }
    else
    {
        printf("%s is not in stock.\n", name);
    }

    STRDUP_FREE(name);

    pause_button();

    return;
}

void ioopm_store_replenish_item(ioopm_store_t *store)
{
    int amount;

    char *name = ask_question_search_name(store, "Item to replenish: ");
    char *location = ask_question_shelf("Replenish shelf: ");

    do   // Amount needs to be over 0
    {
        amount = ask_question_int("Replenish amount: ");
    } while (amount < 1);

    // ioopm_replenish_item returns false if shelf are full
    while (!ioopm_replenish_item(store, name, location, amount))
    {
        printf("%s already had another item. ", location);

        STRDUP_FREE(location);
        location = ask_question_shelf("Select another shelf: ");
    }

    STRDUP_FREE(name);
    STRDUP_FREE(location);
    return;
}

void ioopm_store_create_cart(ioopm_store_t *store)
{
    size_t cartid = ioopm_add_cart_to_store(store);
    printf("Cart %lu is added to the store.\n", cartid);
    return;
}

void ioopm_store_remove_cart(ioopm_store_t *store)
{
    size_t cartid = ask_question_long("Write the id of the cart you want to remove:");

    if (!ioopm_is_cart(store, cartid))
    {
        printf("Cart %ld is not in store\n", cartid);
        return;
    }

    if (ask_question_yes_no("Are you sure you want to remove this cart?"))
    {
        ioopm_remove_cart_from_store(store, cartid);
        printf("Cart %ld has been removed from the store\n", cartid);
        return;
    }
    else
    {
        printf("The action has been canceled");
        return;
    }
    return;
}

void ioopm_store_add_to_cart(ioopm_store_t *store)
{
    if (store_is_empty_check(store)) return;
    size_t cartid = ask_question_long("Write your cartid:");

    if (!ioopm_is_cart(store, cartid))
    {
        printf("Not a valid cartid\n");
        return;
    }

    char *merchname =
        ask_question_search_name(store, "Write the name of the merch you want to add:");

    merch_t *merch = ioopm_get_merch(store, merchname);
    size_t available = ioopm_get_merch_amount(merch) - ioopm_get_merch_reserved(merch);
    printf("There are %lu %ss available \n", available, merchname);

    int amount_to_add =
        ask_question_int("Write the amount that you want to add to your cart:");

    while (amount_to_add > available)
    {
        amount_to_add = ask_question_int("Please write a smaller amount to add:");
    }

    if (ioopm_add_to_cart(store, cartid, merchname, amount_to_add))
    {
        printf("%d %ss has been added to your cart\n", amount_to_add, merchname);
    }
    else
    {
        printf("%s could not be added to your cart\n", merchname);
    }

    STRDUP_FREE(merchname);
    return;
}

void ioopm_store_remove_from_cart(ioopm_store_t *store)
{
    if (store_is_empty_check(store)) return;
    size_t cartid = ask_question_long("Write your cartid:");

    while (!ioopm_is_cart(store, cartid))
    {
        cartid = ask_question_long("Write a valid cartid:");
    }

    char *merchname = ask_question_search_name(
        store, "Write the name of the merch you want to remove:");

    if (ioopm_cart_has_ware(store, cartid, merchname))
    {

        int amount_in_cart = ioopm_get_ware_amount(store, cartid, merchname);

        int amount_to_remove =
            ask_question_int("Write the amount that you want to remove from your cart:");

        while (amount_to_remove > amount_in_cart)
        {
            amount_to_remove =
                ask_question_long("Please write a smaller amount to remove:");
        }

        ioopm_remove_item_from_cart(store, cartid, merchname, amount_to_remove);
        printf("%d %ss have been removed from your cart\n", amount_to_remove, merchname);
    }
    else
    {
        printf("%s is not in cart %lu\n", merchname, cartid);
    }

    STRDUP_FREE(merchname);
    return;
}

void ioopm_store_undo_action(ioopm_store_t *store)
{
    // TODO: Optional
    printf("Action not yet implemented\n");
    return;
}

void ioopm_store_cart_calculate_cost(ioopm_store_t *store)
{
    size_t cartid = ask_question_long("Write your cartid:");

    while (!ioopm_is_cart(store, cartid))
    {
        cartid = ask_question_long("Write a valid cartid:");
    }

    int cost = ioopm_calculate_cart_cost(store, cartid);
    printf("The total cost of the merch in cart %lu is %d\n", cartid, cost);

    return;
}

void ioopm_store_checkout(ioopm_store_t *store)
{
    size_t cartid = ask_question_long("Write your cartid:");
    int cost;

    if (!ioopm_is_cart(store, cartid))
    {
        printf("%lu is not a valid cart \n", cartid);
        return;
    }

    if (ioopm_checkout_cart(store, cartid, &cost))
    {
        printf("Thank you for your purchase, the total is %d\n", cost);
        return;
    }
    else
    {
        printf("Something went wrong and the purchase was canceled");
        return;
    }
}

void ioopm_store_quit(ioopm_store_t *store, bool *flag)
{
    if (ask_question_yes_no("Are you sure you want to quit?"))
    {
        *flag = false;
        return;
    }
    else
    {
        *flag = true;
    }
    return;
}
// Private functions

static void print_menu(void)
{
    printf("[A] Add merchandise\n"
           "[L] List merchandise\n"
           "[D] Remove merchandise\n"
           "[E] Edit merchandise\n"
           "[S] Show stock\n"
           "[P] Replenish stock\n"
           "[C] Create a cart\n"
           "[R] Remove cart\n"
           "[=] Calculate cost\n"
           "[+] Add an item to cart\n"
           "[-] Remove an item from cart\n"
           "[O] Checkout\n"
           "[U] Undo action\n"
           "[Q] Quit\n");
}

static bool valid_char(char buffert, char *valid_characters)
{
    int current_char = 0;
    while (valid_characters[current_char] != '\0')
    {
        if (buffert == valid_characters[current_char++])
        {
            return true;
        }
    }
    return false;
}

static char *stringcat(char *string1, char *string2)
{
    char *string_cat = allocate_array(strlen(string1) + strlen(string2) + 1, sizeof(char), NULL);
    strcpy(string_cat, string1);
    strcat(string_cat, string2);
    return string_cat;
}

static bool ask_question_yes_no(char *question)
{
    question = stringcat(question, " [y/n]");
    char answer = ask_question_char(question);

    while (!valid_char(answer, "ynYN"))
    {
        answer = ask_question_char(question);
    }

    STRDUP_FREE(question);

    if (answer == 'y' || answer == 'Y')
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool print_iter_list(ioopm_list_iterator_t *items_iter, int *linenumber, int lines)
{
    bool error = false;
    int i = 0;

    while (!error && i++ < lines)
    {
        char *key = ioopm_iterator_current(items_iter).p;

        printf("%d. %s \n", *linenumber += 1, key);

        ioopm_iterator_next(items_iter, &error);
    }

    if (error == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static bool string_is_shelf(char *str)
{
    int strlength = strlen(str);

    for (int i = 0; i < strlength; i += 1)
    {
        if (i == 0)
        {
            if (str[i] < 'A' || str[i] > 'Z')
            {
                return false;
            }
        }
        else
        {
            if (!isdigit(((unsigned char *)str)[i]))
            {
                return false;
            }
        }
    }
    return true;
}

static char *ask_question_shelf(char *question)
{
    return ask_question(question, string_is_shelf, (convert_func) ref_strdup).string_value;
}

static char *ask_question_search_name(ioopm_store_t *store, char *question)
{
    char *name = ask_question_string(question);

    while (!ioopm_is_merch(store, name))
    {
        printf("%s doesn't exist. Try again: ", name);
        STRDUP_FREE(name);
        name = ask_question_string("");
    }

    return name;
}

static char *ask_question_new_name(ioopm_store_t *store, char *question)
{
    char *name = ask_question_string(question);

    while (ioopm_is_merch(store, name))
    {
        printf("%s already exists. Please try another name: ", name);
        STRDUP_FREE(name);
        name = ask_question_string("");
    }

    return name;
}

static void pause_button(void)
{
    valid_char(ask_question_char("[m] to go back to menu"), "mM");
}

static bool store_is_empty_check(ioopm_store_t *store)
{
    if (ioopm_store_is_empty(store))
    {
        printf("There are no items in store.\n");
        return true;
    }
    {
        return false;
    }
}

static char *ref_strdup(char *src)
{
    size_t len = strlen(src);
    char *str = allocate_array(len + 1, sizeof(char), NULL);
    retain(str);
    strcpy(str, src);
    return str;
}

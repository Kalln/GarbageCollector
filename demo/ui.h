#pragma once

#include "store_logic.h"

/**
 * @file ui.h
 * @author Hampus Jigehamn & Thomas Nohrin
 * @date 06 November 2023
 * @brief A simple store user interface
 *
 * A simple store user interface that runs in terminal.
 * It lets users input strings to manage different store actions.
 *
 * The store functions retain ownership of all allocated strings, and make sure to free all strings after each function call.
 *
 * Errors are handled by printing error messages to the terminal
 *
 */

/**
 * @brief Prints menu and asks what choice to make. Only accepts the characters valid
 *        according to the menu print.
 * @param question string to be printed as a question of input.
 * @return valid char of selected action, e.g. 'A' for choice "Add item"
 */
char ioopm_ask_question_menu(char *question);

/**
 * @brief Print a list of all merches in store in linguistic order.
 *        It prints 20 items and then ask for a confirmation to list the next 20.
 *        When there is no more merches to list, it will pause and ask the user for any input before exiting.
 * @param store Store
 */
void ioopm_store_list_merch(ioopm_store_t *store);

/**
 * @brief Asks for name, description and price of an item that will be added to the store
 *        If item name is already in store, it will ask again until a new name is provided.
 * @param store Store
 */
void ioopm_store_add_item(ioopm_store_t *store);

/**
 * @brief Asks for item to be removed, removes item unless the item is in a cart
 *        Before removing the item, a confirmation is required from the user.
 * @param store Store
 */
void ioopm_store_remove_item(ioopm_store_t *store);
/**
 * @brief Edit an items description and price
 * @param store
 */
void ioopm_store_edit_item(ioopm_store_t *store);

/**
 * @brief Show available stock of an item in store, and prints out the shelf locations with amount of items
 * @param store Store
 */
void ioopm_store_show_stock(ioopm_store_t *store);

/**
 * @brief Replenish an item of provided amount at given shelf location.
            Shelf-names consist of an uppercase letter followed by two numbers.
 * @param store Store
 */
void ioopm_store_replenish_item(ioopm_store_t *store);

/**
 * @brief Creates a cart in the store with a unique id.
 * @param store Store
 */
void ioopm_store_create_cart(ioopm_store_t *store);

/**
 * @brief Removes a cart if the cart is in store, otherwise prints a warning and returns to menu
 *        Before removing the cart, a confirmation is required from the user.
 * @param store Store
 */
void ioopm_store_remove_cart(ioopm_store_t *store);

/**
 * @brief Asks for a cart and item to be added to cart. Prints out available amount of item.
 *        If item can be added to cart, a confirmation is printed, else a warning is printed
 * @param store Store
 */
void ioopm_store_add_to_cart(ioopm_store_t *store);

/**
 * @brief Asks for a cart to remove an item from. Asks for how much of the item to be removed.
 *        If the amount is bigger than whats in cart, it will ask again for a smaller amount to be
 * @param store Store
 */
void ioopm_store_remove_from_cart(ioopm_store_t *store);
/**
 * @brief Not implemented
 *
 * @param store Store
 */
void ioopm_store_undo_action(ioopm_store_t *store);

/**
 * @brief Continuesly asks for a valid cartid. Prints the total cost of the items in the cart
 * @param store Store
 */
void ioopm_store_cart_calculate_cost(ioopm_store_t *store);

/**
 * @brief Asks for a cart that will be checked out. Checks out the cart and prints out a confirmation together with total cost
 * @param store
 */
void ioopm_store_checkout(ioopm_store_t *store);

/**
 * @brief Takes an eventloop flag and set to false, which will quit the store program
 * @param store Store
 * @param flag Event loop flag that will be set to false
 */
void ioopm_store_quit(ioopm_store_t *store, bool *flag);

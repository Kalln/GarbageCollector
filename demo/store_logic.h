#pragma once

#include "hash_table.h"
#include "linked_list.h"

/**
 * @file store_logic.h
 * @author Hampus Jigehamn & Thomas Nohrin
 * @date 06 November 2023
 * @brief A simple store-like logic database, with merchandise, warehouse and shopping carts.
 *
 * A simple store logic that lets users add, edit, remove and keep track of merchandise, locations in warehouse and shopping carts.
 *
 * The store datastructures copies all necessary strings given to the functions, taking ownership of the strings.
 * All strings used to call the functions may be free'd at any given time by the user.
 *
 * Errors are handled by function returning a boolean(true = no errors, false = error) if nothing else is specified in the function documentation
 *
 */

typedef struct merch merch_t;
typedef struct shelf shelf_t;
typedef struct store ioopm_store_t;
typedef struct cart cart_t;

/**
 * @brief Creates an empty store
 * @return A pointer to an empty store
 */
ioopm_store_t *ioopm_store_create(void);

/**
 * @brief Destroys the given store and returns it's allocated resources
 * @param store A pointer to the store you want to destroy
 */
void ioopm_store_destroy(ioopm_store_t *store);

/**
 * @brief Checks if the store is empty or not, i.e. there is no items in store
 * @param store A pointer to a store that you want to see if it's empty or not
 * @return True if the store is empty, false if not
 */
bool ioopm_store_is_empty(ioopm_store_t *store);

/**
 * @brief Adds a merch to the store with provided information.
 *
 * @param store A pointer to the store you want to add the merch to
 * @param name The name of the merch you want to add
 * @param description The description of the merch you want to add
 * @param cost The cost of the merch you want to add
 */
void ioopm_add_merch_to_store(ioopm_store_t *store, char *name, char *description,
                              int cost);

/**
 *  @brief Removes the merch with the given name from the given store.
 *         Also removes item from all carts and shelves.
 * @param store A pointer to the store you want to remove the merch from
 * @param name The name of the merch you want to remove
 * @return True if the merch was found and removed, false if not
 */
bool ioopm_merch_remove(ioopm_store_t *store, char *name);

/**
 * @brief Replaces existing information about an existing merch with the provided information (name, description and price).
 *        Also edits name in carts and on shelves.
 * @param store Store
 * @param name The current name of the merch that you want to edit
 * @param new_name The new name you want the merch to have
 * @param new_description The new description you want the merch to have
 * @param new_price The new price you want the merch to have
 */
void ioopm_merch_edit(ioopm_store_t *store, char *name, char *new_name,
                      char *new_description, int new_price);

/**
 * @brief Given a pointer to a store, it returns a sorted list of all the merchnames in the store.
 *        If the store is empty, it returns an empty list.
 * @param store A pointer to the store which merch you want to see
 * @return A sorted list of merchnames as strings
 */
ioopm_list_t *ioopm_get_merch_list(ioopm_store_t *store);

/**
 * @brief Checks if a store contains merch with a given name
 * @param store A pointer to a store
 * @param merch_name The name of a merch that you want to see if the store contains
 * @return True if the store contains a merch with the given name, false if not
 */
bool ioopm_is_merch(ioopm_store_t *store, char *merch_name);

/**
 * @brief Retrieves a merch from a store. Expects merch to be in store.
 * @param store A pointer to a store containing the wanted merch
 * @param name The name of the merch that you want the information of
 * @return a merch if merch is in store, otherwise undefined.
 */
merch_t *ioopm_get_merch(ioopm_store_t *store, char *name);

/**
 * @brief Given a merch, returns the name of it
 * @param merch The merch you want the name of
 * @return The name of the given merch
 */
char *ioopm_get_merch_name(merch_t *merch);

/**
 * @brief Returns amount of merch.
 * @param merch merch
 * @return The current amount of the merch
 */
int ioopm_get_merch_amount(merch_t *merch);

/**
 * @brief Returns reserved amount of a merch
 * @param merch The merch you that want to know how many copies are reserved
 * @return reserved amount of merch
 */
int ioopm_get_merch_reserved(merch_t *merch);

/**
 * @brief Given a merch it returns its description
 * @param merch The merch that you want to know the description of
 * @return The description of the given merch
 */
char *ioopm_get_merch_desc(merch_t *merch);

/**
 * @brief Given a merch it returns its price
 * @param merch The merch that you want to know the price of
 * @return The price of the given merch
 */
int ioopm_get_merch_price(merch_t *merch);

/**
 * @brief Checks if provided shelfname is a shelf in store
 * @param store Store
 * @param shelf Shelfname to be checked if in store
 * @return True if there is a shelf with same name in store, else false.
 */
bool ioopm_is_shelf(ioopm_store_t *store, char *shelf);

/**
 * @brief Given a shelf it returns its name
 * @param shelf The shelf that you want to know the name of
 * @return The name of the given shelf
 */
char *ioopm_get_shelf_name(shelf_t *shelf);

/**
 * @brief Get itemname from provided shelf
 * @param shelf Shelf in store
 * @return
 */
char *ioopm_get_shelf_itemname(shelf_t *shelf);

/**
 * @brief Given a shelf it returns the amount of unspecified merch it is holding
 * @param shelf The shelf that you want to know how much merch it is holding
 * @return The amount of merch it is holding
 */
int ioopm_get_shelf_amount(shelf_t *shelf);
/**
 * @brief Increases the amount of a merch on a shelf in the store.
 *        If the shelf does not exist, it will add the shelf to the store.
 *        If the shelf does exist, if the merch is the same as already on shelf, it will add the new amount to the shelf.
 *        Else it will not add anything and return false.
 * @param store A pointer to the store containing the merch and shelf
 * @param name The name of the merch that you want to increase the amount of
 * @param shelf The name of the shelf that you want to hold the merch
 * @param amount The amount you want increase the merch with
 * @return True if the shelf dont exist, or if the shelf holds the same merch as given.
 */
bool ioopm_replenish_item(ioopm_store_t *store, char *name, char *shelf, int amount);

/**
 * @brief Given a pointer to a store and a name of a shelf it returns the shelf with that name and its related information
 * @param store A pointer to a store
 * @param name The name of the shelf that you want the information of
 * @return The shelf with the given name and its related information
 */
shelf_t *ioopm_get_shelf(ioopm_store_t *store, char *name);

/**
 * @brief Gets a merch location list from a given merch.
 * @param merch The merch that you find the locations of
 * @param retur A pointer to a list that will return the location list. If merch isn't on any shelves, it will return a empty list.
 * @return True if merch has any locations, false if merch doesn't have any locations.
 */
bool ioopm_get_merch_locations(merch_t *merch, ioopm_list_t **retur);

/**
 * @brief Adds a new cart to the store
 * @param store A pointer to the store in which you want to add a cart
 * @return The id of the added cart
 */
size_t ioopm_add_cart_to_store(ioopm_store_t *store);

/**
 * @brief Removes a cart from the store, and unreserve all items in store.
 * @param store A pointer to the store that has the cart that is to be removed
 * @param cartid The id of the cart that is to be removed
 * @return True if the cart was found and remove, false if not
 */
bool ioopm_remove_cart_from_store(ioopm_store_t *store, size_t cartid);

/**
 * @brief Checks if a cart id is in store.
 * @param store A pointer to a store
 * @param cartid The id of the cart that is checked
 * @return true if cart id is a cart in store, else false
 */
bool ioopm_is_cart(ioopm_store_t *store, size_t cartid);

/**
 * @brief Gets a cart from the store
 * @param store A pointer to a store
 * @param cartid The id of the cart you want
 * @param cart If the cart is found, its information will be stored in this argument. Otherwise it will be undefined.
 * @return True if the cart was found, false if not
 */
bool ioopm_get_cart(ioopm_store_t *store, size_t cartid, cart_t **cart);

/**
 * @brief Checks if cart has item (Used for testing)
 * @param store Store
 * @param cartid Cart to be checked
 * @param name Name of item to be checked if in cart
 * @return True if item is in cart
 */
bool ioopm_cart_has_ware(ioopm_store_t *store, size_t cartid, char *name);

/**
 * @brief Adds some amount of a merch to a cart.
 *        If the amount is bigger than what is available, it will return false.
 * @param store A pointer to a store that has the cart
 * @param cartid The id of the cart that you want to add merch to
 * @param merchname The name of the merch that you want to add to the cart
 * @param amount The amount of copies of the given merch that you want to add to the cart
 * @return True if the merch was added correctly to the cart, false if not
 */
bool ioopm_add_to_cart(ioopm_store_t *store, size_t cartid, char *merchname, int amount);

/** @brief Removes some amount of a merch from a cart
 * @param store A pointer to a store that has the cart
 * @param cartid The id of the cart that want to remove merch from
 * @param merchname The name of the merch you want to remove copies of
 * @param amount The amount of copies of merch that you want to remove
 * @return True if the merch was removed correctly from the cart, false if not
 */
bool ioopm_remove_item_from_cart(ioopm_store_t *store, size_t cartid, char *merchname,
                                 int amount);

/**
 * @brief Checks the amount a cart has of a certain merch/ware
 * @param store Store
 * @param cartid The cart in which you want to look for a merch
 * @param itemname The name of the merch that you want to know the amount of in the cart
 * @return The amount of the merch that the cart has
 */
int ioopm_get_ware_amount(ioopm_store_t *store, size_t cartid, char *itemname);

/**
 * @brief Calculates the cost off all wares/merch in a cart. If the cart is not found, it will return 0.
 * @param store A pointer to a store
 * @param cartid The id of the cart that you want to calculate the cost of
 * @return The cost of all wares/merch in a cart. 0 if cart is not found
 */
int ioopm_calculate_cart_cost(ioopm_store_t *store, size_t cartid);

/**
 * @brief Calculates price of all items in a cart, and then removes all items in cart from store.
 *        The cart is then freed and removed from store
 *
 * @param store Store
 * @param cartid Id of cart to checkout
 * @param cost_return pointer to return the total price of the cart to
 * @return True if there is a cart, else false
 */
bool ioopm_checkout_cart(ioopm_store_t *store, size_t cartid, int *cost_return);

#define _XOPEN_SOURCE   700
#define _POSIX_C_SOURCE 200809L

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "equality_functions.h"
#include "hash_table.h"
#include "iterator.h"
#include "linked_list.h"

#include "store_logic.h"

#include "../src/refmem.h"

#define merch_elem(x)                                                                    \
    (elem_t) { .p = (x) }

#define shelf_elem(x)                                                                    \
    (elem_t) { .p = (x) }

#define cart_elem(x)                                                                     \
    (elem_t) { .p = (x) }

#define str_elem(x)                                                                      \
    (elem_t) { .p = (x) }

#define int_elem(x)                                                                      \
    (elem_t) { .i = (x) }

#define uint_elem(x)                                                                     \
    (elem_t) { .uns_int = (x) }

#define STRDUP_FREE(str) release(str)

#define ADD true
#define SUB false

typedef struct merch merch_t;
typedef struct shelf shelf_t;
typedef struct store ioopm_store_t;

struct store
{
    ioopm_hash_table_t *items;
    ioopm_hash_table_t *shelves;
    ioopm_hash_table_t *carts;
    size_t cart_id_counter;
};

struct merch
{
    char *name;
    char *desc;
    ioopm_list_t *locations;
    int cost;
    size_t amount;
    size_t reserved;
};

struct cart
{
    size_t id;
    ioopm_hash_table_t *wares;
};

struct shelf
{
    char *shelfname;
    char *item;
    size_t amount;
};

// Prototypes
static void ioopm_store_clear(ioopm_store_t *store);

static merch_t *ioopm_merch_create(char *name, char *description, int cost);

static bool merch_equiv(elem_t value_a, elem_t value_b);

static bool shelf_equiv(elem_t value_a, elem_t value_b);

static bool cart_equal(elem_t value_a, elem_t value_b);

static int strcomp(elem_t x, elem_t y);

static int shelfcomp(elem_t x, elem_t y);

static void merch_destroy(merch_t *merch);

static bool merch_is_on_shelf(merch_t *merch, char *shelfname);

static shelf_t *create_shelf(ioopm_store_t *store, char *name, char *shelfname,
                             int amount);

static void update_shelf(ioopm_store_t *store, shelf_t *shelf, int amount,
                         bool add_or_sub);

static void add_shelf_to_store(ioopm_store_t *store, char *name, char *shelfname,
                               int amount);

static void apply_store_items_destroy(elem_t key, elem_t *merch, void *ignore);

static void apply_store_shelves_destroy(elem_t key, elem_t *value, void *ignore);

static void shelf_destroy(shelf_t *shelf);

static void swap_and_free_old_string(char **old, char **new);

static cart_t *ioopm_create_cart(ioopm_store_t *store);

static void ioopm_remove_all_carts(ioopm_store_t *store);

static void ioopm_destroy_cart(ioopm_store_t *store, cart_t *cart);

static void ioopm_clear_cart(ioopm_store_t *store, cart_t *cart);

static void ioopm_change_reservation(ioopm_store_t *store, merch_t *merch,
                                     bool add_or_sub, size_t amount);

static void add_item_to_cart(ioopm_store_t *store, cart_t *cart, merch_t *merch,
                             int amount);

static bool merch_is_in_stock(merch_t *merch, int amount);

static void remove_cart_apply(elem_t key, elem_t *value, void *extra);

static void update_cart_counter(ioopm_store_t *store);

static bool cart_is_empty(cart_t *cart);

static int calculate_cart_cost(ioopm_store_t *store, cart_t *cart);

static bool cart_has_ware(cart_t *cart, char *name);

static bool remove_ware_from_cart(cart_t *cart, merch_t *merch);

static void update_wares_in_cart(cart_t *cart, char *merchname, size_t amount);

static void ioopm_checkout_merch(ioopm_store_t *store, merch_t *merch, size_t amount);

static void remove_shelf(ioopm_store_t *store, shelf_t *shelf, merch_t *merch);

static bool remove_cart_wares_from_store(ioopm_store_t *store, cart_t *cart);

static int get_ware_amount(cart_t *cart, char *name);

static void edit_merch_in_cart(elem_t cartid, elem_t *cartelem, void *namesarray);

static void edit_merch_in_carts(ioopm_store_t *store, char *names[]);

static void edit_merch_in_shelf(merch_t *merch, char *newname);

static void shelf_edit_merchname(size_t index, elem_t *value, void *newname);

static char *ref_strdup(char *src);

// Public Store specific functions

ioopm_store_t *ioopm_store_create(void)
{
    ioopm_store_t *store = allocate(sizeof(ioopm_store_t), NULL);
    retain(store);

    store->items = ioopm_hash_table_create(ioopm_djb2_hash, ioopm_string_equal, merch_equiv);
    store->shelves = ioopm_hash_table_create(ioopm_djb2_hash, ioopm_string_equal, shelf_equiv);
    store->carts = ioopm_hash_table_create(ioopm_simple_unsint_hash, ioopm_unsint_equal, cart_equal);

    store->cart_id_counter = 0;
    return store;
}

void ioopm_store_destroy(ioopm_store_t *store)
{
    ioopm_store_clear(store);

    ioopm_hash_table_destroy(store->items);
    ioopm_hash_table_destroy(store->shelves);
    ioopm_hash_table_destroy(store->carts);

    release(store);
    store = NULL;
    return;
}

bool ioopm_store_is_empty(ioopm_store_t *store)
{
    return ioopm_hash_table_isempty(store->items);
}

// Public merch specific functions

void ioopm_add_merch_to_store(ioopm_store_t *store, char *nameinput, char *descinput,
                              int cost)
{

    char *name = ref_strdup(nameinput);
    char *description = ref_strdup(descinput);


    merch_t *merch = ioopm_merch_create(name, description, cost);
    retain(merch);
    elem_t key = str_elem(merch->name);
    elem_t value = merch_elem(merch);
    ioopm_hash_table_insert(store->items, key, value);
    STRDUP_FREE(name);
    STRDUP_FREE(description);
    release(merch);
}

bool ioopm_is_merch(ioopm_store_t *store, char *merch_name)
{
    return ioopm_hash_table_has_key(store->items, str_elem(merch_name));
}

bool ioopm_merch_remove(ioopm_store_t *store, char *name)
{

    elem_t retur;
    merch_t *merch = ioopm_get_merch(store, name);
    retain(merch);
    if (merch->reserved > 0)
    {
        release(merch);
        return false;
    }
    else
    {
        
        ioopm_hash_table_remove(store->items, str_elem(name), &retur);
        //merch_destroy(merch); 
        release(merch);
        return true;
    }
}

void ioopm_merch_edit(ioopm_store_t *store, char *name, char *new_nameinput,
                      char *new_descinput, int new_price)
{
    // Remove merch from hashtable as we get new key. Update values and free old
    // strdups

    char *new_name = ref_strdup(new_nameinput);
    retain(new_name);
    char *new_description = ref_strdup(new_descinput);
    retain(new_description);

    elem_t item;
    ioopm_hash_table_remove(store->items, str_elem(name), &item);
    merch_t *merch = item.p;
    retain(merch);

    char *names[] = {name, new_name};
    edit_merch_in_carts(store, names);

    edit_merch_in_shelf(merch, new_name);

    swap_and_free_old_string(&merch->name, &new_name);
    swap_and_free_old_string(&merch->desc, &new_description);
    merch->cost = new_price;



    ioopm_hash_table_insert(store->items, str_elem(new_name), merch_elem(merch));
    release(new_description);
    release(new_name);
    release(merch);
}

ioopm_list_t *ioopm_get_merch_list(ioopm_store_t *store)
{
    ioopm_list_t *key_list = ioopm_hash_table_getkeys(store->items);
    retain(key_list);
    if (!ioopm_linkedlist_isempty(key_list))
    {
        ioopm_linkedlist_bubblesort(key_list, strcomp);
    }
    release(key_list);
    return key_list;
}

merch_t *ioopm_get_merch(ioopm_store_t *store, char *name)
{

    elem_t merch;

    ioopm_hash_table_lookup(store->items, str_elem(name), &merch);

    return merch.p;
}

char *ioopm_get_merch_name(merch_t *merch) { return merch->name; }

int ioopm_get_merch_amount(merch_t *merch) { return merch->amount; }

int ioopm_get_merch_reserved(merch_t *merch) { return merch->reserved; }

char *ioopm_get_merch_desc(merch_t *merch) { return merch->desc; }

int ioopm_get_merch_price(merch_t *merch) { return merch->cost; }

bool ioopm_get_merch_locations(merch_t *merch, ioopm_list_t **retur)
{
    *retur = merch->locations;
    retain(retur);

    if (ioopm_linkedlist_isempty(*retur))
    {
        release(retur);
        return false;
    }
    else
    {
        ioopm_linkedlist_bubblesort(*retur, shelfcomp);
        release(retur);
        return true;
    }
}

// Public shelf specific functions

bool ioopm_is_shelf(ioopm_store_t *store, char *shelf)
{
    return ioopm_hash_table_has_key(store->shelves, str_elem(shelf));
}

shelf_t *ioopm_get_shelf(ioopm_store_t *store, char *name)
{
    elem_t retur;
    ioopm_hash_table_lookup(store->shelves, str_elem(name), &retur);
    return (shelf_t *) retur.p;
}

char *ioopm_get_shelf_name(shelf_t *shelf) { return shelf->shelfname; }

char *ioopm_get_shelf_itemname(shelf_t *shelf) { return shelf->item; }

int ioopm_get_shelf_amount(shelf_t *shelf) { return shelf->amount; }

bool ioopm_replenish_item(ioopm_store_t *store, char *name, char *shelfnameinput,
                          int amount)
{
    merch_t *merch = ioopm_get_merch(store, name);
    retain(merch);
    char *shelfname = ref_strdup(shelfnameinput);


    if (merch_is_on_shelf(merch, shelfname))
    {
        shelf_t *shelf = ioopm_get_shelf(store, shelfname);
        retain(shelf);
        update_shelf(store, shelf, amount, ADD);
        release(shelf);
        merch->amount += amount;

        STRDUP_FREE(shelfname);
        release(merch);
        return true;
    }
    else if (!ioopm_is_shelf(store, shelfname))
    {
        add_shelf_to_store(store, name, shelfname, amount);
        merch->amount += amount;
        release(merch);
        return true;
    }
    else
    {
        release(merch);

        STRDUP_FREE(shelfname);   // Free as only want on heap if shelf does not exist
        return false;
    }
}

//Public cart specific functions

size_t ioopm_add_cart_to_store(ioopm_store_t *store)
{
    cart_t *cart = ioopm_create_cart(store);
    retain(cart);
    ioopm_hash_table_insert(store->carts, uint_elem(cart->id), cart_elem(cart));
    release(cart);
    return cart->id;
}

bool ioopm_remove_cart_from_store(ioopm_store_t *store, size_t cartid)
{
    cart_t *cart = NULL;

    if (ioopm_get_cart(store, cartid, &cart))
    {
        retain(cart);
        elem_t ret;
        ioopm_destroy_cart(store, cart);
        ioopm_hash_table_remove(store->carts, uint_elem(cartid), &ret);
        release(cart);
        return true;
    }

    return false;
}

bool ioopm_is_cart(ioopm_store_t *store, size_t cartid)
{
    elem_t ret;
    return ioopm_hash_table_lookup(store->carts, uint_elem(cartid), &ret);
}

bool ioopm_get_cart(ioopm_store_t *store, size_t cartid, cart_t **cart)
{
    elem_t ret;

    if (ioopm_hash_table_lookup(store->carts, uint_elem(cartid), &ret))
    {
        *cart = ret.p;
        return true;
    }
    else
    {
        return false;
    }
}

int ioopm_get_ware_amount(ioopm_store_t *store, size_t cartid, char *itemname)
{
    cart_t *cart;
    ioopm_get_cart(store, cartid, &cart);
    retain(cart);
    release(cart);
    return get_ware_amount(cart, itemname);
}

bool ioopm_add_to_cart(ioopm_store_t *store, size_t cartid, char *merchname, int amount)
{
    cart_t *cart;
    if (ioopm_is_merch(store, merchname))
    {
        merch_t *merch = ioopm_get_merch(store, merchname);
        retain(merch);

        if (ioopm_get_cart(store, cartid, &cart) && !cart_has_ware(cart, merchname) &&
            merch_is_in_stock(merch, amount))
        {
            retain(cart);
            add_item_to_cart(store, cart, merch, amount);
            release(cart);
            release(merch);
            return true;
        }
        release(merch);
    }
    return false;
}

bool ioopm_remove_item_from_cart(ioopm_store_t *store, size_t cartid, char *merchname,
                                 int amount)
{
    cart_t *cart;
    elem_t merch_amount;
    merch_t *merch = ioopm_get_merch(store, merchname);   //Merch has to exist
    retain(merch);

    if (ioopm_get_cart(store, cartid, &cart) &&
        ioopm_hash_table_lookup(cart->wares, str_elem(merch->name), &merch_amount))
    {
        retain(cart);
        int new_amount = merch_amount.uns_int - amount;

        if (new_amount > 0)
        {
            update_wares_in_cart(cart, merch->name, new_amount);
            ioopm_change_reservation(store, merch, SUB, amount);
            release(merch);
            release(cart);
            return true;
        }
        else
        {
            remove_ware_from_cart(cart, merch);
            ioopm_change_reservation(store, merch, SUB, merch_amount.uns_int);
            release(merch);
            release(cart);
            return true;
        }
    }
    else
    {
        release(merch);
        return false;
    }
}

int ioopm_calculate_cart_cost(ioopm_store_t *store, size_t cartid)
{
    int cost = 0;
    cart_t *cart;

    if (ioopm_get_cart(store, cartid, &cart) && !cart_is_empty(cart))
    {
        retain(cart);
        cost = calculate_cart_cost(store, cart);
        release(cart);
    }

    return cost;
}

bool ioopm_checkout_cart(ioopm_store_t *store, size_t cartid, int *cost_return)
{
    cart_t *cart;
    *cost_return = 0;
    elem_t ret;

    if (ioopm_get_cart(store, cartid, &cart))
    {
        retain(cart);

        if (!cart_is_empty(cart))
        {
            *cost_return = calculate_cart_cost(store, cart);
            remove_cart_wares_from_store(store, cart);
        }

        ioopm_destroy_cart(store, cart);
        ioopm_hash_table_remove(store->carts, uint_elem(cartid), &ret);
        release(cart);

        return true;
    }
    return false;
}

bool ioopm_cart_has_ware(ioopm_store_t *store, size_t cartid, char *name)
{
    cart_t *cart;
    ioopm_get_cart(store, cartid, &cart);
    retain(cart);
    release(cart);
    return cart_has_ware(cart, name);
}

// STATIC

static void ioopm_store_clear(ioopm_store_t *store)
{
    ioopm_remove_all_carts(store);
    ioopm_hash_table_apply_all(store->shelves, apply_store_shelves_destroy, NULL);
    ioopm_hash_table_apply_all(store->items, apply_store_items_destroy, NULL);
    return;
}

static bool merch_equiv(elem_t value_a, elem_t value_b)
{
    merch_t *merch_a = (merch_t *) value_a.p;
    merch_t *merch_b = (merch_t *) value_b.p;
    retain(merch_a);
    release(merch_a);
    retain(merch_b);
    release(merch_b);

    return strcmp(merch_a->name, merch_b->name) == 0;
}

static bool shelf_equiv(elem_t value_a, elem_t value_b)
{
    shelf_t *shelf_a = (shelf_t *) value_a.p;
    shelf_t *shelf_b = (shelf_t *) value_b.p;
    retain(shelf_a);
    retain(shelf_b);
    release(shelf_a);
    release(shelf_b);

    return strcmp(shelf_a->shelfname, shelf_b->shelfname) == 0;
}

static bool cart_equal(elem_t value_a, elem_t value_b)
{

    cart_t *cart_a = (cart_t *) value_a.p;
    cart_t *cart_b = (cart_t *) value_b.p;
    retain(cart_a);
    retain(cart_b);
    release(cart_a);
    release(cart_b);

    return cart_a->id == cart_b->id;
}

static merch_t *ioopm_merch_create(char *name, char *description, int cost)
{
    merch_t *merch = allocate(sizeof(merch_t), NULL);
    retain(merch);

    merch->name = name;
    merch->desc = description;
    merch->cost = cost;
    merch->amount = 0;
    merch->locations = ioopm_linkedlist_create(shelf_equiv);   // Empty list of shelves


    return merch;
}

static void apply_store_items_destroy(elem_t key, elem_t *merch, void *ignore)
{
    merch_t *item = (merch_t *) merch->p;
    retain(item);
    release(item);
    merch_destroy(item);
    return;
}

static void merch_destroy(merch_t *merch)
{
    ioopm_linkedlist_destroy(merch->locations);
    release(merch->locations);
    release(merch->name);
    release(merch->desc);
    release(merch);
}

static void update_shelf(ioopm_store_t *store, shelf_t *shelf, int amount,
                         bool add_or_sub)
{
    if (add_or_sub)
    {
        shelf->amount += amount;
    }
    else
    {
        shelf->amount -= amount;
    }
    return;
}

static void add_shelf_to_store(ioopm_store_t *store, char *name, char *shelfname,
                               int amount)
{
    merch_t *merch = ioopm_get_merch(store, name);
    retain(merch);
    shelf_t *shelf = create_shelf(store, merch->name, shelfname, amount);
    retain(shelf);

    ioopm_hash_table_insert(store->shelves, str_elem(shelfname), shelf_elem(shelf));
    ioopm_linkedlist_append(merch->locations, shelf_elem(shelf));
    release(merch);
    release(shelf);
    return;
}

static shelf_t *create_shelf(ioopm_store_t *store, char *name, char *shelfname,
                             int amount)
{
    shelf_t *shelf = allocate(sizeof(shelf_t), NULL);
    retain(shelf);

    shelf->shelfname = shelfname;
    shelf->item = name;
    shelf->amount = amount;

    return shelf;
}

static bool merch_is_on_shelf(merch_t *merch, char *shelfname)
{
    bool error;
    ioopm_list_t *locations = merch->locations;
    retain(locations);
    if (ioopm_linkedlist_isempty(locations))   // Check that list is not empty
                                               // before doing an iterator
    {
        release(locations);
        return false;
    }

    ioopm_list_iterator_t *loc_iter = ioopm_list_iterator(locations);

    // Iterate over items location list to see if shelf is on the list.

    do
    {
        retain(loc_iter);
        shelf_t *current_shelf = ioopm_iterator_current(loc_iter).p;
        retain(current_shelf);
        if (strcmp(current_shelf->shelfname, shelfname) == 0)
        {
            ioopm_iterator_destroy(loc_iter);
            release(locations);
            release(loc_iter);
            release(current_shelf);
            return true;
        }
        release(loc_iter);
        ioopm_iterator_next(loc_iter, &error);
        release(current_shelf);
    } while (ioopm_iterator_has_next(loc_iter));

    ioopm_iterator_destroy(loc_iter);
    release(locations);
    return false;
}

static void apply_store_shelves_destroy(elem_t key, elem_t *value, void *ignore)
{
    shelf_t *shelf = (shelf_t *) value->p;
    retain(shelf);
    shelf_destroy(shelf);
    release(shelf);
    return;
}

static void shelf_destroy(shelf_t *shelf)
{
    release(shelf->shelfname);
    release(shelf);
}

static void swap_and_free_old_string(char **old, char **new)
{
    char *tmp = *old;
    *old = *new;
    STRDUP_FREE(tmp);
}

static bool cart_is_empty(cart_t *cart) { return ioopm_hash_table_isempty(cart->wares); }

static cart_t *ioopm_create_cart(ioopm_store_t *store)
{
    cart_t *cart = allocate(sizeof(cart_t), NULL);
    retain(cart);
    cart->id = store->cart_id_counter;
    update_cart_counter(store);

    cart->wares =
        ioopm_hash_table_create(ioopm_djb2_hash, ioopm_string_equal, ioopm_int_equal);

    return cart;
}

static void ioopm_destroy_cart(ioopm_store_t *store, cart_t *cart)
{
    ioopm_clear_cart(store, cart);
    ioopm_hash_table_destroy(cart->wares);
    release(cart);
}

static void ioopm_clear_cart(ioopm_store_t *store, cart_t *cart)
{

    if (cart_is_empty(cart))
    {
        // Do nothing
    }
    else
    {
        ioopm_list_t *ware_names = ioopm_hash_table_getkeys(cart->wares);
        retain(ware_names);
        ioopm_list_t *ware_amounts = ioopm_hash_table_getvalues(cart->wares);
        retain(ware_amounts);
        ioopm_list_iterator_t *name_iter = ioopm_list_iterator(ware_names);
        retain(name_iter);
        ioopm_list_iterator_t *amount_iter = ioopm_list_iterator(ware_amounts);
        retain(amount_iter);

        bool name_error = false;
        bool amount_error = false;

        do
        {
            char *current_ware_name = ioopm_iterator_current(name_iter).p;
            retain(current_ware_name);
            int current_ware_amount = ioopm_iterator_current(amount_iter).i;
            merch_t *current_merch = ioopm_get_merch(store, current_ware_name);
            retain(current_merch);

            ioopm_change_reservation(store, current_merch, SUB, current_ware_amount);
            release(current_merch);

            release(name_iter);
            ioopm_iterator_next(name_iter, &name_error);
            release(amount_iter);
            ioopm_iterator_next(amount_iter, &amount_error);
            retain(name_iter);
            retain(amount_iter);
            release(current_ware_name);

        } while (!name_error && !amount_error);

        //Clear last one in the list

        ioopm_iterator_destroy(name_iter);
        ioopm_iterator_destroy(amount_iter);
        ioopm_linkedlist_destroy(ware_names);
        ioopm_linkedlist_destroy(ware_amounts);
        release(name_iter);
        release(amount_iter);
        release(ware_names);
        release(ware_amounts);
    }
}

static void ioopm_change_reservation(ioopm_store_t *store, merch_t *merch,
                                     bool add_or_sub, size_t amount)
{
    if (add_or_sub)
    {
        merch->reserved = merch->reserved + amount;
    }
    else
    {
        merch->reserved = merch->reserved - amount;
    }
}

static void add_item_to_cart(ioopm_store_t *store, cart_t *cart, merch_t *merch,
                             int amount)
{
    ioopm_hash_table_insert(cart->wares, str_elem(merch->name), int_elem(amount));
    ioopm_change_reservation(store, merch, ADD, amount);
}

static bool merch_is_in_stock(merch_t *merch, int amount)
{
    if (merch != NULL && merch->amount - merch->reserved >= amount)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void update_cart_counter(ioopm_store_t *store)
{
    if (store->cart_id_counter < UINT_MAX)
    {
        store->cart_id_counter += 1;
    }
    else
    {
        // Loop around so we dont get overflow
        store->cart_id_counter = 0;
    }
}

static void ioopm_remove_all_carts(ioopm_store_t *store)
{
    if (!ioopm_hash_table_isempty(store->carts))
    {
        ioopm_hash_table_apply_all(store->carts, remove_cart_apply, store);
        ioopm_hash_table_clear(store->carts);
    }
}

static void remove_cart_apply(elem_t key, elem_t *value, void *extra)
{
    ioopm_store_t *store = (ioopm_store_t *) extra;
    retain(store);
    cart_t *cart = (cart_t *) value->p;
    retain(cart);

    ioopm_destroy_cart(store, cart);
    release(store);
    release(cart);
}

static int get_ware_amount(cart_t *cart, char *name)
{
    elem_t amount;

    ioopm_hash_table_lookup(cart->wares, str_elem(name), &amount);

    return amount.i;
}

static int calculate_cart_cost(ioopm_store_t *store, cart_t *cart)
{
    int cost = 0;
    bool name_error = false;

    ioopm_list_t *ware_names = ioopm_hash_table_getkeys(cart->wares);
    retain(ware_names);
    ioopm_list_iterator_t *name_iter = ioopm_list_iterator(ware_names);
    retain(name_iter);

    do
    {
        char *current_ware_name = ioopm_iterator_current(name_iter).p;
        retain(current_ware_name);

        merch_t *merch = ioopm_get_merch(store, current_ware_name);
        retain(merch);
        int ware_cost = merch->cost;
        int ware_amount = get_ware_amount(cart, current_ware_name);
        release(current_ware_name);
        release(merch);
        cost += ware_amount * ware_cost;
        release(name_iter);
        ioopm_iterator_next(name_iter, &name_error);
        retain(name_iter);

    } while (!name_error);

    ioopm_iterator_destroy(name_iter);
    release(name_iter);
    ioopm_linkedlist_destroy(ware_names);
    release(ware_names);
    return cost;
}

static bool cart_has_ware(cart_t *cart, char *name)
{
    return ioopm_hash_table_has_key(cart->wares, str_elem(name));
}

static bool remove_ware_from_cart(cart_t *cart, merch_t *merch)
{
    elem_t ret;
    return ioopm_hash_table_remove(cart->wares, str_elem(merch->name), &ret);
}

static void update_wares_in_cart(cart_t *cart, char *merchname, size_t amount)
{
    ioopm_hash_table_insert(cart->wares, str_elem(merchname), uint_elem(amount));
}

static void remove_shelf(ioopm_store_t *store, shelf_t *shelf, merch_t *merch)
{
    elem_t ret;
    ioopm_hash_table_remove(store->shelves, str_elem(shelf->shelfname), &ret);
    shelf_destroy(shelf);
}

static void ioopm_checkout_merch(ioopm_store_t *store, merch_t *merch, size_t amount)
{
    ioopm_list_t *merch_locations = NULL;
    if (ioopm_get_merch_locations(merch, &merch_locations))
    {
        retain(merch_locations);
        bool error = false;
        size_t remaining_to_remove = amount;

        ioopm_list_iterator_t *shelf_iter = ioopm_list_iterator(merch_locations);
        retain(shelf_iter);

        do
        {

            shelf_t *shelf = ioopm_iterator_current(shelf_iter).p;
            retain(shelf);
            size_t shelfstock = shelf->amount;

            if (shelfstock <= remaining_to_remove)
            {

                error = !ioopm_iterator_has_next(shelf_iter);
                ioopm_iterator_remove(shelf_iter);
                remove_shelf(store, shelf, merch);
                release(shelf);
                remaining_to_remove -= shelfstock;
            }
            else
            {
                update_shelf(store, shelf, remaining_to_remove, SUB);
                release(shelf_iter);
                release(shelf);
                ioopm_iterator_next(shelf_iter, &error);
                retain(shelf_iter);
            }

        } while (!error);

        ioopm_iterator_destroy(shelf_iter);
        release(shelf_iter);
        release(merch_locations);

        merch->amount -= amount;
        merch->reserved -= amount;
    }
}

static bool remove_cart_wares_from_store(ioopm_store_t *store, cart_t *cart)
{
    bool error = false;

    ioopm_list_t *merch_list = ioopm_hash_table_getkeys(cart->wares);
    retain(merch_list);

    ioopm_list_t *merch_amount_list = ioopm_hash_table_getvalues(cart->wares);
    retain(merch_amount_list);

    ioopm_list_iterator_t *merch_iter = ioopm_list_iterator(merch_list);
    retain(merch_iter);

    ioopm_list_iterator_t *merch_amount_iter = ioopm_list_iterator(merch_amount_list);
    retain(merch_amount_iter);

    do
    {
        char *merchname = ioopm_iterator_current(merch_iter).p;
        retain(merchname);

        elem_t checkout_amount = ioopm_iterator_current(merch_amount_iter);

        merch_t *merch = ioopm_get_merch(store, merchname);
        retain(merch);
        ioopm_checkout_merch(store, merch, checkout_amount.i);
        remove_ware_from_cart(cart, merch);
        release(merchname);
        release(merch);

        release(merch_iter);
        ioopm_iterator_next(merch_iter, &error);
        retain(merch_iter);
        release(merch_amount_iter);
        ioopm_iterator_next(merch_amount_iter, &error);
        retain(merch_amount_iter);
    } while (!error);

    ioopm_iterator_destroy(merch_iter);
    release(merch_iter);

    ioopm_iterator_destroy(merch_amount_iter);
    release(merch_amount_iter);

    ioopm_linkedlist_destroy(merch_list);
    release(merch_list);

    ioopm_linkedlist_destroy(merch_amount_list);
    release(merch_amount_list);

    return false;
}

static void edit_merch_in_cart(elem_t cartid, elem_t *cartelem, void *namesarray)
{
    cart_t *cart = cartelem->p;
    retain(cart);
    char **names = namesarray;

    if (cart_has_ware(cart, names[0]))
    {
        elem_t amount;
        ioopm_hash_table_remove(cart->wares, str_elem(names[0]), &amount);
        ioopm_hash_table_insert(cart->wares, str_elem(names[1]), amount);
    }
    release(cart);
}

static void edit_merch_in_carts(ioopm_store_t *store, char *names[])
{
    ioopm_hash_table_apply_all(store->carts, edit_merch_in_cart, names);
}

static void shelf_edit_merchname(size_t index, elem_t *value, void *newname)
{
    shelf_t *shelf = value->p;
    retain(shelf);
    shelf->item = (char *) newname;
    release(shelf);
}

static void edit_merch_in_shelf(merch_t *merch, char *newname)
{
    ioopm_list_t *shelflist = merch->locations;
    retain(shelflist);
    ioopm_linkedlist_apply_all(shelflist, shelf_edit_merchname, newname);
    release(shelflist);
}

// Utilities
//  To be used with bubblesort
static int shelfcomp(elem_t x, elem_t y)
{
    shelf_t *x_shelf = (shelf_t *) x.p;
    retain(x_shelf);
    shelf_t *y_shelf = (shelf_t *) y.p;
    retain(y_shelf);

    if (strcmp(x_shelf->shelfname, y_shelf->shelfname) > 0) {
        release(x_shelf);
        release(y_shelf);
        return true;
    } else {
        release(x_shelf);
        release(y_shelf);
        return false;
    }
}

static int strcomp(elem_t x, elem_t y) { return strcmp((char *) x.p, (char *) y.p) > 0; }

static char *ref_strdup(char *src)
{
    size_t len = strlen(src);
    char *str = allocate_array(len + 1, sizeof(char), NULL);
    retain(str);
    strcpy(str, src);
    return str;
}

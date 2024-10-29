#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "store_logic.h"
#include "ui.h"

void event_loop(ioopm_store_t *store)
{

    bool flag = true;

    while (flag)
    {
        char user_selection = ioopm_ask_question_menu("Choose an action");

        switch (user_selection)
        {
        case 'A':
            ioopm_store_add_item(store);
            break;

        case 'L':
            ioopm_store_list_merch(store);
            break;

        case 'D':
            ioopm_store_remove_item(store);
            break;

        case 'E':
            ioopm_store_edit_item(store);
            break;

        case 'S':
            ioopm_store_show_stock(store);
            break;

        case 'P':
            ioopm_store_replenish_item(store);
            break;

        case 'C':
            ioopm_store_create_cart(store);
            break;

        case 'R':
            ioopm_store_remove_cart(store);
            break;

        case '+':
            ioopm_store_add_to_cart(store);
            break;

        case '-':
            ioopm_store_remove_from_cart(store);
            break;

        case '=':
            ioopm_store_cart_calculate_cost(store);
            break;

        case 'O':
            ioopm_store_checkout(store);
            break;

        case 'Q':
            ioopm_store_quit(store, &flag);
            break;

        case 'U':
            //TODO: not implemented, optional
            ioopm_store_undo_action(store);
            break;

        default:
            printf("Invalid character. Please try again\n");
            break;
        }
    }
}

int main(void)
{
    ioopm_store_t *store = ioopm_store_create();

    event_loop(store);

    ioopm_store_destroy(store);

    return 0;
}

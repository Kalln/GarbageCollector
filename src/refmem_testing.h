// Header file for static refmem.c functions which are only exported for tests
#include "linked_list.h"
#include "refmem.h"
#include "refmem_internal.h"

extern ref_list_t *object_list;
extern ref_list_t *ptr_list;
extern size_t cascade_limit;
extern size_t freed_objects;

bool eq_fun(ref_elem_t ptr, ref_elem_t other_ptr);

void add_ptr_to_memory(obj *to_save);

void remove_ptr_from_memory(obj *to_remove);

void default_destructor(obj *o);

object_t *get_struct(obj *object);

bool get_struct_index(obj *object, int *index);

void destroy_object(object_t *object_struct);

size_t cleanup_helper(size_t start_index, size_t limit, size_t cleaned_up);

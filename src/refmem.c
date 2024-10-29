#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "refmem.h"
#include "refmem_internal.h"
#include "linked_list.h"

// Remove all instances of the static keyword for refmem unittests
#ifdef REFMEM_DISABLE_STATIC
// Replace static with nothing
#define static
#endif

static ref_list_t *object_list = NULL;
static ref_list_t *ptr_list = NULL;
static size_t cascade_limit = SIZE_MAX;
static size_t freed_objects = 0;

static bool eq_fun(ref_elem_t ptr, ref_elem_t other_ptr)
{
    return ptr.p == other_ptr.p ? true : false;
}

/// @brief Adds a given pointer to ptr_list. Used when default destructor is
///        used.
/// @param to_save the pointer to store in ptr_list

static void add_ptr_to_memory(obj *to_save)
{
    if (!ptr_list)
    {
        ptr_list = ref_linked_list_create(eq_fun);
    }

    if (to_save)
    {
        ref_linked_list_append(ptr_list, (ref_elem_t){.p = (to_save)});
        // ptr saved.
    }
}

/// @brief Removes a given pointer from ptr_list, that stores all allocated ptr.
///        This is used when the default destructor is in use.
/// @param to_remove the pointer to remove from the ptr_list.

static void remove_ptr_from_memory(obj *to_remove)
{
    if (ptr_list)
    {
        for (int i = 0; i < ref_linked_list_size(ptr_list); i += 1)
        {
            if (ref_linked_list_get(ptr_list, i).p == to_remove)
            {
                ref_linked_list_remove(ptr_list, i);
            }
        }
        if (ref_linked_list_size(ptr_list) == 0)
        {
            ref_linked_list_destroy(ptr_list);
            ptr_list = NULL;
        }
    }
}

/// @brief Get an object's struct from object_list
/// @param object the object whose struct we want to get
/// @return the object's struct or, if the there is no such, NULL
static object_t *get_struct(obj *object)
{
    if (object && object_list)
    {
        object_t *current_struct;
        for (int i = 0; i <= ref_linked_list_size(object_list); i++)
        {
            current_struct = ref_linked_list_get(object_list, i).p;
            if (current_struct->object == object)
            {
                return current_struct;
            }
        }
    }
    return NULL;
}

/// @brief  A default destructor that is used when NULL is given as an objects
///         destructor. On allocation a pointer is saved. This default destructor
///         looks for a saved pointer in every possible byte. If a match is found
///         the pointer will be freed.
/// @param o the object to destroy.
static void default_destructor(obj *o)
{
    object_t *object_struct = get_struct(o);

    if (!object_struct)
    {
        return;
    }
    size_t size = object_struct->size;

    unsigned long long start_memory = (unsigned long long)o;
    unsigned long long end_memory = start_memory + size - sizeof(void *);
    void **potential_ptr;

    uintptr_t p;

    for (p = start_memory; p <= end_memory; p += sizeof(void*))
    {
        if (!ptr_list || ref_linked_list_size(object_list) == 0) {
            break;
        }
        potential_ptr = (void **)p;

        if (potential_ptr) {
            if (ref_linked_list_contains(ptr_list, (ref_elem_t ){.p = *potential_ptr})){
                    // we found a match, this is the ptr we want to release.
                    release(*potential_ptr);
                }
        }
    }
}

/// @brief Get the index of an object's struct in object_list
/// @param object the object whose struct index we want to get
/// @param index where the index will be stored if the object's struct is found
/// @return true if the object's struct is found, false if it is not found

static bool get_struct_index(obj *object, int *index)
{
    if (object && object_list)
    {
        object_t *current_struct;
        for (int i = 0; i < ref_linked_list_size(object_list); i++)
        {
            current_struct = ref_linked_list_get(object_list, i).p;
            if (current_struct->object == object)
            {
                *index = i;
                return true;
            }
        }
    }
    return false;
}

void retain(obj *object)
{
    object_t *object_struct = get_struct(object);
    if (object_struct)
    {
        object_struct->rc++;
    }
}

void release(obj *object)
{
    object_t *object_struct = get_struct(object);
    if (object_struct)
    {
        if (rc(object) > 0)
        {
            object_struct->rc--;
        }
        if (rc(object) == 0)
        {
            remove_ptr_from_memory(object);
            deallocate(object);
        }
    }
    if (object_list != NULL && ref_linked_list_size(object_list) == 0)
    {
        ref_linked_list_destroy(object_list);
        object_list = NULL;
    }
}

size_t rc(obj *object)
{
    struct object *struct_object = get_struct(object);

    if (!object || !struct_object->object)
    {
        return 0; /* This might cause problems, since it "signals" that we need to deallocate. */
    }

    return struct_object->rc;
}

static void destroy_object(object_t *object_struct)
{
    object_struct->destructor(object_struct->object);

    free(object_struct->object);
    free(object_struct);
}

/// @brief Clean up up to `limit` objects with reference count 0
/// @param start_index The list index to start cleanup at
/// @param limit The maximum number of objects to deallocate
/// @return The combined size (in bytes) of all cleaned-up objects
static size_t cleanup_helper(size_t start_index, size_t limit, size_t cleaned_up)
{
    if (object_list == NULL || limit == 0 || start_index >= ref_linked_list_size(object_list))
    {
        return cleaned_up;
    }
    else
    {
        object_t *object_struct = ref_linked_list_get(object_list, start_index).p;
        if (rc(object_struct->object) == 0)
        {
            size_t object_size = object_struct->size;

            destroy_object(object_struct);
            ref_linked_list_remove(object_list, start_index);

            if (ref_linked_list_size(object_list) == 0) {
                ref_linked_list_destroy(object_list);
                object_list = NULL;
            }

            return cleanup_helper(start_index, limit - 1, cleaned_up + object_size);
        }
        else
        {
            return cleanup_helper(start_index + 1, limit, cleaned_up);
        }
    }
}

void cleanup(void)
{
    cleanup_helper(0, SIZE_MAX, 0);
}

obj *allocate(size_t bytes, function1_t destructor)
{
    /* ON first allocation, create the list. */
    if (!object_list)
    {
        object_list = ref_linked_list_create(NULL);
    }
    if (!destructor)
    {
        destructor = default_destructor;
    }

    size_t total_freed_memory = 0;
    size_t new_freed_memory;
    do
    {
        new_freed_memory = cleanup_helper(0, cascade_limit, total_freed_memory);
        total_freed_memory += new_freed_memory;
    } while (new_freed_memory != 0 && total_freed_memory < bytes);

    if (!object_list)
    {
        object_list = ref_linked_list_create(NULL);
    }

    object_t *result = calloc(1, sizeof(object_t));
    // add_ptr_to_memory(result);

    /* Check if allocation went well, result is NULL if it did not */
    if (result)
    {
        result->object = calloc(1, bytes);
        add_ptr_to_memory(result->object);
        result->rc = 0;
        result->destructor = destructor;
        result->size = bytes;
        ref_linked_list_append(object_list, (ref_elem_t){.p = (result)});
        return result->object;
    }
    else
    {
        return NULL;
    }
}

obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor)
{
    /* ON first allocation, create the list. */
    if (!object_list)
    {
        object_list = ref_linked_list_create(NULL);
    }

    /* If the required allocation size is larger than
       SIZE_MAX, the allocation is not possible, so return NULL */
    return elem_size == 0 || (elements < SIZE_MAX / elem_size)
               ? allocate(elements * elem_size, destructor)
               : NULL;
}

void deallocate(obj *object)
{
    /* Get object and index from list */
    int to_deallocate_index;
    object_t *to_deallocate = get_struct(object);

    /* If the object exists and rc is 0 then remove it from list and start deallocating it */
    if (to_deallocate && get_struct_index(object, &to_deallocate_index) && to_deallocate->rc == 0)
    {
        if (freed_objects < cascade_limit)
        {
            freed_objects++;
            destroy_object(to_deallocate);
            ref_linked_list_remove(object_list, to_deallocate_index);
        }
    }
    freed_objects = 0;
}

void set_cascade_limit(size_t limit)
{
    cascade_limit = limit;
}

size_t get_cascade_limit(void)
{
    return cascade_limit;
}

void shutdown(void)
{
    if (object_list != NULL)
    {
        while (ref_linked_list_size(object_list) > 0)
        {
            ref_elem_t object = ref_linked_list_get(object_list, 0);
            struct object *obj_struct = (struct object *)object.p;
            ref_linked_list_remove(object_list, 0);
            remove_ptr_from_memory(obj_struct->object);
            free(obj_struct->object);
            free(obj_struct);
        }
        ref_linked_list_destroy(object_list);
        object_list = NULL;
    }
    if (ptr_list != NULL) {
        ref_linked_list_destroy(ptr_list);
        ptr_list = NULL;
    }
}

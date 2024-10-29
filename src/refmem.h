#include <stddef.h>
#include "linked_list.h"
#pragma once

typedef void obj;
typedef void (*function1_t)(obj *);

/// @brief Increases refrence count by 1. Does nothing when called on NULL
/// @param object the object to operate on
void retain(obj *object);

/// @brief Decreases reference count by 1. If called on object with reference
/// count 1, object will be freed. Does nothing when called on NULL or object with
/// reference count 0.
/// @param object the object to operate on
void release(obj *object);

/// @brief Will return the amount of references an object has. 
///        Example: object A points towards object B, but object A has no other references, then the
///        reference count of object B will be 1, and the reference count of object A will be 0.
/// @param  object the object to get the reference count of
/// @return size_t the reference count of the object
size_t rc(obj *);

/// @brief Allocates memory for an object and the related reference count and destructor for that object
/// @param bytes The size of the object that we want to allocate space for
/// @param destructor A function used to deallocate smaller segments of an object, can also be null
/// @return A pointer to the allocated space for the object
obj *allocate(size_t bytes, function1_t destructor);

/// @brief Allocates memory for an object and the related reference count and destructor for that object
/// @param elements The number of elements that we want to allocate space for
/// @param elem_size The size of the object that we want to allocate space for
/// @param destructor A function used to deallocate smaller segments of an object, can also be null
/// @return A pointer to the allocated space for the object or NULL if the required allocation size
/// is greater than the cascade limit
obj *allocate_array(size_t elements, size_t elem_size, function1_t destructor);

/// @brief If the objects reference count is 0 this function will deallocate all memory related to the object
///        If the object could not be 
/// @param obj The object which we want to deallocate
void deallocate(obj *);

/// @brief Sets the cascade limit, i.e the maximum number of objects that will
/// be deallocated at a time. Any "leftover" objects will be freed at a later time.
/// @param limit The new cascade limit
/// @todo TODO: Perhaps we need a special value, e.g. 0 to signal that there is no limit, and all objects will be freed ASAP
void set_cascade_limit(size_t limit);

/// @brief Get the current cascade limit
/// @return The cascade limit
size_t get_cascade_limit(void);

/*Free all objects with reference count 0*/
void cleanup(void);

/// @brief Free's all allocated objects in the program
void shutdown(void);

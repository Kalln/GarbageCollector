// Type definitions for internal use in refmem.c and for use in refmem unit tests
#include "refmem.h"

typedef struct object object_t;
struct object
{
    /// @brief A pointer to the region containing the object
    obj *object;
    /// @brief The amount of active references to the object
    size_t rc;
    /// @brief The size of the object in bytes
    size_t size;
    /// @brief The destructor to run when the object is deallocated
    function1_t destructor;
};

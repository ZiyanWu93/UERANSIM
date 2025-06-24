#ifndef _CORE5G_MEMORY_UTILS_H
#define _CORE5G_MEMORY_UTILS_H

#include "allocator.h"
#include <stddef.h>

/**
 * @brief Initialize the global memory management system
 * 
 * @return true if initialization succeeded, false otherwise
 */
bool memory_system_init(void);

/**
 * @brief Clean up the global memory management system
 */
void memory_system_cleanup(void);

/**
 * @brief Get an allocator for a specific object type
 * 
 * @param object_size Size of objects to allocate
 * @param capacity Number of objects to pre-allocate
 * @return memory_allocator_t* Pointer to the allocator or NULL on failure
 */
memory_allocator_t* memory_get_allocator(size_t object_size, size_t capacity);

#endif /* _CORE5G_MEMORY_UTILS_H */

#ifndef _CORE5G_MEMORY_POOL_ALLOCATOR_H
#define _CORE5G_MEMORY_POOL_ALLOCATOR_H

#include "allocator.h"
#include <stdint.h>

/**
 * @brief Pool allocator implementation
 * 
 * Implements a pre-allocated pool of fixed-size objects with O(1)
 * allocation and deallocation using a free-list approach.
 */
typedef struct {
    // Memory block containing all pre-allocated objects
    void* memory_block;
    
    // Size of each object in bytes
    size_t object_size;
    
    // Total capacity (number of objects)
    size_t capacity;
    
    // Number of currently allocated objects
    size_t allocated_count;
    
    // Head of the free list
    void* free_list_head;
    
    // Base allocator interface
    memory_allocator_t base;
} pool_allocator_t;

/**
 * @brief Initialize a pool allocator
 * 
 * @param allocator Pointer to the allocator structure to initialize
 * @param object_size Size of each object in bytes
 * @param capacity Number of objects to pre-allocate
 * @return true if initialization succeeded, false otherwise
 */
bool pool_allocator_init(pool_allocator_t* allocator, size_t object_size, size_t capacity);

/**
 * @brief Destroy a pool allocator and free its resources
 * 
 * @param allocator Pointer to the allocator to destroy
 */
void pool_allocator_destroy(pool_allocator_t* allocator);

/**
 * @brief Get the base memory_allocator_t interface
 * 
 * @param allocator Pool allocator instance
 * @return memory_allocator_t* Interface pointer for generic use
 */
memory_allocator_t* pool_allocator_get_interface(pool_allocator_t* allocator);

#endif /* _CORE5G_MEMORY_POOL_ALLOCATOR_H */

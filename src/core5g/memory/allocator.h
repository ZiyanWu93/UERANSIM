#ifndef _CORE5G_MEMORY_ALLOCATOR_H
#define _CORE5G_MEMORY_ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Abstract memory allocator interface
 * 
 * This interface defines the basic operations for memory allocation
 * and deallocation with O(1) time complexity
 */
typedef struct {
    // Allocate an object of the size determined by the allocator
    void* (*allocate)(void* allocator_instance);
    
    // Deallocate an object previously allocated by this allocator
    void (*deallocate)(void* allocator_instance, void* ptr);

    // Check if a pointer was allocated from this allocator
    bool (*owns)(void* allocator_instance, void* ptr);
    
    // Get the size of objects managed by this allocator
    size_t (*get_object_size)(void* allocator_instance);
    
    // Get the total capacity (number of objects) of this allocator
    size_t (*get_capacity)(void* allocator_instance);
    
    // Get the number of currently allocated objects
    size_t (*get_allocated_count)(void* allocator_instance);

    // Reset the allocator state (deallocates all objects)
    void (*reset)(void* allocator_instance);

    // Implementation-specific data
    void* impl;
} memory_allocator_t;

#endif /* _CORE5G_MEMORY_ALLOCATOR_H */

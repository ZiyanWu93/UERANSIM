#include "pool_allocator.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Internal structure for free list nodes
// Uses memory overlay pattern where free-list pointers
// reuse memory of inactive objects
typedef struct free_node {
    struct free_node* next;
} free_node_t;

// Forward declarations of allocator operations
static void* pool_allocate(void* instance);
static void pool_deallocate(void* instance, void* ptr);
static bool pool_owns(void* instance, void* ptr);
static size_t pool_get_object_size(void* instance);
static size_t pool_get_capacity(void* instance);
static size_t pool_get_allocated_count(void* instance);
static void pool_reset(void* instance);

bool pool_allocator_init(pool_allocator_t* allocator, size_t object_size, size_t capacity) {
    if (!allocator || object_size < sizeof(free_node_t) || capacity == 0) {
        return false;
    }

    // Ensure object_size is at least large enough for the free list node
    size_t actual_object_size = (object_size > sizeof(free_node_t)) 
                              ? object_size 
                              : sizeof(free_node_t);

    // Allocate memory block for all objects
    void* memory_block = malloc(actual_object_size * capacity);
    if (!memory_block) {
        return false;
    }

    // Initialize allocator struct
    allocator->memory_block = memory_block;
    allocator->object_size = actual_object_size;
    allocator->capacity = capacity;
    allocator->allocated_count = 0;

    // Initialize the free list
    uint8_t* block = (uint8_t*)memory_block;
    allocator->free_list_head = block;
    
    // Build the free list chain
    for (size_t i = 0; i < capacity - 1; i++) {
        free_node_t* node = (free_node_t*)(block + i * actual_object_size);
        node->next = (free_node_t*)(block + (i + 1) * actual_object_size);
    }
    
    // Set the last node's next pointer to NULL
    free_node_t* last_node = (free_node_t*)(block + (capacity - 1) * actual_object_size);
    last_node->next = NULL;

    // Initialize the base interface
    allocator->base.allocate = pool_allocate;
    allocator->base.deallocate = pool_deallocate;
    allocator->base.owns = pool_owns;
    allocator->base.get_object_size = pool_get_object_size;
    allocator->base.get_capacity = pool_get_capacity;
    allocator->base.get_allocated_count = pool_get_allocated_count;
    allocator->base.reset = pool_reset;
    allocator->base.impl = allocator;

    return true;
}

void pool_allocator_destroy(pool_allocator_t* allocator) {
    if (allocator && allocator->memory_block) {
        free(allocator->memory_block);
        allocator->memory_block = NULL;
        allocator->free_list_head = NULL;
        allocator->allocated_count = 0;
    }
}

memory_allocator_t* pool_allocator_get_interface(pool_allocator_t* allocator) {
    return allocator ? &allocator->base : NULL;
}

static void* pool_allocate(void* instance) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    
    // Check if we have free objects
    if (!allocator->free_list_head) {
        return NULL;  // All objects are allocated
    }
    
    // Get the next free object
    void* object = allocator->free_list_head;
    
    // Update the free list head to the next free object
    allocator->free_list_head = ((free_node_t*)object)->next;
    
    // Increment allocated count
    allocator->allocated_count++;
    
    // Zero out the memory before returning
    memset(object, 0, allocator->object_size);
    
    return object;
}

static void pool_deallocate(void* instance, void* ptr) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    
    // Verify the pointer belongs to this allocator
    if (!pool_owns(instance, ptr)) {
        return;  // Not our pointer, ignore
    }
    
    // Add the object back to the free list
    free_node_t* node = (free_node_t*)ptr;
    node->next = (free_node_t*)allocator->free_list_head;
    allocator->free_list_head = node;
    
    // Decrement allocated count
    allocator->allocated_count--;
}

static bool pool_owns(void* instance, void* ptr) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    
    if (!allocator || !ptr || !allocator->memory_block) {
        return false;
    }
    
    // Check if the pointer is within our memory block
    uint8_t* block_start = (uint8_t*)allocator->memory_block;
    uint8_t* block_end = block_start + (allocator->object_size * allocator->capacity);
    
    // Verify pointer is in range and properly aligned
    return (ptr >= allocator->memory_block && 
            ptr < block_end && 
            ((uint8_t*)ptr - block_start) % allocator->object_size == 0);
}

static size_t pool_get_object_size(void* instance) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    return allocator ? allocator->object_size : 0;
}

static size_t pool_get_capacity(void* instance) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    return allocator ? allocator->capacity : 0;
}

static size_t pool_get_allocated_count(void* instance) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    return allocator ? allocator->allocated_count : 0;
}

static void pool_reset(void* instance) {
    pool_allocator_t* allocator = (pool_allocator_t*)instance;
    
    if (!allocator || !allocator->memory_block) {
        return;
    }
    
    // Rebuild the free list
    uint8_t* block = (uint8_t*)allocator->memory_block;
    allocator->free_list_head = block;
    
    for (size_t i = 0; i < allocator->capacity - 1; i++) {
        free_node_t* node = (free_node_t*)(block + i * allocator->object_size);
        node->next = (free_node_t*)(block + (i + 1) * allocator->object_size);
    }
    
    // Set the last node's next pointer to NULL
    free_node_t* last_node = (free_node_t*)(block + (allocator->capacity - 1) * allocator->object_size);
    last_node->next = NULL;
    
    // Reset allocated count
    allocator->allocated_count = 0;
}

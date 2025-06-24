#include "memory_utils.h"
#include "pool_allocator.h"
#include <stdlib.h>
#include <string.h>

// Maximum number of different object size allocators
#define MAX_ALLOCATORS 16

// Pre-defined common object sizes
#define SMALL_OBJECT_SIZE 64
#define MEDIUM_OBJECT_SIZE 256
#define LARGE_OBJECT_SIZE 1024

// Pre-allocate enough objects for common usage
#define SMALL_OBJECT_CAPACITY 128
#define MEDIUM_OBJECT_CAPACITY 64
#define LARGE_OBJECT_CAPACITY 32

// Global allocators
static pool_allocator_t g_small_object_allocator;
static pool_allocator_t g_medium_object_allocator;
static pool_allocator_t g_large_object_allocator;

// Dynamic allocators for custom object sizes
typedef struct {
    size_t object_size;
    size_t capacity;
    pool_allocator_t allocator;
} dynamic_allocator_t;

static dynamic_allocator_t g_dynamic_allocators[MAX_ALLOCATORS];
static size_t g_dynamic_allocator_count = 0;

bool memory_system_init(void) {
    // Initialize pre-defined allocators
    if (!pool_allocator_init(&g_small_object_allocator, SMALL_OBJECT_SIZE, SMALL_OBJECT_CAPACITY)) {
        return false;
    }

    if (!pool_allocator_init(&g_medium_object_allocator, MEDIUM_OBJECT_SIZE, MEDIUM_OBJECT_CAPACITY)) {
        pool_allocator_destroy(&g_small_object_allocator);
        return false;
    }

    if (!pool_allocator_init(&g_large_object_allocator, LARGE_OBJECT_SIZE, LARGE_OBJECT_CAPACITY)) {
        pool_allocator_destroy(&g_small_object_allocator);
        pool_allocator_destroy(&g_medium_object_allocator);
        return false;
    }

    // Initialize dynamic allocators array
    memset(g_dynamic_allocators, 0, sizeof(g_dynamic_allocators));
    g_dynamic_allocator_count = 0;

    return true;
}

void memory_system_cleanup(void) {
    // Clean up pre-defined allocators
    pool_allocator_destroy(&g_small_object_allocator);
    pool_allocator_destroy(&g_medium_object_allocator);
    pool_allocator_destroy(&g_large_object_allocator);

    // Clean up dynamic allocators
    for (size_t i = 0; i < g_dynamic_allocator_count; i++) {
        pool_allocator_destroy(&g_dynamic_allocators[i].allocator);
    }
    g_dynamic_allocator_count = 0;
}

memory_allocator_t* memory_get_allocator(size_t object_size, size_t capacity) {
    // Check pre-defined allocators first
    if (object_size <= SMALL_OBJECT_SIZE) {
        return pool_allocator_get_interface(&g_small_object_allocator);
    } else if (object_size <= MEDIUM_OBJECT_SIZE) {
        return pool_allocator_get_interface(&g_medium_object_allocator);
    } else if (object_size <= LARGE_OBJECT_SIZE) {
        return pool_allocator_get_interface(&g_large_object_allocator);
    }

    // Look for an existing dynamic allocator that can handle this size
    for (size_t i = 0; i < g_dynamic_allocator_count; i++) {
        if (g_dynamic_allocators[i].object_size >= object_size) {
            return pool_allocator_get_interface(&g_dynamic_allocators[i].allocator);
        }
    }

    // Create a new dynamic allocator if we have room
    if (g_dynamic_allocator_count < MAX_ALLOCATORS) {
        size_t index = g_dynamic_allocator_count;
        
        // Round up object size to nearest multiple of 8 for alignment
        size_t aligned_size = (object_size + 7) & ~7;
        
        if (pool_allocator_init(&g_dynamic_allocators[index].allocator, aligned_size, capacity)) {
            g_dynamic_allocators[index].object_size = aligned_size;
            g_dynamic_allocators[index].capacity = capacity;
            g_dynamic_allocator_count++;
            
            return pool_allocator_get_interface(&g_dynamic_allocators[index].allocator);
        }
    }

    // Could not create or find a suitable allocator
    return NULL;
}

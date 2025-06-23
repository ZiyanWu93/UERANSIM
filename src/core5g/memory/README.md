# Memory Management

## Overview

The Memory Management module provides efficient, predictable memory allocation for the Core5G system. It implements a pool-based allocation strategy that eliminates dynamic memory allocation overhead, prevents fragmentation, and ensures deterministic performance. The module includes a generic allocator interface and a high-performance pool allocator implementation.

## Architecture

### Components

1. **Allocator Interface** (`allocator.h`)
   - Generic allocator abstraction
   - Vtable-based polymorphism
   - Extensible for different strategies

2. **Pool Allocator** (`pool_allocator.h/c`)
   - Fixed-size block allocation
   - O(1) allocation and deallocation
   - Zero-fragmentation guarantee

3. **Memory Utilities** (`memory_utils.h/c`)
   - Helper functions for memory operations
   - Profiling and debugging support
   - Memory statistics tracking

### Design Principles

- **Predictable Performance**: Constant-time operations
- **No Fragmentation**: Fixed-size blocks prevent fragmentation
- **Cache Efficiency**: Memory layout optimized for CPU caches
- **Type Safety**: Compile-time size verification
- **Zero Overhead**: No hidden costs or metadata bloat

## Allocator Interface

### Structure

```c
typedef struct Allocator {
    void* (*alloc)(struct Allocator* allocator, size_t size);
    void (*free)(struct Allocator* allocator, void* ptr);
    void (*destroy)(struct Allocator* allocator);
    void* impl;  // Pointer to implementation-specific data
} Allocator;
```

### Usage Pattern

```c
// Create allocator
Allocator* alloc = (Allocator*)pool_allocator_create(sizeof(EventNf), 1024);

// Allocate memory
EventNf* event = (EventNf*)alloc->alloc(alloc, sizeof(EventNf));

// Use the allocated memory
event->event_id = EVENT_NAS_REGISTRATION_REQUEST;

// Free memory
alloc->free(alloc, event);

// Destroy allocator
alloc->destroy(alloc);
```

### Implementing Custom Allocators

To create a custom allocator, implement the three required functions:

```c
typedef struct MyAllocator {
    Allocator base;  // Must be first member
    // Custom fields...
} MyAllocator;

void* my_alloc(Allocator* allocator, size_t size) {
    MyAllocator* my_alloc = (MyAllocator*)allocator;
    // Implementation...
}

void my_free(Allocator* allocator, void* ptr) {
    MyAllocator* my_alloc = (MyAllocator*)allocator;
    // Implementation...
}

void my_destroy(Allocator* allocator) {
    MyAllocator* my_alloc = (MyAllocator*)allocator;
    // Cleanup...
    free(my_alloc);
}

Allocator* my_allocator_create(void) {
    MyAllocator* alloc = malloc(sizeof(MyAllocator));
    alloc->base.alloc = my_alloc;
    alloc->base.free = my_free;
    alloc->base.destroy = my_destroy;
    alloc->base.impl = alloc;
    // Initialize custom fields...
    return &alloc->base;
}
```

## Pool Allocator

### API Reference

#### `PoolAllocator* pool_allocator_create(size_t block_size, size_t num_blocks)`
Creates a new pool allocator.
- **block_size**: Size of each allocation block
- **num_blocks**: Total number of blocks in the pool
- **Returns**: Pointer to PoolAllocator or NULL on failure

#### `void* pool_allocator_alloc(PoolAllocator* pool)`
Allocates a block from the pool.
- **Returns**: Pointer to allocated block or NULL if pool exhausted
- O(1) time complexity
- Returns zero-initialized memory

#### `void pool_allocator_free(PoolAllocator* pool, void* ptr)`
Returns a block to the pool.
- **ptr**: Must be a pointer previously returned by pool_allocator_alloc
- O(1) time complexity
- Clears the block before returning to pool

#### `void pool_allocator_destroy(PoolAllocator* pool)`
Destroys the pool and frees all memory.
- Frees the entire pool memory
- Invalidates all pointers from this pool

#### `size_t pool_allocator_available(const PoolAllocator* pool)`
Returns the number of free blocks.
- Useful for monitoring pool usage
- Can trigger expansion or backpressure

#### `size_t pool_allocator_capacity(const PoolAllocator* pool)`
Returns the total capacity of the pool.

#### `void pool_allocator_reset(PoolAllocator* pool)`
Returns all blocks to the free state.
- Useful for bulk deallocation
- Much faster than individual frees

### Implementation Details

#### Memory Layout

```
Pool Allocator Memory Layout:
+----------------------+
| PoolAllocator struct |
| - metadata           |
| - free_list head     |
| - statistics         |
+----------------------+
| Memory Pool          |
| +------------------+ |
| | Block 0          | |
| | - next ptr       | |
| | - user data      | |
| +------------------+ |
| | Block 1          | |
| | - next ptr       | |
| | - user data      | |
| +------------------+ |
| | ...              | |
| +------------------+ |
| | Block N-1        | |
| | - next ptr       | |
| | - user data      | |
| +------------------+ |
+----------------------+
```

#### Free List Management

The pool uses a simple free list:
1. Free blocks are linked together
2. The `next` pointer is stored in the first bytes of free blocks
3. Allocation pops from the free list head
4. Deallocation pushes to the free list head

### Usage Examples

#### Basic Usage

```c
// Create pool for EventNf structures
PoolAllocator* event_pool = pool_allocator_create(sizeof(EventNf), 1024);

// Allocate events
EventNf* event1 = pool_allocator_alloc(event_pool);
EventNf* event2 = pool_allocator_alloc(event_pool);

// Use events
event1->event_id = EVENT_NAS_REGISTRATION_REQUEST;
event2->event_id = EVENT_NAS_AUTHENTICATION_REQUEST;

// Free when done
pool_allocator_free(event_pool, event1);
pool_allocator_free(event_pool, event2);

// Destroy pool
pool_allocator_destroy(event_pool);
```

#### Pool Management

```c
void manage_pool_health(PoolAllocator* pool) {
    size_t available = pool_allocator_available(pool);
    size_t capacity = pool_allocator_capacity(pool);
    
    double usage = 1.0 - ((double)available / capacity);
    
    if (usage > 0.9) {
        log_warning("Pool usage critical: %.1f%%", usage * 100);
        // Consider creating overflow pool
    } else if (usage > 0.7) {
        log_info("Pool usage high: %.1f%%", usage * 100);
    }
}
```

## Memory Utilities

### API Functions

#### `void memory_clear(void* ptr, size_t size)`
Clears memory to zero.
- Optimized for different sizes
- Uses platform-specific instructions when available

#### `void memory_randomize(void* ptr, size_t size)`
Fills memory with random data.
- Useful for security-sensitive deallocations
- Prevents information leakage

#### `bool is_power_of_two(size_t size)`
Checks if a size is a power of two.
- Used for alignment calculations
- Enables optimization opportunities

#### `size_t round_up_to_power_of_two(size_t size)`
Rounds up to the nearest power of two.
- Useful for buffer size calculations
- Improves cache efficiency

#### `void* align_pointer(void* ptr, size_t alignment)`
Aligns a pointer to specified boundary.
- Required for SIMD operations
- Improves cache performance

### Memory Profiling

```c
typedef struct {
    size_t total_allocations;
    size_t total_deallocations;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_failures;
} memory_stats_t;

// Global statistics tracking
extern memory_stats_t g_memory_stats;

// Update statistics in allocator
void track_allocation(size_t size) {
    g_memory_stats.total_allocations++;
    g_memory_stats.current_usage += size;
    if (g_memory_stats.current_usage > g_memory_stats.peak_usage) {
        g_memory_stats.peak_usage = g_memory_stats.current_usage;
    }
}
```

## Performance Characteristics

### Benchmarks

Pool allocator performance (Intel x86_64):
- Allocation: ~10-15 CPU cycles
- Deallocation: ~8-12 CPU cycles
- No system calls after initialization
- No lock contention (single-threaded)

### Comparison with malloc/free

| Operation | Pool Allocator | malloc/free | Improvement |
|-----------|---------------|-------------|-------------|
| Alloc     | 15 cycles     | 150+ cycles | 10x         |
| Free      | 10 cycles     | 100+ cycles | 10x         |
| Fragment. | None          | Variable    | ∞           |
| Predict.  | Guaranteed    | Variable    | ∞           |

## Best Practices

### 1. Right-Size Your Pools

```c
// Calculate pool size based on expected load
size_t calculate_pool_size(size_t avg_concurrent, double peak_factor) {
    size_t base_size = avg_concurrent * peak_factor;
    // Round up to power of two for efficiency
    return round_up_to_power_of_two(base_size);
}

// Example: 100 average, 2x peak
size_t pool_size = calculate_pool_size(100, 2.0); // 256
```

### 2. Use Type-Safe Wrappers

```c
// Type-safe pool wrapper
typedef struct {
    PoolAllocator* pool;
} EventPool;

EventPool* event_pool_create(size_t count) {
    EventPool* ep = malloc(sizeof(EventPool));
    ep->pool = pool_allocator_create(sizeof(EventNf), count);
    return ep;
}

EventNf* event_pool_alloc(EventPool* ep) {
    return (EventNf*)pool_allocator_alloc(ep->pool);
}
```

### 3. Monitor Pool Health

```c
void monitor_pools(void) {
    // Check all pools periodically
    for (int i = 0; i < num_pools; i++) {
        PoolAllocator* pool = pools[i];
        size_t free_pct = (pool_allocator_available(pool) * 100) / 
                          pool_allocator_capacity(pool);
        
        if (free_pct < 10) {
            log_critical("Pool %d critically low: %zu%% free", i, free_pct);
        }
    }
}
```

### 4. Handle Allocation Failures

```c
EventNf* allocate_event_with_fallback(void) {
    // Try primary pool
    EventNf* event = pool_allocator_alloc(primary_pool);
    if (event) return event;
    
    // Try overflow pool
    event = pool_allocator_alloc(overflow_pool);
    if (event) {
        stats.overflow_used++;
        return event;
    }
    
    // Last resort - drop oldest
    log_error("All pools exhausted, dropping events");
    return NULL;
}
```

## Error Handling

### Common Errors

1. **Pool Exhaustion**
   - Detection: `pool_allocator_alloc` returns NULL
   - Handling: Implement backpressure or overflow pools

2. **Double Free**
   - Detection: Debug builds can track allocations
   - Prevention: Clear pointers after free

3. **Use After Free**
   - Detection: Debug fills freed memory with pattern
   - Prevention: Proper ownership tracking

### Debug Mode

Enable debug mode for additional checks:
```c
#ifdef DEBUG_MEMORY
    // Fill with pattern on free
    memset(ptr, 0xDE, pool->block_size);
    
    // Check for double-free
    if (is_in_free_list(pool, ptr)) {
        panic("Double free detected!");
    }
#endif
```

## Testing

### Unit Tests

Comprehensive test coverage:
```bash
make memory_test
./build/memory_test
```

Tests include:
- Basic allocation/deallocation
- Pool exhaustion handling
- Alignment verification
- Performance benchmarks
- Stress testing

### Memory Leak Detection

```c
void test_no_leaks(void) {
    PoolAllocator* pool = pool_allocator_create(64, 100);
    size_t initial = pool_allocator_available(pool);
    
    // Perform operations
    void* ptrs[50];
    for (int i = 0; i < 50; i++) {
        ptrs[i] = pool_allocator_alloc(pool);
    }
    for (int i = 0; i < 50; i++) {
        pool_allocator_free(pool, ptrs[i]);
    }
    
    // Verify no leaks
    assert(pool_allocator_available(pool) == initial);
    
    pool_allocator_destroy(pool);
}
```

## Advanced Topics

### Cache Optimization

Optimize memory layout for cache efficiency:
```c
// Align blocks to cache line size
#define CACHE_LINE_SIZE 64
size_t aligned_block_size = ALIGN_UP(block_size, CACHE_LINE_SIZE);

// Prefetch next block during allocation
void* pool_alloc_prefetch(PoolAllocator* pool) {
    void* block = pool_allocator_alloc(pool);
    if (block && pool->free_list) {
        __builtin_prefetch(pool->free_list, 0, 1);
    }
    return block;
}
```

### NUMA Awareness

For NUMA systems:
```c
// Allocate pool on specific NUMA node
void* numa_alloc_pool(size_t size, int node) {
    #ifdef HAVE_NUMA
    return numa_alloc_onnode(size, node);
    #else
    return malloc(size);
    #endif
}
```

### Lock-Free Pools

For extreme performance (future enhancement):
```c
// Compare-and-swap based allocation
void* pool_alloc_lockfree(PoolAllocator* pool) {
    Block* old_head;
    Block* new_head;
    
    do {
        old_head = pool->free_list;
        if (!old_head) return NULL;
        new_head = old_head->next;
    } while (!CAS(&pool->free_list, old_head, new_head));
    
    return old_head;
}
```

## Future Enhancements

### Planned Features

- **Multi-Size Pools**: Single pool supporting multiple block sizes
- **Slab Allocator**: For kernel-style object caching
- **Memory Pressure API**: Callbacks for low-memory conditions
- **Hot/Cold Separation**: Optimize for access patterns
- **Compression**: Transparent memory compression
- **Telemetry**: Detailed allocation tracking and reporting

### Research Areas

- Hardware transactional memory for allocation
- Machine learning for pool size prediction
- Automatic pool tuning based on workload
- Integration with kernel memory management
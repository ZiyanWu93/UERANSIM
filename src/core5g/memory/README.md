# Memory Management

Efficient, predictable memory allocation using pool-based strategies.

## Overview

The Memory module provides:
- **Generic allocator interface** for different strategies
- **Pool allocator** with O(1) allocation/deallocation
- **Zero fragmentation** through fixed-size blocks
- **Memory utilities** for profiling and debugging

## Architecture

```
┌─────────────────┐
│ Allocator API   │ (Generic Interface)
├─────────────────┤
│ Pool Allocator  │ (Implementation)
├─────────────────┤
│ Memory Pool     │ [■■■□□□□□] (Pre-allocated blocks)
└─────────────────┘
```

## Allocator Interface

```c
typedef struct Allocator {
    void* (*alloc)(struct Allocator* allocator, size_t size);
    void (*free)(struct Allocator* allocator, void* ptr);
    void (*destroy)(struct Allocator* allocator);
    void* impl;
} Allocator;
```

## Key APIs

### Memory System
```c
bool memory_system_init(void)                        // Initialize
void memory_system_shutdown(void)                    // Cleanup
Allocator* memory_get_allocator(size_t size, size_t count)
```

### Pool Allocator
```c
PoolAllocator* pool_allocator_create(size_t block_size, size_t num_blocks)
void pool_allocator_destroy(PoolAllocator* allocator)
```

### Usage Pattern
```c
// Via memory system
memory_system_init();
Allocator* alloc = memory_get_allocator(sizeof(MyStruct), 100);
MyStruct* obj = (MyStruct*)alloc->alloc(alloc, sizeof(MyStruct));
alloc->free(alloc, obj);

// Direct pool allocator
PoolAllocator* pool = pool_allocator_create(sizeof(Event), 256);
Event* evt = (Event*)pool->allocator.alloc(&pool->allocator, sizeof(Event));
pool->allocator.free(&pool->allocator, evt);
pool_allocator_destroy(pool);
```

## Pool Allocator Details

- **Fixed-size blocks**: All allocations same size
- **Free list**: O(1) allocation via linked list
- **Bounds checking**: Validates pointers on free
- **Zero-init**: Optional clearing on allocation

## Performance

- **Allocation**: ~50ns per operation
- **Deallocation**: ~30ns per operation
- **Memory overhead**: < 1% (minimal metadata)
- **Cache friendly**: Contiguous memory layout

## Configuration

Pool parameters set at creation:
- Block size: Size of each allocation
- Block count: Total number of blocks
- Alignment: Automatic based on size

## Examples

Used by Core5G components:
- [Event Pool](../event_system/event_pool.c) - EventNf allocation
- [Mailbox](../mailbox/mailbox.c) - Integrated memory
- [Tests](memory_test.c) - Unit tests

## Custom Allocators

Implement the Allocator interface:
```c
typedef struct MyAllocator {
    Allocator base;  // Must be first
    // Custom fields...
} MyAllocator;

// Implement: my_alloc, my_free, my_destroy
```

## API Details

For complete documentation, see [API Reference](../docs/api-reference.md).
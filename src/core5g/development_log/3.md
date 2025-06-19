# Goal: Implement an efficient memory management system

## Memory Management
   - [X] Implement a pre-allocated pool of fixed-size objects with O(1) allocation/deallocation
   - [X] Design an abstract memory allocator interface for generic object management
   - [X] Implement a free-list allocator with constant-time object acquisition
   - [X] Use memory overlay pattern where free-list pointers reuse memory of inactive objects
   - [X] Provide intrusive tracking mechanism for allocation state to optimize cache usage
   - [X] Create simple API with allocate() and deallocate() operations, both O(1)
   - [X] Ensure memory locality optimization with contiguous storage for better cache performance
   - [X] Maintain fixed memory footprint regardless of object throughput
   - [X] Allow for future alternative allocator implementations without changing client code
   
## Current Status

The memory management system has been successfully implemented and can be used throughout the codebase. Below is a guide to its usage, starting with high-level patterns and proceeding to more detailed usage scenarios.

### Basic Concepts

The memory management system is built on three fundamental concepts:

1. **Allocators**: Specialized components that manage memory for objects of similar size. An allocator pre-allocates memory and efficiently hands out chunks as needed.

2. **Memory Pools**: Contiguous blocks of memory divided into fixed-size chunks. These pools enable O(1) allocation and deallocation with optimal cache locality.

3. **Free Lists**: Internal data structures that track available memory chunks. By using memory overlay techniques, these lists reuse the memory of inactive objects without additional overhead.

### Basic Usage

At its core, the memory system provides a simplified interface that abstracts away the underlying complexity. Most application code should only need to initialize the system, request allocators for specific object sizes, and use standard allocation/deallocation functions.

The system automatically selects the appropriate pre-defined allocator based on object size and manages the memory efficiently without requiring the caller to understand the implementation details. This approach provides both performance and ease of use.

```c
// Initialize the memory system at application startup
bool success = memory_system_init();

// Get an allocator for a specific object size
memory_allocator_t* allocator = memory_get_allocator(object_size, capacity);

// Use the allocator interface to allocate/deallocate objects
void* obj = allocator->allocate(allocator->impl);

// When done with the object
allocator->deallocate(allocator->impl, obj);

// Clean up at program termination
memory_system_cleanup();
```

### Intermediate Usage

When more specific control over memory resources is required, you can work directly with the pool allocator implementation. This approach is beneficial when:

1. **Resource Isolation**: You need dedicated memory pools for specific subsystems
2. **Lifecycle Management**: Different components have different memory lifecycles
3. **Performance Tuning**: You want to optimize pool sizes for specific object types
4. **Object Grouping**: You want to keep related objects in the same memory pool

By creating dedicated pool allocators, you gain more control over memory allocation patterns while still maintaining the O(1) performance benefits. The pool allocator provides additional functionalities like statistics tracking and bulk reset operations that can be valuable for performance-critical code.

```c
// Create a dedicated pool allocator for a specific object type
pool_allocator_t my_allocator;

// Initialize with object size and capacity
pool_allocator_init(&my_allocator, sizeof(my_object_t), 100);

// Get the standard interface
memory_allocator_t* allocator_if = pool_allocator_get_interface(&my_allocator);

// Use the standard interface for allocation/deallocation
my_object_t* obj = (my_object_t*)allocator_if->allocate(allocator_if->impl);

// Check allocator statistics
size_t allocated = allocator_if->get_allocated_count(allocator_if->impl);
size_t capacity = allocator_if->get_capacity(allocator_if->impl);
size_t obj_size = allocator_if->get_object_size(allocator_if->impl);

// Reset all allocations at once if needed
allocator_if->reset(allocator_if->impl);

// Clean up when done
pool_allocator_destroy(&my_allocator);
```

### Advanced Usage

The memory management system is designed to be extensible through its abstract interface. For specialized use cases or when the default pool allocator doesn't meet all requirements, you can implement custom allocators while maintaining compatibility with the rest of the system.

This extensibility is valuable in scenarios such as:

1. **Specialized Allocation Strategies**: When you need allocation patterns beyond simple pooling (e.g., buddy systems, slab allocators)
2. **Memory Constraints**: Working with hardware-specific memory regions or memory-mapped I/O
3. **Integration**: Adapting existing allocation systems to work with the memory_allocator_t interface
4. **Debugging**: Creating allocators with additional tracking or validation capabilities

The abstract allocator interface ensures that any custom implementation can be used interchangeably with the standard pool allocator. This allows for innovation in allocation strategies without requiring changes to the client code.

```c
// Define your custom allocator implementation
typedef struct {
    // Your implementation-specific fields
    void* memory;
    size_t object_size;
    // ...
    
    // Base interface (must be included)
    memory_allocator_t base;
} my_custom_allocator_t;

// Implement required operations
static void* my_allocate(void* instance) {
    my_custom_allocator_t* allocator = (my_custom_allocator_t*)instance;
    // Custom allocation logic here
    return allocated_memory;
}

static void my_deallocate(void* instance, void* ptr) {
    my_custom_allocator_t* allocator = (my_custom_allocator_t*)instance;
    // Custom deallocation logic here
}

// Initialize your custom allocator
void init_my_allocator(my_custom_allocator_t* allocator) {
    // Setup your allocator's internal state
    
    // Initialize the base interface
    allocator->base.allocate = my_allocate;
    allocator->base.deallocate = my_deallocate;
    allocator->base.owns = my_owns;
    // ... set other function pointers ...
    
    // Set the impl pointer to point to your allocator instance
    allocator->base.impl = allocator;
}
```

### Performance Optimization

To achieve maximum performance:

1. Use pre-sized allocators for objects with known, fixed sizes
2. Group related objects in the same allocator for better cache locality
3. Consider the memory access patterns of your application
4. For high-throughput components, initialize allocators with sufficient capacity
5. Avoid frequent allocation/deallocation cycles when possible

The memory system provides consistent O(1) performance for allocations and deallocations regardless of the system's state, making it ideal for real-time applications with strict timing requirements.
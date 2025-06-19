#include "memory_utils.h"
#include "pool_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Test object structure
typedef struct {
    int id;
    char data[32];
    double value;
} test_object_t;

// Simple test for the pool allocator
void test_pool_allocator() {
    printf("Testing pool allocator...\n");
    
    pool_allocator_t allocator;
    assert(pool_allocator_init(&allocator, sizeof(test_object_t), 10));
    
    memory_allocator_t* allocator_if = pool_allocator_get_interface(&allocator);
    
    // Allocate all objects
    void* objects[10];
    for (int i = 0; i < 10; i++) {
        objects[i] = allocator_if->allocate(allocator_if->impl);
        assert(objects[i] != NULL);
        
        // Cast to test object and set some data
        test_object_t* obj = (test_object_t*)objects[i];
        obj->id = i;
        sprintf(obj->data, "Test object %d", i);
        obj->value = i * 10.5;
        
        // Verify the object size
        assert(allocator_if->get_object_size(allocator_if->impl) == sizeof(test_object_t));
    }
    
    // Verify all objects are allocated
    assert(allocator_if->get_allocated_count(allocator_if->impl) == 10);
    
    // Try to allocate one more (should fail)
    assert(allocator_if->allocate(allocator_if->impl) == NULL);
    
    // Deallocate one object
    allocator_if->deallocate(allocator_if->impl, objects[5]);
    assert(allocator_if->get_allocated_count(allocator_if->impl) == 9);
    
    // Allocate again (should succeed)
    void* new_obj = allocator_if->allocate(allocator_if->impl);
    assert(new_obj != NULL);
    
    // Reset the allocator
    allocator_if->reset(allocator_if->impl);
    assert(allocator_if->get_allocated_count(allocator_if->impl) == 0);
    
    // Clean up
    pool_allocator_destroy(&allocator);
    printf("Pool allocator test passed!\n");
}

// Test for the memory system utilities
void test_memory_system() {
    printf("Testing memory system utilities...\n");
    
    assert(memory_system_init());
    
    // Test different sized object allocators
    memory_allocator_t* small_allocator = memory_get_allocator(32, 10);
    memory_allocator_t* medium_allocator = memory_get_allocator(128, 10);
    memory_allocator_t* large_allocator = memory_get_allocator(512, 5);
    
    assert(small_allocator != NULL);
    assert(medium_allocator != NULL);
    assert(large_allocator != NULL);
    
    // Allocate and test objects
    void* small_obj = small_allocator->allocate(small_allocator->impl);
    void* medium_obj = medium_allocator->allocate(medium_allocator->impl);
    void* large_obj = large_allocator->allocate(large_allocator->impl);
    
    assert(small_obj != NULL);
    assert(medium_obj != NULL);
    assert(large_obj != NULL);
    
    // Write some data to each object
    memset(small_obj, 1, 32);
    memset(medium_obj, 2, 128);
    memset(large_obj, 3, 512);
    
    // Free the objects
    small_allocator->deallocate(small_allocator->impl, small_obj);
    medium_allocator->deallocate(medium_allocator->impl, medium_obj);
    large_allocator->deallocate(large_allocator->impl, large_obj);
    
    // Get a custom allocator
    memory_allocator_t* custom_allocator = memory_get_allocator(sizeof(test_object_t), 5);
    assert(custom_allocator != NULL);
    
    // Allocate some objects
    test_object_t* obj1 = (test_object_t*)custom_allocator->allocate(custom_allocator->impl);
    test_object_t* obj2 = (test_object_t*)custom_allocator->allocate(custom_allocator->impl);
    
    assert(obj1 != NULL);
    assert(obj2 != NULL);
    
    // Set some data
    obj1->id = 100;
    strcpy(obj1->data, "Custom object 1");
    obj1->value = 123.456;
    
    obj2->id = 200;
    strcpy(obj2->data, "Custom object 2");
    obj2->value = 789.012;
    
    // Free the objects
    custom_allocator->deallocate(custom_allocator->impl, obj1);
    custom_allocator->deallocate(custom_allocator->impl, obj2);
    
    memory_system_cleanup();
    printf("Memory system test passed!\n");
}

int main() {
    printf("Memory Management System Tests\n");
    printf("=============================\n\n");
    
    // Run tests
    test_pool_allocator();
    printf("\n");
    
    test_memory_system();
    printf("\n");
    
    printf("All tests passed successfully!\n");
    
    return 0;
}

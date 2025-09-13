/**
 * @file set_utils.c
 * @brief Set data structure implementation for graph algorithms
 * @author Graph Theory Project Team
 * @date 2024
 *
 * This module provides a simple dynamic set implementation optimized for
 * storing vertex indices in graph algorithms. The set is implemented as
 * a dynamic array with basic operations for creation, insertion, removal,
 * and destruction.
 *
 * Key features:
 * - Dynamic array-based implementation
 * - Integer vertex storage
 * - Simple stack-like removal (LIFO)
 * - Memory management utilities
 *
 * The set does not maintain ordering or prevent duplicates - it's designed
 * for performance in graph traversal algorithms where these properties
 * are not required.
 *
 * Time Complexity: O(1) for all operations
 * Space Complexity: O(capacity) for storage
 */

#include "set_utils.h"

/**
 * @brief Creates a new set with specified initial capacity
 *
 * This function allocates memory for a new Set structure and initializes
 * it with the given capacity. The set starts empty and can grow up to
 * the specified capacity.
 *
 * @param capacity Maximum number of vertices the set can hold
 *
 * @return Pointer to newly created Set structure, NULL if allocation fails
 *
 * @complexity O(1) constant time allocation
 *
 * @pre capacity must be positive (> 0)
 * @post Returns initialized Set with size=0 and given capacity
 * @post Caller is responsible for calling set_destroy() to free memory
 *
 * @warning No bounds checking on capacity parameter
 * @warning Caller must check return value for NULL (allocation failure)
 *
 * @see set_destroy() for proper memory deallocation
 *
 * @example
 * Set *vertices = set_create(100);
 * if (vertices == NULL) {
 *     // Handle allocation failure
 * }
 */
Set *set_create(int capacity)
{
    Set *s = malloc(sizeof(Set));
    s->vertices = malloc(capacity * sizeof(int));
    s->size = 0;
    s->capacity = capacity;
    return s;
}

/**
 * @brief Adds a vertex to the set
 *
 * This function appends a vertex to the end of the set's internal array
 * if there is available capacity. The operation is performed in constant
 * time without checking for duplicates.
 *
 * @param s Pointer to the set to add vertex to
 * @param vertex Integer vertex identifier to add
 *
 * @complexity O(1) constant time insertion
 *
 * @pre s must be a valid Set pointer (not NULL)
 * @pre s->vertices must be properly allocated
 * @post If capacity allows, vertex is added and size is incremented
 * @post If capacity exceeded, operation is silently ignored
 * @post No duplicate checking is performed
 *
 * @warning Does not check for duplicate vertices
 * @warning Silently fails if set is at full capacity
 * @warning No NULL pointer validation performed
 *
 * @note This function does not resize the set automatically
 * @note Insertion order is preserved
 *
 * @example
 * Set *s = set_create(10);
 * set_add(s, 5);  // Add vertex 5 to set
 * set_add(s, 3);  // Add vertex 3 to set
 */
void set_add(Set *s, int vertex)
{
    if (s->size < s->capacity)
    {
        s->vertices[s->size++] = vertex;
    }
}

/**
 * @brief Removes the last added vertex from the set (LIFO removal)
 *
 * This function implements stack-like removal by decreasing the size
 * counter, effectively removing the most recently added vertex.
 * The underlying memory is not cleared or deallocated.
 *
 * @param s Pointer to the set to remove vertex from
 *
 * @complexity O(1) constant time removal
 *
 * @pre s must be a valid Set pointer (not NULL)
 * @pre s->size must be properly maintained
 * @post If set is not empty, size is decremented by 1
 * @post If set is empty, operation is silently ignored
 * @post Last vertex becomes inaccessible (but memory not cleared)
 *
 * @warning Does not clear the memory of removed vertex
 * @warning Silently fails if set is already empty
 * @warning No NULL pointer validation performed
 *
 * @note Follows Last-In-First-Out (LIFO) semantics
 * @note Does not actually deallocate memory for the removed element
 * @note Removed vertex data remains in memory until overwritten
 *
 * @example
 * Set *s = set_create(10);
 * set_add(s, 5);
 * set_add(s, 3);
 * set_remove(s);  // Removes vertex 3 (last added)
 */
void set_remove(Set *s)
{
    if (s->size > 0)
    {
        s->size--;
    }
}

/**
 * @brief Destroys the set and frees all allocated memory
 *
 * This function properly deallocates all memory associated with the set,
 * including both the vertices array and the Set structure itself.
 * After calling this function, the set pointer becomes invalid.
 *
 * @param s Pointer to the set to destroy
 *
 * @complexity O(1) constant time deallocation
 *
 * @pre s must be a valid Set pointer created by set_create()
 * @pre s->vertices must be properly allocated (by set_create)
 * @post All memory associated with the set is freed
 * @post Set pointer becomes invalid and must not be used
 *
 * @warning Using set pointer after destruction results in undefined behavior
 * @warning No NULL pointer validation performed
 * @warning Double-free will occur if called multiple times on same set
 *
 * @note This function must be called for every set created by set_create()
 * @note Failure to call this function will result in memory leaks
 *
 * @example
 * Set *s = set_create(10);
 * // ... use set ...
 * set_destroy(s);
 * s = NULL;  // Good practice to avoid accidental reuse
 */
void set_destroy(Set *s)
{
    free(s->vertices);
    free(s);
}
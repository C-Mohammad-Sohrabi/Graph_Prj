/**
 * @file set_utils.h
 * @brief Dynamic set data structure utilities for vertex collections
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module provides a simple dynamic set implementation for storing
 * collections of vertex indices. The set is implemented as a dynamic array
 * with automatic capacity management.
 * 
 * The set structure is used extensively throughout the graph algorithms
 * for storing:
 * - Clique vertices in clique detection algorithms
 * - Independent set vertices
 * - Vertex cover sets
 * - Candidate and excluded sets in recursive algorithms
 * 
 * Features:
 * - Dynamic resizing (manual management)
 * - Simple add/remove operations
 * - Memory management utilities
 * 
 * Time Complexity: O(1) for basic operations
 * Space Complexity: O(capacity) where capacity ≥ size
 */

#ifndef SET_UTILS_H
#define SET_UTILS_H

#include "structs.h"

/**
 * @brief Creates a new empty set with specified initial capacity
 * 
 * Allocates memory for a new Set structure and initializes its vertex array
 * with the given capacity. The set starts empty (size = 0) but can grow
 * up to the specified capacity without reallocation.
 * 
 * @param capacity Initial capacity (maximum number of vertices the set can hold)
 * @return Pointer to newly created Set structure, or NULL on allocation failure
 * 
 * @complexity O(1) time, O(capacity) space
 * 
 * @pre capacity must be > 0 for meaningful use
 * @post Returns initialized empty set with given capacity
 * @post Caller must call set_destroy() to free memory
 * 
 * @warning Returns NULL if memory allocation fails
 * @warning Caller is responsible for freeing returned set
 * 
 * @note Set size is initially 0, capacity determines maximum size
 * @note No automatic resizing - operations fail silently if capacity exceeded
 */
Set *set_create(int capacity);

/**
 * @brief Adds a vertex to the set
 * 
 * Appends a vertex to the end of the set if there is available capacity.
 * The operation fails silently if the set is already at full capacity.
 * 
 * @param s Pointer to the set structure
 * @param vertex Vertex index to add to the set
 * 
 * @complexity O(1)
 * 
 * @pre s must be a valid Set pointer created by set_create()
 * @pre vertex should be a valid vertex index (non-negative)
 * @post If space available: vertex is added and size is incremented
 * @post If no space: operation has no effect (fails silently)
 * 
 * @note Does not check for duplicate vertices
 * @note Does not validate vertex index
 * @note Fails silently when capacity is reached
 */
void set_add(Set *s, int vertex);

/**
 * @brief Removes the last vertex from the set
 * 
 * Removes the most recently added vertex by simply decrementing the size.
 * This effectively "pops" the last element. The operation fails silently
 * if the set is already empty.
 * 
 * @param s Pointer to the set structure
 * 
 * @complexity O(1)
 * 
 * @pre s must be a valid Set pointer
 * @post If set not empty: size is decremented by 1
 * @post If set empty: operation has no effect
 * 
 * @note Does not actually clear the memory of the removed vertex
 * @note Only decrements size counter - removed vertex data remains in array
 * @note Fails silently when set is empty
 */
void set_remove(Set *s);

/**
 * @brief Destroys a set and frees all associated memory
 * 
 * Deallocates all memory associated with a set, including both the vertex
 * array and the Set structure itself. After calling this function, the
 * set pointer becomes invalid and should not be used.
 * 
 * @param s Pointer to the set structure to destroy
 * 
 * @complexity O(1)
 * 
 * @pre s must be a valid Set pointer created by set_create(), or NULL
 * @post All memory associated with the set is freed
 * @post Set pointer becomes invalid and must not be used
 * 
 * @note Safe to call with NULL pointer (no operation performed)
 * @note After calling set_destroy(), the set pointer should be set to NULL
 * @note Frees both the vertices array and the Set structure
 */
void set_destroy(Set *s);

#endif
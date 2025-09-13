/**
 * @file independent_set.h
 * @brief Maximum independent set algorithms for undirected graphs
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module implements algorithms for finding maximum independent sets in graphs.
 * An independent set is a set of vertices with no edges between them.
 * 
 * The implementation uses the fundamental relationship between independent sets
 * and cliques: A maximum independent set in graph G corresponds to a maximum
 * clique in the complement graph G'.
 * 
 * Key concepts:
 * - Independent set: Set of vertices with no edges between any pair
 * - Maximum independent set: Largest possible independent set
 * - Complement graph: Graph where edges exist iff they don't exist in original
 * 
 * The module also provides minimum vertex cover computation using the relationship:
 * Minimum Vertex Cover = V \ Maximum Independent Set
 * 
 * Time Complexity: O(3^(n/3)) - same as maximum clique problem
 * Space Complexity: O(n²) for complement graph and intermediate structures
 */

#ifndef INDEPENDENT_SET_H
#define INDEPENDENT_SET_H

#include "structs.h"

/**
 * @brief Creates the complement of an undirected graph
 * 
 * Constructs a new graph where an edge exists between vertices u and v
 * if and only if there is no edge between u and v in the original graph.
 * Self-loops are never included in the complement.
 * 
 * The complement graph is used to transform the maximum independent set
 * problem into a maximum clique problem, since:
 * Maximum Independent Set in G = Maximum Clique in complement(G)
 * 
 * @param original Pointer to the original graph
 * @return Pointer to newly allocated complement graph, or NULL on error
 * 
 * @complexity O(n²) where n is the number of vertices
 * 
 * @pre original must be a valid undirected graph (is_directed = false)
 * @pre original must have initialized adjacency matrix
 * @post Returns a new Graph structure with complemented edges
 * @post Caller must free both adjacency matrix and Graph structure
 * 
 * @warning Returns NULL for directed graphs (not supported)
 * @warning Caller is responsible for freeing returned graph memory
 * 
 * @note Self-loops are always absent in the complement graph
 * @note The complement relationship: complement[i][j] = 1 - original[i][j] (for i ≠ j)
 */
Graph *create_complement_graph(Graph *original);

/**
 * @brief Finds maximum independent set using complement graph approach
 * 
 * Computes the maximum independent set by:
 * 1. Creating the complement graph of the input graph
 * 2. Finding the maximum clique in the complement graph
 * 3. The maximum clique in complement = maximum independent set in original
 * 
 * This approach leverages the theoretical equivalence between these problems
 * and reuses the efficient clique detection algorithms.
 * 
 * @param graph Pointer to the input graph
 * @return Pointer to Set containing maximum independent set vertices, or NULL on error
 * 
 * @complexity O(3^(n/3)) - dominated by maximum clique computation
 * 
 * @pre graph must be a valid undirected graph
 * @pre graph must have valid adjacency matrix
 * @post Returns Set with vertices forming maximum independent set
 * @post Caller must free returned Set using set_destroy()
 * 
 * @warning Returns NULL for directed graphs (not supported)
 * @warning Returns NULL if complement graph creation fails
 * 
 * @note The returned set contains vertex indices from the original graph
 * @note No two vertices in the returned set are adjacent in the original graph
 */
Set *find_maximum_independent_set(Graph *graph);

/**
 * @brief Finds minimum vertex cover using maximum independent set
 * 
 * Computes the minimum vertex cover using the fundamental relationship:
 * Minimum Vertex Cover = V \ Maximum Independent Set
 * 
 * Where V is the set of all vertices. This relationship holds because:
 * - Every edge must have at least one endpoint in the vertex cover
 * - If a set S is independent, then V \ S covers all edges
 * - The minimum vertex cover has size n - |maximum independent set|
 * 
 * @param graph Pointer to the input graph
 * @return Pointer to Set containing minimum vertex cover, or NULL on error
 * 
 * @complexity O(3^(n/3)) - dominated by maximum independent set computation
 * 
 * @pre graph must be a valid undirected graph
 * @pre graph must have valid adjacency matrix
 * @post Returns Set containing vertices that cover all edges
 * @post Every edge has at least one endpoint in the returned set
 * @post Caller must free returned Set using set_destroy()
 * 
 * @warning Returns NULL for directed graphs (not supported)
 * @warning Returns NULL if maximum independent set computation fails
 * 
 * @note The returned vertex cover is optimal (minimum size)
 * @note Running time is exponential, suitable for small to medium graphs
 */
Set *find_minimum_vertex_cover(Graph *graph);

#endif
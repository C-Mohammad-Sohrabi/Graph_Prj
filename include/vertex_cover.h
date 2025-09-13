/**
 * @file vertex_cover.h
 * @brief Multiple algorithms for minimum vertex cover problem
 * @author Graph Theory Project Team
 * @date 2024
 *
 * This module provides three different approaches to solve the minimum vertex cover problem:
 * 1. Exact algorithm via Maximum Independent Set (exponential but optimal)
 * 2. König's theorem for bipartite graphs (polynomial time, optimal for bipartite graphs)
 * 3. 2-approximation algorithm via maximal matching (fast, approximate)
 *
 * Vertex Cover Problem: Find the smallest set of vertices such that every edge
 * has at least one endpoint in the set.
 *
 * The problem is NP-hard for general graphs, but polynomial-time solvable
 * for bipartite graphs using König's theorem.
 *
 * Algorithm complexities:
 * - Exact via MIS: O(3^(n/3)) - exponential
 * - König (bipartite): O(E√V) - polynomial
 * - 2-approximation: O(E) - linear
 */

#ifndef VERTEX_COVER_H
#define VERTEX_COVER_H

#include "structs.h"
#include "set_utils.h"

/**
 * @brief Computes exact minimum vertex cover via Maximum Independent Set
 *
 * Uses the relationship: Minimum Vertex Cover = V \ Maximum Independent Set
 * This provides the optimal solution but has exponential running time.
 *
 * The algorithm:
 * 1. Finds the maximum independent set using complement graph approach
 * 2. Returns all vertices not in the maximum independent set
 *
 * This method guarantees the optimal solution but is only practical
 * for small to medium-sized graphs due to exponential complexity.
 *
 * @param graph Pointer to the input graph
 * @return Pointer to Set containing optimal vertex cover, or NULL on error
 *
 * @complexity O(3^(n/3)) exponential time complexity
 *
 * @pre graph must be a valid undirected graph
 * @pre graph->adjacency must be properly initialized
 * @post Returns optimal vertex cover (minimum size)
 * @post Every edge has at least one endpoint in returned set
 * @post Caller must free returned Set using set_destroy()
 *
 * @warning Not suitable for large graphs (n > 50) due to exponential complexity
 * @warning Returns NULL for directed graphs (not supported)
 */
Set *vertex_cover_exact_via_mis(Graph *graph);

/**
 * @brief Computes minimum vertex cover for bipartite graphs using König's theorem
 *
 * Implements König's theorem: In bipartite graphs, the size of maximum matching
 * equals the size of minimum vertex cover. The algorithm:
 * 1. Checks if the graph is bipartite using BFS coloring
 * 2. Finds maximum matching using Hopcroft-Karp algorithm
 * 3. Constructs minimum vertex cover using alternating paths
 *
 * König's theorem construction:
 * - Start from unmatched vertices in left partition
 * - Follow alternating paths (non-matching edges to right, matching edges to left)
 * - Vertex cover = (Left \ Visited) ∪ (Right ∩ Visited)
 *
 * @param graph Pointer to the input graph
 * @return Pointer to Set containing minimum vertex cover for bipartite graph, or NULL if not bipartite
 *
 * @complexity O(E√V) where E is edges and V is vertices
 *
 * @pre graph must be a valid undirected graph
 * @pre graph must be bipartite for optimal result
 * @post Returns optimal vertex cover if graph is bipartite
 * @post Returns NULL if graph is not bipartite
 * @post Caller must free returned Set using set_destroy()
 *
 * @note Automatically detects if graph is bipartite
 * @note Provides optimal solution for bipartite graphs in polynomial time
 * @note Much more efficient than exponential algorithms for bipartite graphs
 */
Set *vertex_cover_bipartite_konig(Graph *graph);

/**
 * @brief Fast 2-approximation algorithm using maximal matching
 *
 * Provides a fast approximation algorithm that guarantees a solution at most
 * twice the optimal size. The algorithm:
 * 1. Finds a maximal matching greedily
 * 2. Includes both endpoints of each matching edge in the vertex cover
 *
 * The approximation ratio is exactly 2 because:
 * - Each matching edge contributes 2 vertices to our cover
 * - Optimal cover needs at least 1 vertex per matching edge
 * - Therefore: |our cover| ≤ 2 × |optimal cover|
 *
 * @param graph Pointer to the input graph
 * @return Pointer to Set containing 2-approximate vertex cover (never NULL for valid input)
 *
 * @complexity O(E) where E is the number of edges
 *
 * @pre graph must be a valid undirected graph
 * @post Returns vertex cover with size ≤ 2 × optimal size
 * @post Every edge has at least one endpoint in returned set
 * @post Caller must free returned Set using set_destroy()
 *
 * @note Always returns a valid vertex cover (never NULL for valid graphs)
 * @note Very fast algorithm suitable for large graphs
 * @note Quality vs speed trade-off: fast but potentially 2x larger than optimal
 */
Set *vertex_cover_approx(Graph *graph);

/**
 * @brief Checks if graph is bipartite and returns partition
 *
 * Determines whether the input graph is bipartite (2-colorable) using BFS.
 * If bipartite, it provides the partition into two independent sets.
 *
 * Algorithm:
 * 1. Use BFS to color vertices with alternating colors (left/right partition)
 * 2. If any edge connects two vertices of the same color, graph is not bipartite
 * 3. Return boolean result and partition masks
 *
 * @param graph Pointer to the input graph
 * @param left_mask_out Pointer to store left partition mask (allocated by function)
 * @param right_mask_out Pointer to store right partition mask (allocated by function)
 * @return true if graph is bipartite, false otherwise
 *
 * @complexity O(V + E) where V is vertices and E is edges
 *
 * @pre graph must be a valid undirected graph
 * @pre left_mask_out and right_mask_out must be valid char** pointers
 * @post If bipartite: masks are allocated and filled (caller must free both)
 * @post If not bipartite: masks are not allocated
 * @post left_mask[i] = 1 if vertex i is in left partition, 0 otherwise
 * @post right_mask[i] = 1 if vertex i is in right partition, 0 otherwise
 *
 * @warning Caller must free both masks using free() if function returns true
 * @warning Returns false for directed graphs
 *
 * @note Uses BFS coloring algorithm for bipartiteness testing
 * @note Isolated vertices are assigned to left partition by default
 */
bool is_bipartite_partition(Graph *graph, char **left_mask_out, char **right_mask_out);

#endif
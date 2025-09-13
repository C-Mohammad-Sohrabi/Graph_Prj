/**
 * @file connectivity_number.h
 * @brief Vertex connectivity calculation and analysis
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module calculates the vertex connectivity (connectivity number) of graphs.
 * The vertex connectivity κ(G) is the minimum number of vertices whose removal
 * disconnects the graph or reduces it to a single vertex.
 * 
 * Key concepts:
 * - κ(G) = 0: Graph is already disconnected or has ≤1 vertex
 * - κ(G) = 1: Graph has articulation points (cut vertices)
 * - κ(G) = k: Graph is k-connected (remains connected after removing any k-1 vertices)
 * - κ(G) = n-1: Complete graph Kn
 * 
 * The module provides two approaches:
 * 1. Exact algorithm (brute force): O(2^n × (V+E)) - for small graphs (n ≤ 12)
 * 2. Approximation algorithm: O(V²) - uses minimum degree as upper bound
 * 
 * Properties:
 * - κ(G) ≤ λ(G) ≤ δ(G) (vertex connectivity ≤ edge connectivity ≤ minimum degree)
 * - For many graphs, κ(G) = δ(G)
 */

#ifndef CONNECTIVITY_NUMBER_H
#define CONNECTIVITY_NUMBER_H

#include "structs.h"

/**
 * @brief Calculates the vertex connectivity number of a graph
 * 
 * Main function that determines the vertex connectivity by choosing the
 * appropriate algorithm based on graph size:
 * - Small graphs (n ≤ 12): Uses exact brute force algorithm
 * - Large graphs (n > 12): Uses approximation based on minimum degree
 * 
 * For directed graphs, the function treats them as undirected for analysis
 * since directed vertex connectivity is more complex and less commonly needed.
 * 
 * @param graph Pointer to the input graph
 * @return Vertex connectivity number (minimum vertices to disconnect graph)
 * 
 * @complexity O(2^n × (V+E)) for exact, O(V²) for approximation
 * 
 * @pre graph must be a valid graph structure
 * @post Returns connectivity number without modifying graph
 * 
 * @note Returns 0 for NULL graphs or trivial cases
 * @note Prints warning for directed graphs (treated as undirected)
 * @note Prints note when using approximation for large graphs
 */
int calculate_connectivity_number(Graph *graph);

/**
 * @brief Checks if graph remains connected after removing specified vertices
 * 
 * Helper function that determines if a graph stays connected after removing
 * a given set of vertices. Uses DFS to check if all remaining vertices
 * are reachable from each other.
 * 
 * @param graph Pointer to the graph structure
 * @param removed_vertices Array of vertex indices to remove
 * @param removed_count Number of vertices to remove
 * @return true if graph remains connected after removal, false otherwise
 * 
 * @complexity O(V + E) where V is vertices and E is edges
 * 
 * @pre graph must have valid adjacency matrix
 * @pre removed_vertices must be valid array of size removed_count (can be NULL if removed_count is 0)
 * @pre removed vertex indices must be valid (0 ≤ index < node_count)
 * @post Returns connectivity status without modifying original graph
 * 
 * @note Returns true for graphs with ≤1 remaining vertex
 * @note Uses DFS from first non-removed vertex
 * @note Efficiently handles the case where all vertices are removed
 */
bool is_connected_after_removal(Graph *graph, int *removed_vertices, int removed_count);

/**
 * @brief Finds minimum vertex cut using brute force approach
 * 
 * Exhaustive algorithm that tries all possible combinations of k vertices
 * for k = 1, 2, ..., n-2 until it finds a combination whose removal disconnects
 * the graph. Returns the minimum k and optionally the actual cut vertices.
 * 
 * Algorithm:
 * 1. Check if graph is already disconnected (return 0)
 * 2. For k = 1 to n-2:
 *    - Generate all C(n,k) combinations of k vertices
 *    - Test if removing each combination disconnects the graph
 *    - Return k if disconnection found
 * 3. If no disconnection found, return n-1 (complete graph)
 * 
 * @param graph Pointer to the graph structure
 * @param cut_vertices_out Pointer to store cut vertices (optional, can be NULL)
 * @return Minimum number of vertices needed to disconnect the graph
 * 
 * @complexity O(2^n × (V+E)) exponential time complexity
 * 
 * @pre graph must be a valid graph with n ≥ 3 for meaningful results
 * @post If cut_vertices_out is not NULL, it's allocated and filled with cut vertices
 * @post Caller must free cut_vertices_out if it's allocated
 * 
 * @warning Only suitable for small graphs (n ≤ 12) due to exponential complexity
 * @note Returns exact vertex connectivity
 * @note More thorough but much slower than approximation methods
 */
int find_min_vertex_cut_bruteforce(Graph *graph, int **cut_vertices_out);

/**
 * @brief Approximation algorithm using minimum degree bound
 * 
 * Fast approximation that uses the theoretical bound κ(G) ≤ δ(G) where
 * δ(G) is the minimum degree. For many practical graphs, this bound is tight
 * (κ(G) = δ(G)), making it a reasonable approximation.
 * 
 * The algorithm simply finds the vertex with minimum degree and returns
 * that degree as the approximate vertex connectivity.
 * 
 * @param graph Pointer to the graph structure  
 * @param cut_vertices_out Pointer to store minimum degree vertex (optional)
 * @return Approximate vertex connectivity (minimum degree)
 * 
 * @complexity O(V²) to compute all degrees
 * 
 * @pre graph must be a valid graph structure
 * @post If cut_vertices_out is not NULL, contains minimum degree vertex
 * @post Result is an upper bound on actual vertex connectivity
 * 
 * @note Fast algorithm suitable for large graphs
 * @note May overestimate vertex connectivity, but provides reasonable approximation
 * @note Exact for many graph classes (regular graphs, complete graphs, etc.)
 */
int find_min_vertex_cut_maxflow(Graph *graph, int **cut_vertices_out);

/**
 * @brief Comprehensive connectivity number analysis and reporting
 * 
 * Performs detailed analysis of graph connectivity and prints comprehensive
 * results including:
 * - Graph statistics (vertices, edges, degree distribution)
 * - Connectivity number calculation
 * - Theoretical bounds and comparisons
 * - Graph properties and classification
 * - Algorithm used and performance notes
 * 
 * Analysis includes:
 * - Basic graph properties (size, density)
 * - Degree statistics (min, max, average)
 * - Connectivity classification (disconnected, 1-connected, 2-connected, etc.)
 * - Special cases identification (complete graphs, trees, etc.)
 * - Algorithm selection rationale
 * 
 * @param graph Pointer to the graph structure
 * 
 * @complexity Depends on graph size: O(2^n × (V+E)) for small, O(V²) for large
 * 
 * @pre graph must be a valid graph structure
 * @post Prints comprehensive analysis to stdout
 * @post All allocated memory is freed internally
 * 
 * @note Provides educational information about graph connectivity
 * @note Explains the connectivity number in context of graph theory
 * @note Handles edge cases and provides meaningful interpretation
 */
void analyze_connectivity_number(Graph *graph);

#endif
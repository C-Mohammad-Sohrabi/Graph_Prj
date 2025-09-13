/**
 * @file euler_path.h
 * @brief Eulerian path and cycle detection using Hierholzer's algorithm
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module implements Eulerian path and cycle detection for undirected graphs.
 * An Eulerian path visits every edge exactly once, while an Eulerian cycle
 * is an Eulerian path that starts and ends at the same vertex.
 * 
 * Theoretical background:
 * - Eulerian cycle exists iff all vertices have even degree and graph is connected
 * - Eulerian path exists iff exactly 0 or 2 vertices have odd degree and graph is connected
 * - If 0 vertices have odd degree → Eulerian cycle exists
 * - If 2 vertices have odd degree → Eulerian path exists (between the two odd-degree vertices)
 * 
 * The implementation uses Hierholzer's algorithm, which efficiently constructs
 * the Eulerian path/cycle by building a path and extending it when stuck.
 * 
 * Time Complexity: O(E) where E is the number of edges
 * Space Complexity: O(E) for storing the path and stack
 */

#ifndef EULER_PATH_H
#define EULER_PATH_H

#include "structs.h"
#include <stdbool.h>

/**
 * @brief Checks connectivity of vertices with non-zero degree
 * 
 * Verifies that all vertices with degree > 0 are connected, which is
 * a necessary condition for Eulerian paths/cycles. Uses DFS to check
 * if all non-isolated vertices are reachable from each other.
 * 
 * @param graph Pointer to the graph structure
 * @param degrees Array containing degree of each vertex
 * @return true if all non-zero degree vertices are connected, false otherwise
 * 
 * @complexity O(V + E) where V is vertices and E is edges
 * 
 * @pre graph must be a valid graph structure
 * @pre degrees must be an array of size graph->node_count with valid degree values
 * @post Returns connectivity status without modifying input
 * 
 * @note Ignores isolated vertices (degree 0) in connectivity check
 * @note Uses DFS traversal starting from first non-zero degree vertex
 */
bool is_connected_for_euler(Graph *graph, int *degrees);

/**
 * @brief Counts the degree (number of adjacent vertices) of a vertex
 * 
 * Simple utility function that counts the number of edges incident to
 * a given vertex by examining the adjacency matrix row.
 * 
 * @param graph Pointer to the graph structure
 * @param u Vertex index to count adjacencies for
 * @return Number of vertices adjacent to vertex u
 * 
 * @complexity O(V) where V is the number of vertices
 * 
 * @pre graph must have valid adjacency matrix
 * @pre u must be a valid vertex index (0 ≤ u < node_count)
 * @post Returns degree count without modifying graph
 * 
 * @note For undirected graphs, this gives the vertex degree
 * @note For directed graphs, this gives the out-degree
 */
int count_adj(Graph *graph, int u);

/**
 * @brief Finds and prints Eulerian path or cycle using Hierholzer's algorithm
 * 
 * Implements Hierholzer's algorithm to find Eulerian paths and cycles:
 * 
 * Algorithm steps:
 * 1. Check Eulerian conditions:
 *    - All non-zero degree vertices must be connected
 *    - For Eulerian cycle: all vertices have even degree
 *    - For Eulerian path: exactly 0 or 2 vertices have odd degree
 * 
 * 2. Hierholzer's algorithm:
 *    - Start from appropriate vertex (odd degree vertex for path, any vertex for cycle)
 *    - Use stack to build current path
 *    - When stuck (no more edges), backtrack and add to final path
 *    - Continue until all edges are used
 * 
 * 3. Output the complete Eulerian path/cycle
 * 
 * The algorithm maintains a working graph copy and removes edges as they're traversed
 * to avoid revisiting them.
 * 
 * @param original_graph Pointer to the input graph (remains unmodified)
 * 
 * @complexity O(E) where E is the number of edges
 * 
 * @pre original_graph must be a valid undirected graph
 * @pre original_graph adjacency matrix must be properly initialized
 * @post Prints the Eulerian path/cycle if it exists
 * @post Prints error message if no Eulerian path/cycle exists
 * @post Original graph remains unmodified
 * 
 * @warning Only works for undirected graphs (prints error for directed graphs)
 * @warning Prints results directly to stdout
 * 
 * @note If Eulerian cycle exists, can start from any vertex with non-zero degree
 * @note If only Eulerian path exists, must start from an odd-degree vertex
 * @note The printed path shows the sequence of vertices visited
 */
void find_euler_path(Graph *original_graph);

#endif 
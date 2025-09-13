/**
 * @file connectivity.h
 * @brief Graph connectivity analysis for directed and undirected graphs
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module provides comprehensive connectivity analysis for graphs.
 * For undirected graphs, it determines simple connectivity.
 * For directed graphs, it analyzes three types of connectivity:
 * - Strong connectivity: Every vertex is reachable from every other vertex
 * - Weak connectivity: Connected when edges are treated as undirected
 * - One-sided connectivity: For any pair of vertices, at least one can reach the other
 * 
 * The implementation uses BFS (Breadth-First Search) traversal to determine
 * reachability between vertices.
 * 
 * Time Complexity: O(V + E) where V is vertices and E is edges
 * Space Complexity: O(V) for the visited array and queue
 */

#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include "structs.h"

/**
 * @brief Analyzes connectivity properties of a graph
 * 
 * This function performs comprehensive connectivity analysis based on the graph type:
 * 
 * For undirected graphs:
 * - Performs BFS from vertex 0 to check if all vertices are reachable
 * - Sets is_connected to true if the graph is connected
 * 
 * For directed graphs:
 * - Strong connectivity: BFS following directed edges to check if all vertices
 *   are reachable from vertex 0
 * - Weak connectivity: BFS treating all edges as bidirectional
 * - One-sided connectivity: Checks if for every pair of vertices, at least
 *   one can reach the other (currently simplified implementation)
 * 
 * @param graph Pointer to the Graph structure to analyze
 * @return Connectivity structure containing analysis results
 * 
 * @complexity O(V + E) where V is number of vertices, E is number of edges
 * 
 * @pre graph must be a valid Graph pointer with initialized adjacency matrix
 * @pre graph->node_count must be positive
 * @post Returns connectivity analysis without modifying the input graph
 * 
 * @note For directed graphs, the function assumes vertex 0 exists for analysis
 * @note Empty graphs (node_count = 0) are considered connected
 */
Connectivity check_connectivity(Graph *graph);

#endif
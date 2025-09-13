/**
 * @file havel_hakimi.h
 * @brief Graph construction from degree sequences using Havel-Hakimi algorithm
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module implements the Havel-Hakimi algorithm for constructing graphs
 * from given degree sequences. It supports both directed and undirected graphs
 * and validates whether a degree sequence is graphical (realizable by a simple graph).
 * 
 * The Havel-Hakimi algorithm works by:
 * 1. Sorting vertices by degree in descending order
 * 2. Connecting the highest-degree vertex to the next d highest-degree vertices
 * 3. Reducing degrees accordingly and repeating until all degrees are zero
 * 
 * Time Complexity: O(n²) where n is the number of vertices
 * Space Complexity: O(n²) for the adjacency matrix
 */

#ifndef HAVEL_HAKIMI_H
#define HAVEL_HAKIMI_H

#include "structs.h"

 /**
 * @brief Comparison function for sorting nodes by degree (descending order)
 * 
 * This function is used by qsort to arrange nodes in descending order of their
 * degree. If degrees are equal, it sorts by original index in ascending order
 * to ensure deterministic behavior.
 * 
 * @param a Pointer to first Node structure
 * @param b Pointer to second Node structure
 * @return Negative if a > b, positive if a < b, zero if equal
 * 
 * @complexity O(1)
 */
int compare_nodes(const void *a, const void *b);

/**
 * @brief Comparison function for sorting nodes by in-degree (descending order)
 * 
 * Similar to compare_nodes but uses in-degree for directed graph processing.
 * Used specifically in directed graph Havel-Hakimi algorithm.
 * 
 * @param a Pointer to first Node structure
 * @param b Pointer to second Node structure
 * @return Negative if a > b, positive if a < b, zero if equal
 * 
 * @complexity O(1)
 */
int compare_nodes_in_degree(const void *a, const void *b);

/**
 * @brief Constructs an undirected graph using Havel-Hakimi algorithm
 * 
 * Implements the classical Havel-Hakimi algorithm for undirected graphs.
 * The algorithm repeatedly:
 * 1. Sorts vertices by degree in descending order
 * 2. Takes the vertex with highest degree d
 * 3. Connects it to the next d vertices with highest degrees
 * 4. Reduces their degrees and repeats
 * 
 * The function also generates a DOT file for visualization and populates
 * the graph structure's adjacency matrix.
 * 
 * @param nodes Array of Node structures with degree information
 * @param n Number of vertices in the graph
 * @param dot_file Open file pointer for DOT output
 * @param graph Graph structure to populate with adjacency matrix
 * @return true if the degree sequence is graphical, false otherwise
 * 
 * @complexity O(n²) where n is the number of vertices
 * 
 * @pre nodes must be a valid array of size n
 * @pre dot_file must be an open file pointer with write permissions
 * @pre graph must be a valid Graph pointer
 * @post If successful, graph contains the constructed adjacency matrix
 * @post DOT file contains the graph representation for visualization
 */
bool havel_hakimi_undirected(Node *nodes, int n, FILE *dot_file, Graph *graph);

/**
 * @brief Constructs a directed graph using modified Havel-Hakimi algorithm
 * 
 * Implements a modified Havel-Hakimi algorithm for directed graphs that handles
 * both in-degree and out-degree sequences. The algorithm:
 * 1. Maintains separate in-degree and out-degree for each vertex
 * 2. Iteratively connects vertices with highest out-degree to vertices with highest in-degree
 * 3. Optionally allows bidirectional edges based on the allow_bidirectional flag
 * 
 * @param nodes Array of Node structures with out-degree and in-degree information
 * @param n Number of vertices in the graph
 * @param dot_file Open file pointer for DOT output (digraph format)
 * @param graph Graph structure to populate with adjacency matrix
 * @param allow_bidirectional If true, allows bidirectional edges in the directed graph
 * @return true if both degree sequences are graphical, false otherwise
 * 
 * @complexity O(n²) where n is the number of vertices
 * 
 * @pre nodes must contain valid out-degree (degree field) and in-degree values
 * @pre Sum of out-degrees must equal sum of in-degrees
 * @pre dot_file must be an open file pointer with write permissions
 * @post If successful, graph contains the constructed directed adjacency matrix
 * @post DOT file contains the directed graph representation
 */
bool havel_hakimi_directed(Node *nodes, int n, FILE *dot_file, Graph *graph, bool allow_bidirectional);

#endif
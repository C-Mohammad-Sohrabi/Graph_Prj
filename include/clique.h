/**
 * @file clique.h
 * @brief Clique detection algorithms for undirected graphs
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module implements various algorithms for finding cliques in undirected graphs.
 * A clique is a subset of vertices where every pair of vertices is adjacent.
 * 
 * The module provides two main approaches:
 * 1. Backtracking algorithm: Finds all possible cliques (may include duplicates)
 * 2. Bron-Kerbosch algorithm: Finds all maximal cliques efficiently using branch-and-bound
 * 
 * Key concepts:
 * - Clique: Complete subgraph where every pair of vertices is connected
 * - Maximal clique: A clique that cannot be extended by adding another vertex
 * - Maximum clique: The largest maximal clique in the graph
 * 
 * Time Complexity: O(3^(n/3)) in worst case (exponential)
 * Space Complexity: O(n²) for storing cliques and intermediate sets
 */

#ifndef CLIQUE_H
#define CLIQUE_H

#include "structs.h"

/**
 * @brief Finds all cliques using backtracking algorithm
 * 
 * This function implements a recursive backtracking algorithm to enumerate
 * all possible cliques in the graph. The algorithm explores all possible
 * combinations of vertices and checks if they form a clique.
 * 
 * The function uses three sets:
 * - C: Current clique being built
 * - P: Candidate vertices that can extend the current clique
 * - S: Vertices already processed (excluded from further consideration)
 * 
 * @param graph Pointer to the graph structure
 * @param C Current clique set (initially empty)
 * @param P Candidate set (initially all vertices)
 * @param S Excluded set (initially empty)
 * @param all_cliques Pointer to array of clique pointers (dynamically allocated)
 * @param count Pointer to counter of found cliques
 * 
 * @complexity O(3^(n/3)) exponential time complexity
 * 
 * @pre graph must be an undirected graph with valid adjacency matrix
 * @pre C, P, S must be valid Set pointers
 * @pre all_cliques must point to a valid Set** (can be NULL initially)
 * @pre count must point to a valid integer (initialized to 0)
 * @post all_cliques contains all found cliques (caller must free)
 * @post count contains the total number of cliques found
 * 
 * @warning This algorithm may find duplicate cliques due to different discovery paths
 * @warning Memory for all_cliques and individual cliques must be freed by caller
 */
void find_all_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***all_cliques, int *count);

/**
 * @brief Finds maximal cliques using Bron-Kerbosch algorithm with pivot
 * 
 * Implements the Bron-Kerbosch algorithm with pivot selection for finding
 * all maximal cliques efficiently. The algorithm uses a pivot heuristic to
 * reduce the number of recursive calls and improve performance.
 * 
 * The pivot is selected as the vertex with maximum intersection with the
 * candidate set P, which helps minimize the branching factor.
 * 
 * Algorithm steps:
 * 1. If P and S are both empty, C is a maximal clique
 * 2. Choose pivot u with maximum |N(u) ∩ P|
 * 3. For each v in P \ N(u):
 *    - Recursively call with C ∪ {v}, P ∩ N(v), S ∩ N(v)
 *    - Move v from P to S
 * 
 * @param graph Pointer to the graph structure
 * @param C Current clique set
 * @param P Candidate set
 * @param S Excluded set
 * @param maximal_cliques Pointer to array of maximal clique pointers
 * @param count Pointer to counter of found maximal cliques
 * 
 * @complexity O(3^(n/3)) worst case, but typically much better with pivot
 * 
 * @pre graph must be an undirected graph
 * @pre All set parameters must be valid Set pointers
 * @post maximal_cliques contains all maximal cliques (no subsets)
 * @post count contains the number of maximal cliques found
 * 
 * @note This algorithm guarantees no duplicate maximal cliques
 * @note More efficient than find_all_cliques for most practical graphs
 */
void find_maximal_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***maximal_cliques, int *count);

/**
 * @brief Finds one maximum clique (largest among all maximal cliques)
 * 
 * This function finds the maximum clique by first finding all maximal cliques
 * using the Bron-Kerbosch algorithm, then selecting the largest one.
 * 
 * @param graph Pointer to the graph structure
 * @return Pointer to Set containing the maximum clique vertices, or NULL if none found
 * 
 * @complexity O(3^(n/3)) same as finding all maximal cliques
 * 
 * @pre graph must be a valid undirected graph
 * @post Returns the largest maximal clique found
 * @post Caller must free the returned Set using set_destroy()
 * 
 * @note If multiple maximum cliques exist, returns one of them
 * @note Returns NULL for empty graphs or graphs with no edges
 */
Set *find_maximum_clique(Graph *graph);

/**
 * @brief Analyzes and prints clique information for a graph
 * 
 * Convenience function that performs clique analysis and prints results.
 * Allows choosing between two different algorithms and provides formatted
 * output showing maximum clique size and all non-trivial cliques (size ≥ 3).
 * 
 * @param graph Pointer to the graph structure
 * @param algorithm_choice 1 for backtracking (all cliques), 2 for Bron-Kerbosch (maximal cliques)
 * 
 * @complexity Depends on chosen algorithm: O(3^(n/3))
 * 
 * @pre graph must be a valid undirected graph
 * @pre algorithm_choice must be 1 or 2
 * @post Prints clique analysis results to stdout
 * @post All dynamically allocated memory is freed
 * 
 * @note This function handles all memory management internally
 * @note Only prints cliques with size ≥ 3 (non-trivial cliques)
 */
void analyze_cliques(Graph *graph, int algorithm_choice);

#endif
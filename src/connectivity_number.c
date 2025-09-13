/**
 * @file connectivity_number.c
 * @brief Vertex connectivity algorithms for graph analysis
 * @author Graph Theory Project Team
 * @date 2024
 *
 * This module implements algorithms for computing the vertex connectivity number
 * of undirected graphs. Vertex connectivity (κ(G)) is the minimum number of
 * vertices whose removal disconnects the graph or reduces it to a single vertex.
 *
 * The module provides:
 * 1. Exact algorithms for small graphs using brute force enumeration
 * 2. Approximation algorithms for large graphs using min-degree heuristic
 * 3. Comprehensive analysis and reporting functions
 *
 * Key concepts:
 * - Vertex Connectivity: Minimum vertices to remove for disconnection
 * - k-Connected Graph: Graph requiring removal of ≥k vertices to disconnect
 * - Vertex Cut: Set of vertices whose removal disconnects the graph
 * - Whitney's Theorem: κ(G) ≤ λ(G) ≤ δ(G) (vertex ≤ edge ≤ min degree)
 *
 * Time Complexity: O(2^n) for exact (small graphs), O(n²) for approximation
 * Space Complexity: O(n²) for adjacency matrix and auxiliary structures
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "connectivity_number.h"
#include "structs.h"

/**
 * @brief Performs DFS to count reachable vertices after vertex removal
 *
 * This helper function implements depth-first search while avoiding removed
 * vertices to determine connectivity after vertex removal. It counts the
 * number of vertices reachable from a given start vertex.
 *
 * @param graph Pointer to the graph structure
 * @param start Starting vertex for DFS traversal
 * @param visited Boolean array tracking visited vertices
 * @param removed_vertices Array of vertex indices to avoid during traversal
 * @param removed_count Number of vertices in removed_vertices array
 * @param reachable_count Pointer to counter for reachable vertices
 *
 * @complexity O(V + E) for DFS traversal
 *
 * @pre graph must have valid adjacency matrix
 * @pre visited array must be allocated with size ≥ node_count
 * @pre removed_vertices must be valid array or NULL if removed_count = 0
 * @pre reachable_count must point to valid integer
 * @post visited array is modified to reflect DFS traversal
 * @post reachable_count is incremented for each reachable vertex
 *
 * @note Recursive implementation - may cause stack overflow for very large graphs
 * @note Does not reset visited array - caller responsibility
 */
static void dfs_reachable(Graph *graph, int start, bool *visited, int *removed_vertices, int removed_count, int *reachable_count)
{
    /* Check if start vertex is removed */
    for (int i = 0; i < removed_count; i++)
    {
        if (removed_vertices[i] == start)
            return;
    }

    visited[start] = true;
    (*reachable_count)++;

    for (int v = 0; v < graph->node_count; v++)
    {
        if (graph->adjacency[start][v] && !visited[v])
        {
            /* Check if v is removed */
            bool is_removed = false;
            for (int i = 0; i < removed_count; i++)
            {
                if (removed_vertices[i] == v)
                {
                    is_removed = true;
                    break;
                }
            }
            if (!is_removed)
            {
                dfs_reachable(graph, v, visited, removed_vertices, removed_count, reachable_count);
            }
        }
    }
}

/**
 * @brief Checks if graph remains connected after removing specified vertices
 *
 * This function determines whether a graph remains connected after removing
 * a specific set of vertices by performing DFS from any remaining vertex
 * and checking if all non-removed vertices are reachable.
 *
 * Algorithm steps:
 * 1. Find first non-removed vertex as starting point
 * 2. Perform DFS avoiding removed vertices
 * 3. Compare reachable count with total remaining vertices
 *
 * @param graph Pointer to the graph structure
 * @param removed_vertices Array of vertex indices to remove (can be NULL)
 * @param removed_count Number of vertices to remove (0 if none)
 *
 * @return true if graph remains connected, false if disconnected
 *
 * @complexity O(V + E) for single DFS traversal
 *
 * @pre graph must be valid with proper adjacency matrix
 * @pre removed_vertices must be valid array if removed_count > 0
 * @pre removed_count must be ≤ node_count
 * @post Original graph structure unchanged
 * @post Returns true for trivial graphs (≤1 vertices)
 *
 * @note Returns false if all vertices are removed
 * @note Handles NULL removed_vertices array gracefully
 *
 * @example
 * int removed[] = {2, 5};
 * bool connected = is_connected_after_removal(graph, removed, 2);
 */
bool is_connected_after_removal(Graph *graph, int *removed_vertices, int removed_count)
{
    if (!graph || graph->node_count <= 1)
        return true;

    int n = graph->node_count;
    bool *visited = calloc(n, sizeof(bool));

    /* Find first non-removed vertex as start point */
    int start = -1;
    for (int i = 0; i < n; i++)
    {
        bool is_removed = false;
        for (int j = 0; j < removed_count; j++)
        {
            if (removed_vertices[j] == i)
            {
                is_removed = true;
                break;
            }
        }
        if (!is_removed)
        {
            start = i;
            break;
        }
    }

    if (start == -1)
    {
        free(visited);
        return false; /* All vertices removed */
    }

    /* Count reachable vertices from start */
    int reachable_count = 0;
    dfs_reachable(graph, start, visited, removed_vertices, removed_count, &reachable_count);

    /* Count total non-removed vertices */
    int total_remaining = n - removed_count;

    free(visited);
    return (reachable_count == total_remaining);
}

/**
 * @brief Generates next lexicographic combination using algorithm
 *
 * This utility function generates combinations in lexicographic order for
 * systematic enumeration of vertex subsets. Used internally by brute force
 * algorithms to iterate through all possible vertex cuts.
 *
 * Algorithm: Standard next combination generation using rightmost increment
 * and cascade update pattern.
 *
 * @param combination Array representing current combination (modified in-place)
 * @param k Size of combination (number of elements to choose)
 * @param n Total number of elements to choose from
 *
 * @return true if next combination generated, false if no more combinations
 *
 * @complexity O(k) worst case for cascade updates
 *
 * @pre combination must be valid array of size k
 * @pre combination must contain valid indices [0, n-1]
 * @pre k ≤ n and both must be positive
 * @post combination array is modified to next lexicographic combination
 * @post Returns false when all combinations exhausted
 *
 * @note Modifies combination array in-place
 * @note Used internally for systematic vertex subset enumeration
 *
 * @example
 * int combo[] = {0, 1, 2}; // Choose 3 from 5
 * while (next_combination(combo, 3, 5)) {
 *     // Process current combination
 * }
 */
static bool next_combination(int *combination, int k, int n)
{
    int i = k - 1;
    while (i >= 0 && combination[i] == n - k + i)
    {
        i--;
    }

    if (i < 0)
        return false;

    combination[i]++;
    for (int j = i + 1; j < k; j++)
    {
        combination[j] = combination[j - 1] + 1;
    }

    return true;
}

/**
 * @brief Finds minimum vertex cut using brute force enumeration
 *
 * This function implements an exact algorithm for finding vertex connectivity
 * by systematically trying all possible vertex subsets in increasing size
 * order until a disconnecting set is found.
 *
 * Algorithm steps:
 * 1. Check if graph is already disconnected (return 0)
 * 2. For k = 1 to n-2, try all C(n,k) combinations
 * 3. Return size of first disconnecting combination found
 * 4. If no cut found, graph is complete (return n-1)
 *
 * @param graph Pointer to the graph structure
 * @param cut_vertices_out Output parameter for minimum cut vertices (can be NULL)
 *
 * @return Size of minimum vertex cut (vertex connectivity number)
 *
 * @complexity O(2^n * (V + E)) exponential due to subset enumeration
 *
 * @pre graph must be undirected with valid adjacency matrix
 * @pre graph->node_count must be properly set
 * @post cut_vertices_out contains minimum cut if not NULL (caller must free)
 * @post Returns exact vertex connectivity number
 * @post Original graph unchanged
 *
 * @warning Exponential time complexity - only suitable for small graphs (n ≤ 12)
 * @warning cut_vertices_out memory must be freed by caller if not NULL
 *
 * @note Returns 0 for trivial graphs or already disconnected graphs
 * @note Returns n-1 for complete graphs
 *
 * @see find_min_vertex_cut_maxflow() for approximation algorithm
 */
int find_min_vertex_cut_bruteforce(Graph *graph, int **cut_vertices_out)
{
    if (!graph || graph->node_count <= 2)
        return 0;

    int n = graph->node_count;

    /* Check if graph is already disconnected */
    if (!is_connected_after_removal(graph, NULL, 0))
    {
        if (cut_vertices_out)
            *cut_vertices_out = NULL;
        return 0;
    }

    /* Try removing k vertices for k = 1 to n-2 */
    for (int k = 1; k < n - 1; k++)
    {
        int *combination = malloc(k * sizeof(int));

        /* Initialize first combination */
        for (int i = 0; i < k; i++)
        {
            combination[i] = i;
        }

        do
        {
            if (!is_connected_after_removal(graph, combination, k))
            {
                /* Found minimum cut */
                if (cut_vertices_out)
                {
                    *cut_vertices_out = malloc(k * sizeof(int));
                    memcpy(*cut_vertices_out, combination, k * sizeof(int));
                }
                free(combination);
                return k;
            }
        } while (next_combination(combination, k, n));

        free(combination);
    }

    /* If no cut found, connectivity is n-1 (complete graph) */
    if (cut_vertices_out)
        *cut_vertices_out = NULL;
    return n - 1;
}

/**
 * @brief Approximates minimum vertex cut using min-degree heuristic
 *
 * This function provides a fast approximation for vertex connectivity using
 * the theoretical result that κ(G) ≤ δ(G) (connectivity ≤ minimum degree).
 * For many graph classes, this bound is tight.
 *
 * Algorithm steps:
 * 1. Check basic connectivity
 * 2. Find vertex with minimum degree
 * 3. Return minimum degree as approximation
 *
 * @param graph Pointer to the graph structure
 * @param cut_vertices_out Output parameter for cut vertex (can be NULL)
 *
 * @return Approximated vertex connectivity (minimum degree)
 *
 * @complexity O(V²) for degree computation in adjacency matrix
 *
 * @pre graph must be undirected with valid adjacency matrix
 * @post cut_vertices_out contains min-degree vertex if not NULL (caller must free)
 * @post Returns upper bound on vertex connectivity
 * @post Original graph unchanged
 *
 * @note This is an approximation - may overestimate actual connectivity
 * @note For many graph classes (e.g., regular graphs), gives exact result
 * @note Suitable for large graphs where exact computation is infeasible
 *
 * @warning Result may not be exact vertex connectivity
 * @warning cut_vertices_out memory must be freed by caller if not NULL
 *
 * @see find_min_vertex_cut_bruteforce() for exact algorithm
 */
int find_min_vertex_cut_maxflow(Graph *graph, int **cut_vertices_out)
{
    /* For larger graphs, we use a simplified approach based on minimum degree */
    /* This is an approximation - exact vertex connectivity for large graphs requires complex algorithms */

    if (!graph || graph->node_count <= 2)
        return 0;

    int n = graph->node_count;

    /* Check if graph is already disconnected */
    if (!is_connected_after_removal(graph, NULL, 0))
    {
        if (cut_vertices_out)
            *cut_vertices_out = NULL;
        return 0;
    }

    /* Find minimum degree as upper bound */
    int min_degree = n;
    int min_degree_vertex = 0;

    for (int i = 0; i < n; i++)
    {
        int degree = 0;
        for (int j = 0; j < n; j++)
        {
            if (graph->adjacency[i][j])
                degree++;
        }
        if (degree < min_degree)
        {
            min_degree = degree;
            min_degree_vertex = i;
        }
    }

    /* For approximation, return minimum degree */
    /* In many cases, vertex connectivity equals minimum degree */
    if (cut_vertices_out)
    {
        *cut_vertices_out = malloc(sizeof(int));
        (*cut_vertices_out)[0] = min_degree_vertex;
    }

    return min_degree;
}

/**
 * @brief Calculates vertex connectivity number with automatic algorithm selection
 *
 * This is the main function for computing vertex connectivity. It automatically
 * selects the appropriate algorithm based on graph size: exact algorithm for
 * small graphs (n ≤ 12) and approximation for larger graphs.
 *
 * The function handles various edge cases and provides informative output
 * about the algorithm choice and limitations.
 *
 * @param graph Pointer to the graph structure
 *
 * @return Vertex connectivity number κ(G)
 *
 * @complexity O(2^n * (V + E)) for small graphs, O(V²) for large graphs
 *
 * @pre graph must be valid with proper adjacency matrix
 * @post Returns exact result for small graphs (n ≤ 12)
 * @post Returns approximation for large graphs (n > 12) with warning
 * @post Original graph unchanged
 *
 * @note Automatically handles directed graphs by treating as undirected
 * @note Provides console output about algorithm selection
 * @note Returns 0 for trivial cases (n ≤ 1) and disconnected graphs
 * @note Returns 1 for graphs with exactly 2 vertices and an edge
 *
 * @warning For large graphs, result may be approximation, not exact
 *
 * @example
 * Graph *g = create_graph(6, false);
 * // ... add edges ...
 * int connectivity = calculate_connectivity_number(g);
 * printf("Vertex connectivity: %d\n", connectivity);
 */
int calculate_connectivity_number(Graph *graph)
{
    if (!graph)
        return 0;

    if (graph->is_directed)
    {
        printf("Note: Vertex connectivity for directed graphs is complex. Treating as undirected.\n");
    }

    int n = graph->node_count;

    /* Handle trivial cases */
    if (n <= 1)
        return 0;
    if (n == 2)
        return 1;

    /* For small graphs, use exact algorithm */
    if (n <= 12)
    {
        return find_min_vertex_cut_bruteforce(graph, NULL);
    }
    else
    {
        /* For larger graphs, use approximation */
        printf("Note: Using approximation for large graph (n=%d). Result may not be exact.\n", n);
        return find_min_vertex_cut_maxflow(graph, NULL);
    }
}

/**
 * @brief Provides comprehensive connectivity analysis and reporting
 *
 * This function performs detailed analysis of a graph's vertex connectivity
 * properties and prints comprehensive results including theoretical bounds,
 * exact/approximate connectivity values, minimum cuts, and structural insights.
 *
 * Analysis includes:
 * - Basic graph statistics (vertices, edges, degree distribution)
 * - Theoretical bounds (Whitney's inequalities)
 * - Exact or approximate connectivity computation
 * - Minimum vertex cut identification
 * - Structural properties and implications
 * - k-connectivity properties
 *
 * @param graph Pointer to the graph structure to analyze
 *
 * @complexity O(2^n * (V + E)) for small graphs, O(V²) for large graphs
 *
 * @pre graph must be valid with proper adjacency matrix
 * @post Comprehensive analysis printed to stdout
 * @post Original graph unchanged
 * @post All allocated memory properly freed
 *
 * @note Handles both directed and undirected graphs (treats directed as undirected)
 * @note Provides educational output suitable for learning graph theory
 * @note Includes theoretical background and practical insights
 * @note Memory management handled internally
 *
 * @warning Prints directly to stdout - not suitable for library use without output redirection
 *
 * @example
 * Graph *g = create_graph(5, false);
 * // ... construct graph ...
 * analyze_connectivity_number(g); // Prints detailed analysis
 *
 * @see calculate_connectivity_number() for just the numerical result
 */
void analyze_connectivity_number(Graph *graph)
{
    if (!graph)
    {
        printf("Error: Invalid graph for connectivity number analysis.\n");
        return;
    }

    printf("\n=== Connectivity Number Analysis ===\n");

    if (graph->is_directed)
    {
        printf("Note: Graph is directed. Analyzing vertex connectivity as undirected graph.\n");
    }

    int n = graph->node_count;
    printf("Graph has %d vertices.\n", n);

    /* Count edges */
    int edge_count = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = graph->is_directed ? 0 : i + 1; j < n; j++)
        {
            if (graph->adjacency[i][j])
                edge_count++;
        }
    }
    printf("Graph has %d edges.\n", edge_count);

    /* Handle trivial cases */
    if (n <= 1)
    {
        printf("Connectivity number: 0 (trivial graph)\n");
        printf("Analysis: Graph has ≤1 vertex, connectivity undefined or 0.\n");
        return;
    }

    if (n == 2)
    {
        if (edge_count > 0)
        {
            printf("Connectivity number: 1\n");
            printf("Analysis: Two connected vertices.\n");
        }
        else
        {
            printf("Connectivity number: 0\n");
            printf("Analysis: Two disconnected vertices.\n");
        }
        return;
    }

    /* Check if already disconnected */
    if (!is_connected_after_removal(graph, NULL, 0))
    {
        printf("Connectivity number: 0\n");
        printf("Analysis: Graph is already disconnected.\n");
        return;
    }

    /* Calculate minimum and maximum degree */
    int min_degree = n, max_degree = 0;
    int min_degree_vertex = 0;

    for (int i = 0; i < n; i++)
    {
        int degree = 0;
        for (int j = 0; j < n; j++)
        {
            if (graph->adjacency[i][j])
                degree++;
        }
        if (degree < min_degree)
        {
            min_degree = degree;
            min_degree_vertex = i;
        }
        if (degree > max_degree)
        {
            max_degree = degree;
        }
    }

    printf("Minimum degree: %d (vertex %d)\n", min_degree, min_degree_vertex);
    printf("Maximum degree: %d\n", max_degree);
    printf("Theoretical upper bound: %d (minimum degree)\n", min_degree);

    /* Calculate actual connectivity number */
    int *cut_vertices = NULL;
    int connectivity_num;

    if (n <= 12)
    {
        connectivity_num = find_min_vertex_cut_bruteforce(graph, &cut_vertices);
        printf("Connectivity number (exact): %d\n", connectivity_num);

        if (cut_vertices && connectivity_num > 0)
        {
            printf("Minimum vertex cut: ");
            for (int i = 0; i < connectivity_num; i++)
            {
                printf("v%d ", cut_vertices[i]);
            }
            printf("\n");
        }
    }
    else
    {
        connectivity_num = find_min_vertex_cut_maxflow(graph, &cut_vertices);
        printf("Connectivity number (approximation): %d\n", connectivity_num);
    }

    /* Analysis of result */
    if (connectivity_num == 0)
    {
        printf("Analysis: Graph is disconnected.\n");
    }
    else if (connectivity_num == 1)
    {
        printf("Analysis: Graph has articulation points (cut vertices).\n");
    }
    else if (connectivity_num == n - 1)
    {
        printf("Analysis: Complete graph K_%d.\n", n);
    }
    else if (connectivity_num == min_degree)
    {
        printf("Analysis: Connectivity equals minimum degree (common case).\n");
    }
    else
    {
        printf("Analysis: Graph is %d-connected.\n", connectivity_num);
    }

    /* Additional insights */
    if (connectivity_num >= 2)
    {
        printf("Property: Graph is 2-connected (no articulation points).\n");
    }
    if (connectivity_num >= 3)
    {
        printf("Property: Graph remains connected after removing any single vertex.\n");
    }

    /* Cleanup */
    if (cut_vertices)
        free(cut_vertices);
}
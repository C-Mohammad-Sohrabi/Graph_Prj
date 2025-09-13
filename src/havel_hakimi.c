#include "havel_hakimi.h"

/**
 * @file havel_hakimi.c
 * @brief Implementation of Havel-Hakimi algorithm for graph construction
 * @author Graph Theory Project Team
 * @date 2024
 *
 * This file implements the Havel-Hakimi algorithm for both directed and undirected graphs.
 * The algorithm determines if a degree sequence is graphical (can be realized by a simple graph)
 * and constructs such a graph if possible.
 *
 * Key algorithmic insights:
 * - Undirected: Erdős–Gallai theorem provides the theoretical foundation
 * - Directed: Must satisfy sum(out-degrees) = sum(in-degrees)
 * - Greedy approach: Always connect highest-degree vertices first
 *
 * The implementation also generates DOT files for immediate visualization.
 */

/**
 * @brief Comparison function for sorting nodes by degree in descending order
 *
 * Used by qsort() to arrange Node structures in descending order of degree.
 * When degrees are equal, sorts by original_index in ascending order to
 * ensure deterministic behavior and stable sorting.
 *
 * This comparison is crucial for the Havel-Hakimi algorithm's correctness,
 * as it ensures we always connect the highest-degree vertex to the next
 * highest-degree vertices.
 *
 * @param a Pointer to first Node structure
 * @param b Pointer to second Node structure
 * @return Negative if a->degree > b->degree, positive if a->degree < b->degree,
 *         or difference in original_index if degrees are equal
 */
int compare_nodes(const void *a, const void *b)
{
    Node *node_a = (Node *)a;
    Node *node_b = (Node *)b;

    // Primary sort: by degree in descending order
    if (node_a->degree != node_b->degree)
    {
        return node_b->degree - node_a->degree;
    }

    // Secondary sort: by original index in ascending order (for stability)
    return node_a->original_index - node_b->original_index;
}

/**
 * @brief Comparison function for sorting nodes by in-degree in descending order
 *
 * Similar to compare_nodes() but uses in_degree field instead of degree.
 * Used specifically in directed graph processing where we need to sort
 * by in-degrees to find appropriate targets for directed edges.
 *
 * @param a Pointer to first Node structure
 * @param b Pointer to second Node structure
 * @return Negative if a->in_degree > b->in_degree, positive if a->in_degree < b->in_degree,
 *         or difference in original_index if in-degrees are equal
 */
int compare_nodes_in_degree(const void *a, const void *b)
{
    Node *node_a = (Node *)a;
    Node *node_b = (Node *)b;

    // Primary sort: by in-degree in descending order
    if (node_a->in_degree != node_b->in_degree)
    {
        return node_b->in_degree - node_a->in_degree;
    }

    // Secondary sort: by original index in ascending order (for stability)
    return node_a->original_index - node_b->original_index;
}

/**
 * @brief Constructs undirected graph using classical Havel-Hakimi algorithm
 *
 * Implements the classical Havel-Hakimi algorithm for undirected graphs:
 *
 * Algorithm Steps:
 * 1. Sort vertices by degree in descending order
 * 2. Take vertex v with highest degree d
 * 3. Connect v to the next d vertices with highest degrees
 * 4. Reduce the degrees of connected vertices by 1
 * 5. Set v's degree to 0 and repeat until all degrees are 0
 *
 * The algorithm simultaneously:
 * - Validates that the degree sequence is graphical
 * - Constructs the actual graph structure
 * - Generates DOT file for visualization
 *
 * @param nodes Array of Node structures with degree information (modified during execution)
 * @param n Number of vertices in the graph
 * @param dot_file Open file pointer for DOT format output
 * @param graph Graph structure to populate with adjacency matrix
 * @return true if degree sequence is graphical and graph constructed successfully
 */
bool havel_hakimi_undirected(Node *nodes, int n, FILE *dot_file, Graph *graph)
{
    /* ========================================================================
     * INITIALIZATION: Set up graph structure and DOT file
     * ========================================================================*/

    // Write DOT file header for undirected graph
    fprintf(dot_file, "graph G {\n");

    // Initialize graph structure
    graph->node_count = n;
    graph->is_directed = false;
    graph->allow_bidirectional = false;

    // Allocate and initialize adjacency matrix (all zeros initially)
    graph->adjacency = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        graph->adjacency[i] = calloc(n, sizeof(int));
    }

    /* ========================================================================
     * MAIN ALGORITHM LOOP: Iterative degree reduction
     * ========================================================================*/

    while (true)
    {
        // Sort vertices by current degree (descending order)
        qsort(nodes, n, sizeof(Node), compare_nodes);

        // Find first vertex with non-zero degree
        int idx = 0;
        while (idx < n && nodes[idx].degree == 0)
        {
            idx++;
        }

        // If all degrees are zero, construction is complete
        if (idx == n)
        {
            break;
        }

        // Get vertex with highest degree
        Node u = nodes[idx];
        int d = u.degree;

        // Validity check: degree cannot exceed remaining vertices
        if (d > n - 1 - idx)
        {
            fprintf(dot_file, "}\n"); // Close DOT file properly
            return false;             // Invalid degree sequence
        }

        /* ====================================================================
         * EDGE CREATION: Connect current vertex to next d highest-degree vertices
         * ====================================================================*/

        int edges_added = 0;
        for (int i = idx + 1; i < n && edges_added < d; i++)
        {
            Node v = nodes[i];

            // Check if edge can be added (no self-loops, no duplicate edges, positive degree)
            if (v.degree > 0 &&
                u.original_index != v.original_index &&
                graph->adjacency[u.original_index][v.original_index] == 0)
            {

                // Add edge to adjacency matrix (undirected: both directions)
                graph->adjacency[u.original_index][v.original_index] = 1;
                graph->adjacency[v.original_index][u.original_index] = 1;

                // Write edge to DOT file
                fprintf(dot_file, "  %d -- %d;\n", u.original_index, v.original_index);

                // Reduce degree of target vertex
                nodes[i].degree--;
                edges_added++;
            }
        }

        // Validity check: ensure we could add all required edges
        if (edges_added < d)
        {
            fprintf(dot_file, "}\n"); // Close DOT file properly
            return false;             // Could not realize the degree sequence
        }

        // Set current vertex degree to zero (processed)
        nodes[idx].degree = 0;
    }

    // Close DOT file
    fprintf(dot_file, "}\n");
    return true; // Successful construction
}

/**
 * @brief Constructs directed graph using modified Havel-Hakimi algorithm
 *
 * Implements a modified Havel-Hakimi algorithm for directed graphs that handles
 * both out-degree and in-degree sequences simultaneously.
 *
 * Algorithm Steps:
 * 1. Maintain separate out-degree and in-degree for each vertex
 * 2. Find vertex with highest out-degree
 * 3. Find vertex with highest in-degree (among valid targets)
 * 4. Create directed edge between them
 * 5. Reduce respective degrees and repeat
 *
 * The algorithm ensures:
 * - No self-loops (unless specifically allowed)
 * - No duplicate edges (unless bidirectional edges are allowed)
 * - Proper degree sequence realization
 *
 * @param nodes Array of Node structures with out-degree and in-degree information
 * @param n Number of vertices in the graph
 * @param dot_file Open file pointer for DOT format output (digraph)
 * @param graph Graph structure to populate with adjacency matrix
 * @param allow_bidirectional If true, allows bidirectional edges in directed graph
 * @return true if both degree sequences are graphical and graph constructed successfully
 */
bool havel_hakimi_directed(Node *nodes, int n, FILE *dot_file, Graph *graph, bool allow_bidirectional)
{
    /* ========================================================================
     * INITIALIZATION: Set up directed graph structure
     * ========================================================================*/

    // Write DOT file header for directed graph
    fprintf(dot_file, "digraph G {\n");

    // Initialize graph structure for directed graph
    graph->node_count = n;
    graph->is_directed = true;
    graph->allow_bidirectional = allow_bidirectional;

    // Allocate and initialize adjacency matrix
    graph->adjacency = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        graph->adjacency[i] = calloc(n, sizeof(int));
    }

    /* ========================================================================
     * MAIN ALGORITHM LOOP: Directed edge creation
     * ========================================================================*/

    while (true)
    {
        /* ====================================================================
         * ACTIVE VERTEX IDENTIFICATION: Find vertices with remaining degrees
         * ====================================================================*/

        // Collect vertices with positive out-degrees (edge sources)
        Node *active_out = malloc(n * sizeof(Node));
        Node *active_in = malloc(n * sizeof(Node));
        int out_count = 0, in_count = 0;

        for (int i = 0; i < n; i++)
        {
            if (nodes[i].degree > 0)
            { // Out-degree > 0
                active_out[out_count++] = nodes[i];
            }
            if (nodes[i].in_degree > 0)
            { // In-degree > 0
                active_in[in_count++] = nodes[i];
            }
        }

        // If no active vertices remain, construction is complete
        if (out_count == 0 || in_count == 0)
        {
            free(active_out);
            free(active_in);
            break;
        }

        /* ====================================================================
         * VERTEX SELECTION: Choose source and target for next edge
         * ====================================================================*/

        // Sort by out-degree (descending) to get highest out-degree vertex
        qsort(active_out, out_count, sizeof(Node), compare_nodes);
        Node u = active_out[0]; // Vertex with highest out-degree

        // Sort by in-degree (descending) to prioritize high in-degree targets
        qsort(active_in, in_count, sizeof(Node), compare_nodes_in_degree);

        /* ====================================================================
         * EDGE CREATION: Add directed edge with constraint checking
         * ====================================================================*/

        bool edge_added = false;
        for (int i = 0; i < in_count; i++)
        {
            Node v = active_in[i];

            // Check edge validity constraints
            bool can_add_edge = (u.original_index != v.original_index) &&                                           // No self-loops
                                (graph->adjacency[u.original_index][v.original_index] == 0) &&                      // No duplicate edges
                                (allow_bidirectional || graph->adjacency[v.original_index][u.original_index] == 0); // Bidirectional policy

            if (can_add_edge)
            {
                // Add directed edge u → v
                graph->adjacency[u.original_index][v.original_index] = 1;

                // Write edge to DOT file
                fprintf(dot_file, "  %d -> %d;\n", u.original_index, v.original_index);

                // Reduce in-degree of target vertex v
                for (int j = 0; j < n; j++)
                {
                    if (nodes[j].original_index == v.original_index)
                    {
                        nodes[j].in_degree--;
                        break;
                    }
                }

                edge_added = true;
                break;
            }
        }

        // If no edge could be added, degree sequence is not graphical
        if (!edge_added)
        {
            free(active_out);
            free(active_in);
            fprintf(dot_file, "}\n");
            return false;
        }

        // Reduce out-degree of source vertex u
        for (int j = 0; j < n; j++)
        {
            if (nodes[j].original_index == u.original_index)
            {
                nodes[j].degree--;
                break;
            }
        }

        free(active_out);
        free(active_in);
    }

    /* ========================================================================
     * FINAL VALIDATION: Ensure all degrees are zero
     * ========================================================================*/

    // Verify that all degrees have been reduced to zero
    for (int i = 0; i < n; i++)
    {
        if (nodes[i].degree != 0 || nodes[i].in_degree != 0)
        {
            fprintf(dot_file, "}\n");
            return false; // Degree sequence could not be fully realized
        }
    }

    // Close DOT file
    fprintf(dot_file, "}\n");
    return true; // Successful construction
}
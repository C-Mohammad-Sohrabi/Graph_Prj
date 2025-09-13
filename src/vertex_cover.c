/**
 * @file vertex_cover.c
 * @brief Vertex cover algorithms for undirected graphs
 * @author Graph Theory Project Team
 * @date 2024
 *
 * This module implements various algorithms for finding vertex covers in undirected graphs.
 * A vertex cover is a set of vertices such that every edge in the graph is incident to
 * at least one vertex in the set.
 *
 * The module provides several approaches:
 * 1. Exact algorithm via Maximum Independent Set (complement graph approach)
 * 2. 2-approximation algorithm using maximal matching
 * 3. Optimal algorithm for bipartite graphs using König's theorem
 *
 * Key concepts:
 * - Vertex Cover: Set of vertices covering all edges
 * - Minimum Vertex Cover: Smallest possible vertex cover
 * - Maximum Independent Set: Largest set of non-adjacent vertices
 * - König's Theorem: In bipartite graphs, |min vertex cover| = |max matching|
 *
 * Time Complexity: Varies by algorithm (exponential for exact, polynomial for approximation)
 * Space Complexity: O(n²) for adjacency matrix and auxiliary data structures
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "vertex_cover.h"
#include "independent_set.h" /* for create_complement_graph / find_maximum_independent_set */
#include "clique.h"
#include "set_utils.h"

static int INF_DIST = INT_MAX;

/**
 * @brief Checks if a set contains a specific vertex
 *
 * This is a helper function that performs linear search through a set
 * to determine if a given vertex is present.
 *
 * @param s Pointer to the set to search in
 * @param v Vertex value to search for
 *
 * @return true if vertex v is found in set s, false otherwise
 *
 * @complexity O(n) where n is the size of the set
 *
 * @pre s must be a valid Set pointer with initialized vertices array
 * @post No modifications to the set
 */
static bool set_contains(Set *s, int v)
{
    for (int i = 0; i < s->size; i++)
        if (s->vertices[i] == v)
            return true;
    return false;
}

/**
 * @brief Finds exact minimum vertex cover using Maximum Independent Set approach
 *
 * This function implements an exact algorithm for finding the minimum vertex cover
 * by utilizing the relationship: Vertex Cover = V \ Maximum Independent Set.
 *
 * Algorithm steps:
 * 1. Create complement graph of the input graph
 * 2. Find maximum clique in complement (equivalent to MIS in original)
 * 3. Return all vertices not in the maximum independent set
 *
 * @param graph Pointer to the undirected graph structure
 *
 * @return Pointer to Set containing minimum vertex cover, NULL on failure
 *
 * @complexity Exponential O(2^n) due to maximum clique computation
 *
 * @pre graph must be an undirected graph (is_directed == false)
 * @pre graph must have valid adjacency matrix
 * @post Returns newly allocated Set that caller must free
 * @post Original graph remains unchanged
 *
 * @warning This algorithm has exponential time complexity
 * @warning Not suitable for large graphs (>20-30 vertices)
 * @warning Returns NULL for directed graphs
 */
Set *vertex_cover_exact_via_mis(Graph *graph)
{
    if (!graph)
        return NULL;
    if (graph->is_directed)
        return NULL; /* not supported */

    /* Create complement and find maximum clique on complement (== MIS in original) */
    Graph *comp = create_complement_graph(graph);
    if (!comp)
        return NULL;

    Set *max_clique = find_maximum_clique(comp);
    /* free complement adjacency now */
    for (int i = 0; i < comp->node_count; i++)
        free(comp->adjacency[i]);
    free(comp->adjacency);
    free(comp);

    if (!max_clique)
        return NULL;

    /* Vertex cover = V \ MIS */
    Set *vc = set_create(graph->node_count);
    for (int v = 0; v < graph->node_count; v++)
    {
        if (!set_contains(max_clique, v))
            set_add(vc, v);
    }
    set_destroy(max_clique);
    return vc;
}

/**
 * @brief Finds 2-approximation vertex cover using maximal matching
 *
 * This function implements a greedy 2-approximation algorithm that guarantees
 * a vertex cover of size at most 2 times the optimal solution.
 *
 * Algorithm steps:
 * 1. Find a maximal matching (greedily select edges)
 * 2. Include both endpoints of each matched edge in vertex cover
 * 3. This covers all edges since every edge is either matched or
 *    incident to a matched edge
 *
 * @param graph Pointer to the undirected graph structure
 *
 * @return Pointer to Set containing 2-approximate vertex cover, NULL on failure
 *
 * @complexity O(V²) for adjacency matrix representation
 *
 * @pre graph must be an undirected graph (is_directed == false)
 * @pre graph must have valid adjacency matrix
 * @post Returns newly allocated Set that caller must free
 * @post Size of returned cover ≤ 2 × |optimal vertex cover|
 * @post Original graph remains unchanged
 *
 * @note This is a practical algorithm for large graphs
 * @note Provides good performance vs. quality trade-off
 */
Set *vertex_cover_approx(Graph *graph)
{
    if (!graph)
        return NULL;
    if (graph->is_directed)
        return NULL;

    int n = graph->node_count;
    char *matched = calloc(n, sizeof(char)); /* marks if vertex already matched/covered */
    Set *vc = set_create(n);

    for (int u = 0; u < n; u++)
    {
        if (matched[u])
            continue;
        for (int v = 0; v < n; v++)
        {
            if (graph->adjacency[u][v] && !matched[v])
            {
                /* take edge (u,v) into matching => add both endpoints to cover */
                if (!matched[u])
                {
                    set_add(vc, u);
                    matched[u] = 1;
                }
                if (!matched[v])
                {
                    set_add(vc, v);
                    matched[v] = 1;
                }
                break;
            }
        }
    }

    free(matched);
    return vc;
}

/**
 * @brief Checks if graph is bipartite and computes partition masks
 *
 * This function determines if the input graph is bipartite using BFS coloring
 * and returns masks indicating which vertices belong to each partition.
 *
 * Algorithm steps:
 * 1. Use BFS to 2-color the graph components
 * 2. If coloring succeeds, graph is bipartite
 * 3. Create boolean masks for left and right partitions
 *
 * @param graph Pointer to the undirected graph structure
 * @param left_mask_out Output parameter for left partition mask (caller must free)
 * @param right_mask_out Output parameter for right partition mask (caller must free)
 *
 * @return true if graph is bipartite, false otherwise
 *
 * @complexity O(V + E) using BFS traversal
 *
 * @pre graph must be an undirected graph
 * @pre left_mask_out and right_mask_out must be valid char** pointers
 * @post If true returned, masks are allocated and must be freed by caller
 * @post If false returned, no memory is allocated for masks
 * @post Isolated vertices are assigned to left partition by default
 *
 * @warning Caller is responsible for freeing the allocated mask arrays
 * @note Returns false for directed graphs
 */
bool is_bipartite_partition(Graph *graph, char **left_mask_out, char **right_mask_out)
{
    if (!graph || graph->is_directed)
        return false;
    int n = graph->node_count;
    char *color = malloc(n); /* 0 = uncolored, 1 = left, 2 = right */
    memset(color, 0, n);

    /* BFS over components */
    int *queue = malloc(n * sizeof(int));
    for (int s = 0; s < n; s++)
    {
        if (color[s] != 0)
            continue;
        /* if isolated node, color as left by default */
        color[s] = 1;
        int head = 0, tail = 0;
        queue[tail++] = s;
        while (head < tail)
        {
            int u = queue[head++];
            for (int v = 0; v < n; v++)
            {
                if (!graph->adjacency[u][v])
                    continue;
                if (color[v] == 0)
                {
                    color[v] = (color[u] == 1) ? 2 : 1;
                    queue[tail++] = v;
                }
                else if (color[v] == color[u])
                {
                    free(color);
                    free(queue);
                    return false; /* not bipartite */
                }
            }
        }
    }

    free(queue);
    /* produce masks */
    char *left = calloc(n, 1);
    char *right = calloc(n, 1);
    for (int i = 0; i < n; i++)
    {
        if (color[i] == 1)
            left[i] = 1;
        else if (color[i] == 2)
            right[i] = 1;
        else
            left[i] = 1; /* fallback */
    }
    free(color);
    *left_mask_out = left;
    *right_mask_out = right;
    return true;
}

/**
 * @brief Builds left and right vertex lists from partition masks
 *
 * This helper function converts boolean partition masks into arrays of
 * vertex indices for the left and right sides of a bipartite graph.
 *
 * @param graph Pointer to the graph structure
 * @param left_mask Boolean mask indicating left partition vertices
 * @param left_nodes_out Output parameter for left vertex indices array (caller must free)
 * @param left_n_out Output parameter for number of left vertices
 * @param right_nodes_out Output parameter for right vertex indices array (caller must free)
 * @param right_n_out Output parameter for number of right vertices
 *
 * @complexity O(V) single pass through all vertices
 *
 * @pre graph must be valid with initialized node_count
 * @pre left_mask must be valid boolean array of size node_count
 * @pre All output parameters must be valid pointers
 * @post Allocated arrays must be freed by caller
 * @post left_n_out + right_n_out = graph->node_count
 */
static void build_left_right_lists_from_masks(Graph *graph, char *left_mask,
                                              int **left_nodes_out, int *left_n_out,
                                              int **right_nodes_out, int *right_n_out)
{
    int n = graph->node_count;
    int left_count = 0, right_count = 0;
    for (int i = 0; i < n; i++)
    {
        if (left_mask[i])
            left_count++;
        else
            right_count++;
    }
    int *L = malloc(left_count * sizeof(int));
    int *R = malloc(right_count * sizeof(int));
    int li = 0, ri = 0;
    for (int i = 0; i < n; i++)
    {
        if (left_mask[i])
            L[li++] = i;
        else
            R[ri++] = i;
    }
    *left_nodes_out = L;
    *left_n_out = left_count;
    *right_nodes_out = R;
    *right_n_out = right_count;
}

/**
 * @brief Implements Hopcroft-Karp maximum bipartite matching algorithm
 *
 * This function finds a maximum matching in a bipartite graph using the
 * Hopcroft-Karp algorithm, which uses BFS to find shortest augmenting paths
 * and DFS to find multiple augmenting paths simultaneously.
 *
 * Algorithm steps:
 * 1. Build adjacency lists for efficient neighbor iteration
 * 2. Repeatedly find shortest augmenting paths using BFS
 * 3. Use DFS to find and augment multiple paths simultaneously
 * 4. Continue until no more augmenting paths exist
 *
 * @param graph Pointer to the graph structure
 * @param left_nodes Array of vertex indices in left partition
 * @param left_n Number of vertices in left partition
 * @param right_nodes Array of vertex indices in right partition
 * @param right_n Number of vertices in right partition
 * @param pairU_out Output matching array for left vertices (caller must free)
 * @param pairV_out Output matching array for right vertices (caller must free)
 *
 * @return Size of maximum matching found
 *
 * @complexity O(E√V) which is optimal for bipartite matching
 *
 * @pre graph must be bipartite with valid adjacency matrix
 * @pre left_nodes and right_nodes must contain valid vertex indices
 * @pre All output parameters must be valid pointers
 * @post pairU[i] = j means left_nodes[i] is matched to right_nodes[j]
 * @post pairV[j] = i means right_nodes[j] is matched to left_nodes[i]
 * @post Unmatched vertices have value -1 in pairing arrays
 * @post Caller must free pairU_out and pairV_out arrays
 *
 * @warning Returns NULL arrays if either partition is empty
 */
static int hopcroft_karp(Graph *graph, int *left_nodes, int left_n, int *right_nodes, int right_n,
                         int **pairU_out, int **pairV_out)
{
    if (left_n == 0 || right_n == 0)
    {
        *pairU_out = NULL;
        *pairV_out = NULL;
        return 0;
    }

    /* build adjacency list: for each left index i in [0..left_n-1], neighbors are indices j in [0..right_n-1]
       such that graph->adjacency[left_nodes[i]][ right_nodes[j] ] == 1 */
    int **adjL = malloc(left_n * sizeof(int *));
    int *adj_sizes = calloc(left_n, sizeof(int));
    for (int i = 0; i < left_n; i++)
    {
        /* count neighbors */
        int u = left_nodes[i];
        int cnt = 0;
        for (int j = 0; j < right_n; j++)
        {
            int v = right_nodes[j];
            if (graph->adjacency[u][v])
                cnt++;
        }
        adjL[i] = malloc(cnt * sizeof(int));
        int k = 0;
        for (int j = 0; j < right_n; j++)
        {
            int v = right_nodes[j];
            if (graph->adjacency[u][v])
                adjL[i][k++] = j; /* store neighbor as index in right_nodes */
        }
        adj_sizes[i] = cnt;
    }

    int *pairU = malloc(left_n * sizeof(int));
    int *pairV = malloc(right_n * sizeof(int));
    int *dist = malloc(left_n * sizeof(int));

    for (int i = 0; i < left_n; i++)
        pairU[i] = -1;
    for (int j = 0; j < right_n; j++)
        pairV[j] = -1;

    int matching = 0;

    /* BFS in HK to build layers */
    int bfs_queue_capacity = left_n;
    int *q = malloc(bfs_queue_capacity * sizeof(int));

    while (1)
    {
        /* BFS */
        int qh = 0, qt = 0;
        for (int i = 0; i < left_n; i++)
        {
            if (pairU[i] == -1)
            {
                dist[i] = 0;
                q[qt++] = i;
            }
            else
                dist[i] = INF_DIST;
        }
        int found_augmenting = 0;
        while (qh < qt)
        {
            int u = q[qh++];
            for (int k = 0; k < adj_sizes[u]; k++)
            {
                int v = adjL[u][k];
                int pu = pairV[v];
                if (pu == -1)
                {
                    found_augmenting = 1; /* shortest augmenting path exists via this free vertex */
                }
                else
                {
                    if (dist[pu] == INF_DIST)
                    {
                        dist[pu] = dist[u] + 1;
                        q[qt++] = pu;
                    }
                }
            }
        }
        if (!found_augmenting)
            break;

        /* DFS (recursive-style using stack) to find augmenting paths from free U vertices */
        /* We'll implement a simple recursive DFS using helper function closure-like via static local functions is messy;
           so implement iterative augmentation using stack arrays and recursion simulation. For clarity and small graphs,
           we can implement a recursive function using function pointer; but to keep it simple, implement recursive function. */

        /* define recursive lambda via inner function using static helper - but C doesn't have nested functions portably.
           Instead implement iterative DFS with stack storing (u, next_neighbor_index). */

        int *next = calloc(left_n, sizeof(int));
        /* stack of left nodes */
        int *stack = malloc((left_n + 5) * sizeof(int));
        int stack_sz;

        for (int start = 0; start < left_n; start++)
        {
            if (pairU[start] != -1)
                continue;
            /* iterative DFS from `start` */
            stack_sz = 0;
            stack[stack_sz++] = start;
            while (stack_sz > 0)
            {
                int u = stack[stack_sz - 1];
                if (next[u] < adj_sizes[u])
                {
                    int v = adjL[u][next[u]++];
                    int pu = pairV[v];
                    if (pu == -1 || (dist[pu] == dist[u] + 1 && pairU[pu] != -1))
                    {
                        /* try to augment */
                        /* recursively attempt to find augmenting path from pu if exists */
                        if (pu != -1)
                        {
                            stack[stack_sz++] = pu;
                        }
                        else
                        {
                            /* found path: flip along stack */
                            /* stack contains sequence of left indices representing alternating path */
                            /* Walk back and flip pairs along alternating edges */
                            int cur_v = v;
                            for (int idx = stack_sz - 1; idx >= 0; idx--)
                            {
                                int cur_u = stack[idx];
                                /* find neighbor in adjL[cur_u] equal to cur_v */
                                /* set pairU[cur_u] = cur_v and pairV[cur_v] = cur_u */
                                pairU[cur_u] = cur_v;
                                pairV[cur_v] = cur_u;
                                /* if idx > 0, cur_v becomes previous matched partner in previous step: find previous v' for next flip */
                                if (idx > 0)
                                {
                                    /* previous left node is stack[idx-1]; we need the v matched to it before (which is pairU[ stack[idx-1] ]) */
                                    cur_v = pairU[stack[idx - 1]];
                                }
                            }
                            matching++;
                            /* clear stack */
                            stack_sz = 0;
                        }
                    }
                }
                else
                {
                    /* backtrack */
                    stack_sz--;
                }
            }
        }

        free(stack);
        free(next);

        /* continue to next BFS round until no more augmenting paths */
    }

    /* cleanup adjacency lists */
    for (int i = 0; i < left_n; i++)
        free(adjL[i]);
    free(adjL);
    free(adj_sizes);
    free(dist);
    free(q);

    *pairU_out = pairU;
    *pairV_out = pairV;
    return matching;
}

/**
 * @brief Derives minimum vertex cover from maximum bipartite matching
 *
 * This function implements the constructive proof of König's theorem by
 * performing alternating BFS from unmatched left vertices to find the
 * minimum vertex cover.
 *
 * Algorithm steps:
 * 1. Start BFS from all unmatched left vertices
 * 2. Follow alternating paths: non-matching edges L→R, matching edges R→L
 * 3. Let Z be the set of visited vertices
 * 4. Minimum vertex cover = (Left \ Z) ∪ (Right ∩ Z)
 *
 * @param graph Pointer to the graph structure
 * @param left_nodes Array of left partition vertex indices
 * @param left_n Number of left vertices
 * @param right_nodes Array of right partition vertex indices
 * @param right_n Number of right vertices
 * @param pairU Matching array for left vertices
 * @param pairV Matching array for right vertices
 *
 * @return Pointer to Set containing minimum vertex cover
 *
 * @complexity O(V + E) for BFS traversal
 *
 * @pre All input parameters must be valid and consistent
 * @pre pairU and pairV must represent a valid matching
 * @post Returns newly allocated Set that caller must free
 * @post |returned cover| = |maximum matching| (König's theorem)
 * @post Returned cover is guaranteed to be minimum
 */
static Set *vertex_cover_from_matching(Graph *graph, int *left_nodes, int left_n, int *right_nodes, int right_n,
                                       int *pairU, int *pairV)
{
    int n = graph->node_count;
    char *visitedL = calloc(left_n, 1);
    char *visitedR = calloc(right_n, 1);

    /* build adjacency for quick neighbor iteration */
    int **adjL = malloc(left_n * sizeof(int *));
    int *adjL_sz = calloc(left_n, sizeof(int));
    for (int i = 0; i < left_n; i++)
    {
        int u = left_nodes[i];
        int cnt = 0;
        for (int j = 0; j < right_n; j++)
        {
            int v = right_nodes[j];
            if (graph->adjacency[u][v])
                cnt++;
        }
        adjL[i] = malloc((cnt > 0 ? cnt : 1) * sizeof(int));
        int k = 0;
        for (int j = 0; j < right_n; j++)
        {
            int v = right_nodes[j];
            if (graph->adjacency[u][v])
                adjL[i][k++] = j;
        }
        adjL_sz[i] = k;
    }

    /* BFS queue of (isLeft, index) pairs — encode as signed int: positive = left idx, negative-1 = right idx */
    int qcap = left_n + right_n + 5;
    int *queue = malloc(qcap * sizeof(int));
    int qh = 0, qt = 0;

    /* enqueue all unmatched left vertices */
    for (int i = 0; i < left_n; i++)
    {
        if (pairU[i] == -1)
        {
            visitedL[i] = 1;
            queue[qt++] = i; /* encode left as non-negative */
        }
    }

    while (qh < qt)
    {
        int code = queue[qh++];
        if (code >= 0)
        {
            /* left index */
            int li = code;
            for (int k = 0; k < adjL_sz[li]; k++)
            {
                int rj = adjL[li][k];
                if (!visitedR[rj])
                {
                    if (pairU[li] != rj)
                    { /* traverse only via non-matching edges left->right */
                        visitedR[rj] = 1;
                        queue[qt++] = -1 - rj; /* encode right as negative */
                    }
                }
            }
        }
        else
        {
            /* right index */
            int rj = -1 - code;
            int paired_left = pairV[rj];
            if (paired_left != -1 && !visitedL[paired_left])
            {
                visitedL[paired_left] = 1;
                queue[qt++] = paired_left;
            }
        }
    }

    /* Build vertex cover = (Left \ Z) union (Right ∩ Z) */
    Set *vc = set_create(n);
    for (int i = 0; i < left_n; i++)
    {
        int orig = left_nodes[i];
        if (!visitedL[i])
            set_add(vc, orig);
    }
    for (int j = 0; j < right_n; j++)
    {
        int orig = right_nodes[j];
        if (visitedR[j])
            set_add(vc, orig);
    }

    /* cleanup */
    for (int i = 0; i < left_n; i++)
        free(adjL[i]);
    free(adjL);
    free(adjL_sz);
    free(queue);
    free(visitedL);
    free(visitedR);

    return vc;
}

/**
 * @brief Finds optimal vertex cover for bipartite graphs using König's theorem
 *
 * This is the main function for finding minimum vertex cover in bipartite graphs.
 * It combines bipartite detection, maximum matching computation, and vertex cover
 * derivation to provide an optimal solution in polynomial time.
 *
 * Algorithm overview:
 * 1. Check if graph is bipartite and get partitions
 * 2. Find maximum matching using Hopcroft-Karp
 * 3. Derive minimum vertex cover using König's theorem
 *
 * König's Theorem: In bipartite graphs, the size of minimum vertex cover
 * equals the size of maximum matching.
 *
 * @param graph Pointer to the undirected graph structure
 *
 * @return Pointer to Set containing minimum vertex cover, NULL if not bipartite
 *
 * @complexity O(E√V) dominated by Hopcroft-Karp algorithm
 *
 * @pre graph must be an undirected graph (is_directed == false)
 * @pre graph must have valid adjacency matrix
 * @post Returns NULL if graph is not bipartite
 * @post Returns optimal vertex cover for bipartite graphs
 * @post Caller must free returned Set
 * @post Original graph remains unchanged
 *
 * @note This is the most efficient algorithm for bipartite graphs
 * @note Always returns optimal solution (not approximation)
 * @warning Returns NULL for non-bipartite or directed graphs
 */
Set *vertex_cover_bipartite_konig(Graph *graph)
{
    if (!graph)
        return NULL;
    if (graph->is_directed)
        return NULL;

    char *left_mask = NULL;
    char *right_mask = NULL;
    if (!is_bipartite_partition(graph, &left_mask, &right_mask))
    {
        return NULL; /* not bipartite */
    }

    int *left_nodes = NULL, *right_nodes = NULL;
    int left_n = 0, right_n = 0;
    build_left_right_lists_from_masks(graph, left_mask, &left_nodes, &left_n, &right_nodes, &right_n);

    free(left_mask);
    free(right_mask);

    int *pairU = NULL, *pairV = NULL;
    int match_size = hopcroft_karp(graph, left_nodes, left_n, right_nodes, right_n, &pairU, &pairV);

    /* If no matching arrays (empty sides), handle trivial cases */
    if (left_n == 0 || right_n == 0)
    {
        /* trivial cover: all non-isolated vertices (or none) */
        Set *vc = set_create(graph->node_count);
        for (int i = 0; i < graph->node_count; i++)
        {
            int deg = 0;
            for (int j = 0; j < graph->node_count; j++)
                if (graph->adjacency[i][j])
                    deg++;
            if (deg > 0)
                set_add(vc, i);
        }
        if (pairU)
            free(pairU);
        if (pairV)
            free(pairV);
        free(left_nodes);
        free(right_nodes);
        return vc;
    }

    /* derive vertex cover using visited sets from alternating BFS */
    Set *vc = vertex_cover_from_matching(graph, left_nodes, left_n, right_nodes, right_n, pairU, pairV);

    free(pairU);
    free(pairV);
    free(left_nodes);
    free(right_nodes);
    return vc;
}

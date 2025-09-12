#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "connectivity_number.h"
#include "structs.h"

/* Helper function to perform DFS and count reachable vertices */
static void dfs_reachable(Graph *graph, int start, bool *visited, int *removed_vertices, int removed_count, int *reachable_count) {
    /* Check if start vertex is removed */
    for (int i = 0; i < removed_count; i++) {
        if (removed_vertices[i] == start) return;
    }
    
    visited[start] = true;
    (*reachable_count)++;
    
    for (int v = 0; v < graph->node_count; v++) {
        if (graph->adjacency[start][v] && !visited[v]) {
            /* Check if v is removed */
            bool is_removed = false;
            for (int i = 0; i < removed_count; i++) {
                if (removed_vertices[i] == v) {
                    is_removed = true;
                    break;
                }
            }
            if (!is_removed) {
                dfs_reachable(graph, v, visited, removed_vertices, removed_count, reachable_count);
            }
        }
    }
}

/* Check if graph is connected after removing specified vertices */
bool is_connected_after_removal(Graph *graph, int *removed_vertices, int removed_count) {
    if (!graph || graph->node_count <= 1) return true;
    
    int n = graph->node_count;
    bool *visited = calloc(n, sizeof(bool));
    
    /* Find first non-removed vertex as start point */
    int start = -1;
    for (int i = 0; i < n; i++) {
        bool is_removed = false;
        for (int j = 0; j < removed_count; j++) {
            if (removed_vertices[j] == i) {
                is_removed = true;
                break;
            }
        }
        if (!is_removed) {
            start = i;
            break;
        }
    }
    
    if (start == -1) {
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

/* Generate next combination of k elements from n elements */
static bool next_combination(int *combination, int k, int n) {
    int i = k - 1;
    while (i >= 0 && combination[i] == n - k + i) {
        i--;
    }
    
    if (i < 0) return false;
    
    combination[i]++;
    for (int j = i + 1; j < k; j++) {
        combination[j] = combination[j - 1] + 1;
    }
    
    return true;
}

/* Brute force approach for small graphs */
int find_min_vertex_cut_bruteforce(Graph *graph, int **cut_vertices_out) {
    if (!graph || graph->node_count <= 2) return 0;
    
    int n = graph->node_count;
    
    /* Check if graph is already disconnected */
    if (!is_connected_after_removal(graph, NULL, 0)) {
        if (cut_vertices_out) *cut_vertices_out = NULL;
        return 0;
    }
    
    /* Try removing k vertices for k = 1 to n-2 */
    for (int k = 1; k < n - 1; k++) {
        int *combination = malloc(k * sizeof(int));
        
        /* Initialize first combination */
        for (int i = 0; i < k; i++) {
            combination[i] = i;
        }
        
        do {
            if (!is_connected_after_removal(graph, combination, k)) {
                /* Found minimum cut */
                if (cut_vertices_out) {
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
    if (cut_vertices_out) *cut_vertices_out = NULL;
    return n - 1;
}

/* Simplified max-flow approach for larger graphs */
int find_min_vertex_cut_maxflow(Graph *graph, int **cut_vertices_out) {
    /* For larger graphs, we use a simplified approach based on minimum degree */
    /* This is an approximation - exact vertex connectivity for large graphs requires complex algorithms */
    
    if (!graph || graph->node_count <= 2) return 0;
    
    int n = graph->node_count;
    
    /* Check if graph is already disconnected */
    if (!is_connected_after_removal(graph, NULL, 0)) {
        if (cut_vertices_out) *cut_vertices_out = NULL;
        return 0;
    }
    
    /* Find minimum degree as upper bound */
    int min_degree = n;
    int min_degree_vertex = 0;
    
    for (int i = 0; i < n; i++) {
        int degree = 0;
        for (int j = 0; j < n; j++) {
            if (graph->adjacency[i][j]) degree++;
        }
        if (degree < min_degree) {
            min_degree = degree;
            min_degree_vertex = i;
        }
    }
    
    /* For approximation, return minimum degree */
    /* In many cases, vertex connectivity equals minimum degree */
    if (cut_vertices_out) {
        *cut_vertices_out = malloc(sizeof(int));
        (*cut_vertices_out)[0] = min_degree_vertex;
    }
    
    return min_degree;
}

/* Main function to calculate connectivity number */
int calculate_connectivity_number(Graph *graph) {
    if (!graph) return 0;
    
    if (graph->is_directed) {
        printf("Note: Vertex connectivity for directed graphs is complex. Treating as undirected.\n");
    }
    
    int n = graph->node_count;
    
    /* Handle trivial cases */
    if (n <= 1) return 0;
    if (n == 2) return 1;
    
    /* For small graphs, use exact algorithm */
    if (n <= 12) {
        return find_min_vertex_cut_bruteforce(graph, NULL);
    } else {
        /* For larger graphs, use approximation */
        printf("Note: Using approximation for large graph (n=%d). Result may not be exact.\n", n);
        return find_min_vertex_cut_maxflow(graph, NULL);
    }
}

/* Print detailed connectivity number analysis */
void analyze_connectivity_number(Graph *graph) {
    if (!graph) {
        printf("Error: Invalid graph for connectivity number analysis.\n");
        return;
    }
    
    printf("\n=== Connectivity Number Analysis ===\n");
    
    if (graph->is_directed) {
        printf("Note: Graph is directed. Analyzing vertex connectivity as undirected graph.\n");
    }
    
    int n = graph->node_count;
    printf("Graph has %d vertices.\n", n);
    
    /* Count edges */
    int edge_count = 0;
    for (int i = 0; i < n; i++) {
        for (int j = graph->is_directed ? 0 : i + 1; j < n; j++) {
            if (graph->adjacency[i][j]) edge_count++;
        }
    }
    printf("Graph has %d edges.\n", edge_count);
    
    /* Handle trivial cases */
    if (n <= 1) {
        printf("Connectivity number: 0 (trivial graph)\n");
        printf("Analysis: Graph has ≤1 vertex, connectivity undefined or 0.\n");
        return;
    }
    
    if (n == 2) {
        if (edge_count > 0) {
            printf("Connectivity number: 1\n");
            printf("Analysis: Two connected vertices.\n");
        } else {
            printf("Connectivity number: 0\n");
            printf("Analysis: Two disconnected vertices.\n");
        }
        return;
    }
    
    /* Check if already disconnected */
    if (!is_connected_after_removal(graph, NULL, 0)) {
        printf("Connectivity number: 0\n");
        printf("Analysis: Graph is already disconnected.\n");
        return;
    }
    
    /* Calculate minimum and maximum degree */
    int min_degree = n, max_degree = 0;
    int min_degree_vertex = 0;
    
    for (int i = 0; i < n; i++) {
        int degree = 0;
        for (int j = 0; j < n; j++) {
            if (graph->adjacency[i][j]) degree++;
        }
        if (degree < min_degree) {
            min_degree = degree;
            min_degree_vertex = i;
        }
        if (degree > max_degree) {
            max_degree = degree;
        }
    }
    
    printf("Minimum degree: %d (vertex %d)\n", min_degree, min_degree_vertex);
    printf("Maximum degree: %d\n", max_degree);
    printf("Theoretical upper bound: %d (minimum degree)\n", min_degree);
    
    /* Calculate actual connectivity number */
    int *cut_vertices = NULL;
    int connectivity_num;
    
    if (n <= 12) {
        connectivity_num = find_min_vertex_cut_bruteforce(graph, &cut_vertices);
        printf("Connectivity number (exact): %d\n", connectivity_num);
        
        if (cut_vertices && connectivity_num > 0) {
            printf("Minimum vertex cut: ");
            for (int i = 0; i < connectivity_num; i++) {
                printf("v%d ", cut_vertices[i]);
            }
            printf("\n");
        }
    } else {
        connectivity_num = find_min_vertex_cut_maxflow(graph, &cut_vertices);
        printf("Connectivity number (approximation): %d\n", connectivity_num);
    }
    
    /* Analysis of result */
    if (connectivity_num == 0) {
        printf("Analysis: Graph is disconnected.\n");
    } else if (connectivity_num == 1) {
        printf("Analysis: Graph has articulation points (cut vertices).\n");
    } else if (connectivity_num == n - 1) {
        printf("Analysis: Complete graph K_%d.\n", n);
    } else if (connectivity_num == min_degree) {
        printf("Analysis: Connectivity equals minimum degree (common case).\n");
    } else {
        printf("Analysis: Graph is %d-connected.\n", connectivity_num);
    }
    
    /* Additional insights */
    if (connectivity_num >= 2) {
        printf("Property: Graph is 2-connected (no articulation points).\n");
    }
    if (connectivity_num >= 3) {
        printf("Property: Graph remains connected after removing any single vertex.\n");
    }
    
    /* Cleanup */
    if (cut_vertices) free(cut_vertices);
}
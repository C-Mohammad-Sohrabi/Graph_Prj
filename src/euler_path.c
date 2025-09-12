#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "euler_path.h"

/* Simple DFS to test connectivity of non-zero degree vertices */
static void dfs_conn(Graph *graph, int u, bool visited[]) {
    visited[u] = true;
    for (int v = 0; v < graph->node_count; v++) {
        if (graph->adjacency[u][v] && !visited[v]) dfs_conn(graph, v, visited);
    }
}

bool is_connected_for_euler(Graph *graph, int *degrees) {
    int start_node = -1;
    for (int i = 0; i < graph->node_count; i++) {
        if (degrees[i] > 0) { start_node = i; break; }
    }
    if (start_node == -1) return true; /* no edges */

    bool *visited = calloc(graph->node_count, sizeof(bool));
    dfs_conn(graph, start_node, visited);
    for (int i = 0; i < graph->node_count; i++) {
        if (degrees[i] > 0 && !visited[i]) {
            free(visited);
            return false;
        }
    }
    free(visited);
    return true;
}

int count_adj(Graph *graph, int u) {
    int c = 0;
    for (int v = 0; v < graph->node_count; v++) if (graph->adjacency[u][v]) c++;
    return c;
}

/* Hierholzer's algorithm to find Eulerian trail/cycle (undirected) */
void find_euler_path(Graph *original_graph) {
    if (original_graph->is_directed) {
        printf("\nEuler path finding implemented only for undirected graphs.\n");
        return;
    }

    int n = original_graph->node_count;
    int *degrees = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (original_graph->adjacency[i][j]) degrees[i]++;
        }
    }

    if (!is_connected_for_euler(original_graph, degrees)) {
        printf("\nThe graph's non-zero degree vertices are not connected. No Euler Path exists.\n");
        free(degrees);
        return;
    }

    int odd_count = 0;
    int start = -1;
    for (int i = 0; i < n; i++) {
        if (degrees[i] % 2 == 1) {
            odd_count++;
            if (start == -1) start = i;
        }
    }
    if (odd_count != 0 && odd_count != 2) {
        printf("\nGraph is not Eulerian or Semi-Eulerian. Found %d vertices with odd degree.\n", odd_count);
        free(degrees);
        return;
    }
    if (start == -1) { /* Eulerian cycle: choose any vertex with non-zero degree */
        for (int i = 0; i < n; i++) if (degrees[i] > 0) { start = i; break; }
    }
    if (start == -1) {
        printf("\nGraph has no edges (trivial Euler).\n");
        free(degrees);
        return;
    }

    /* Make a local copy of adjacency to remove edges as we traverse */
    int **adj = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        adj[i] = malloc(n * sizeof(int));
        memcpy(adj[i], original_graph->adjacency[i], n * sizeof(int));
    }

    /* stack for current path, vector for final path */
    int *stack = malloc((n * n + 10) * sizeof(int)); /* upper bound */
    int stack_sz = 0;
    int *path = malloc((n * n + 10) * sizeof(int));
    int path_sz = 0;

    stack[stack_sz++] = start;
    while (stack_sz > 0) {
        int v = stack[stack_sz - 1];
        int u = -1;
        for (int w = 0; w < n; w++) {
            if (adj[v][w]) { u = w; break; }
        }
        if (u != -1) {
            /* remove edge v-u */
            adj[v][u] = 0;
            adj[u][v] = 0;
            stack[stack_sz++] = u;
        } else {
            /* backtrack - add to final path */
            path[path_sz++] = v;
            stack_sz--;
        }
    }

    /* path is in reverse order */
    printf("\nEuler Path/Cycle (vertex sequence):\n");
    for (int i = path_sz - 1; i >= 0; i--) {
        if (i != path_sz - 1) printf(" -> ");
        printf("%d", path[i]);
    }
    printf("\n");

    /* cleanup */
    for (int i = 0; i < n; i++) free(adj[i]);
    free(adj);
    free(stack);
    free(path);
    free(degrees);
}

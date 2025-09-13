/**
 * @file euler_path.c
 * @brief Eulerian path/cycle detection using Hierholzer's algorithm
 * @author Graph Theory Project Team
 * @date 2024
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "euler_path.h"

static void dfs_conn(Graph *graph, int u, bool visited[])
{
    visited[u] = true;
    for (int v = 0; v < graph->node_count; v++)
    {
        if (graph->adjacency[u][v] && !visited[v])
            dfs_conn(graph, v, visited);
    }
}

bool is_connected_for_euler(Graph *graph, int *degrees)
{
    int start_node = -1;
    for (int i = 0; i < graph->node_count; i++)
    {
        if (degrees[i] > 0)
        {
            start_node = i;
            break;
        }
    }
    if (start_node == -1)
        return true;

    bool *visited = calloc(graph->node_count, sizeof(bool));
    dfs_conn(graph, start_node, visited);

    for (int i = 0; i < graph->node_count; i++)
    {
        if (degrees[i] > 0 && !visited[i])
        {
            free(visited);
            return false;
        }
    }
    free(visited);
    return true;
}

int count_adj(Graph *graph, int u)
{
    int c = 0;
    for (int v = 0; v < graph->node_count; v++)
        if (graph->adjacency[u][v])
            c++;
    return c;
}

/**
 * @brief Hierholzer's algorithm for finding Eulerian paths/cycles
 */
void find_euler_path(Graph *original_graph)
{
    if (original_graph->is_directed)
    {
        printf("\nEuler path finding implemented only for undirected graphs.\n");
        return;
    }

    int n = original_graph->node_count;
    int *degrees = calloc(n, sizeof(int));

    // Calculate degrees
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (original_graph->adjacency[i][j])
                degrees[i]++;
        }
    }

    // Check connectivity of non-zero degree vertices
    if (!is_connected_for_euler(original_graph, degrees))
    {
        printf("\nGraph's non-zero degree vertices are not connected. No Euler Path.\n");
        free(degrees);
        return;
    }

    // Check Eulerian conditions
    int odd_count = 0, start = -1;
    for (int i = 0; i < n; i++)
    {
        if (degrees[i] % 2 == 1)
        {
            odd_count++;
            if (start == -1)
                start = i;
        }
    }

    if (odd_count != 0 && odd_count != 2)
    {
        printf("\nNot Eulerian: %d vertices with odd degree.\n", odd_count);
        free(degrees);
        return;
    }

    if (start == -1)
    {
        for (int i = 0; i < n; i++)
            if (degrees[i] > 0)
            {
                start = i;
                break;
            }
    }

    if (start == -1)
    {
        printf("\nTrivial Euler path (no edges).\n");
        free(degrees);
        return;
    }

    // Copy adjacency matrix for modification
    int **adj = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        adj[i] = malloc(n * sizeof(int));
        memcpy(adj[i], original_graph->adjacency[i], n * sizeof(int));
    }

    // Hierholzer's algorithm
    int *stack = malloc((n * n + 10) * sizeof(int));
    int stack_sz = 0;
    int *path = malloc((n * n + 10) * sizeof(int));
    int path_sz = 0;

    stack[stack_sz++] = start;
    while (stack_sz > 0)
    {
        int v = stack[stack_sz - 1];
        int u = -1;
        for (int w = 0; w < n; w++)
        {
            if (adj[v][w])
            {
                u = w;
                break;
            }
        }
        if (u != -1)
        {
            adj[v][u] = adj[u][v] = 0;
            stack[stack_sz++] = u;
        }
        else
        {
            path[path_sz++] = v;
            stack_sz--;
        }
    }

    printf("\n=== Euler Path/Cycle ===\n");
    printf("Path: ");
    for (int i = path_sz - 1; i >= 0; i--)
    {
        if (i != path_sz - 1)
            printf(" -> ");
        printf("%d", path[i]);
    }
    printf("\nLength: %d vertices\n", path_sz);

    for (int i = 0; i < n; i++)
        free(adj[i]);
    free(adj);
    free(stack);
    free(path);
    free(degrees);
}
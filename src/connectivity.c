#include "connectivity.h"

Connectivity check_connectivity(Graph *graph) {
    Connectivity conn = {false, false, true, false};
    bool *visited = calloc(graph->node_count, sizeof(bool));
    int *queue = malloc(graph->node_count * sizeof(int));
    int front, rear;

    if (graph->is_directed) {
        front = rear = 0;
        queue[rear++] = 0;
        visited[0] = true;
        int reachable_count = 1;
        while (front < rear) {
            int current = queue[front++];
            for (int i = 0; i < graph->node_count; i++) {
                if (graph->adjacency[current][i] > 0 && !visited[i]) {
                    visited[i] = true;
                    queue[rear++] = i;
                    reachable_count++;
                }
            }
        }
        conn.is_strong = (reachable_count == graph->node_count);

        free(visited);
        visited = calloc(graph->node_count, sizeof(bool));
        front = rear = 0;
        queue[rear++] = 0;
        visited[0] = true;
        reachable_count = 1;
        while (front < rear) {
            int current = queue[front++];
            for (int i = 0; i < graph->node_count; i++) {
                if ((graph->adjacency[current][i] > 0 || graph->adjacency[i][current] > 0) && !visited[i]) {
                    visited[i] = true;
                    queue[rear++] = i;
                    reachable_count++;
                }
            }
        }
        conn.is_weak = (reachable_count == graph->node_count);

        for (int i = 0; i < graph->node_count; i++) {
            for (int j = 0; j < graph->node_count; j++) {
                if (graph->adjacency[i][j] > 0 && graph->adjacency[j][i] > 0 && !graph->allow_bidirectional) {
                    conn.is_one_sided = false;
                    break;
                }
            }
            if (!conn.is_one_sided)
                break;
        }
    } else {
        front = rear = 0;
        if (graph->node_count > 0) {
            queue[rear++] = 0;
            visited[0] = true;
        }
        int reachable_count = (graph->node_count > 0) ? 1 : 0;
        while (front < rear) {
            int current = queue[front++];
            for (int i = 0; i < graph->node_count; i++) {
                if (graph->adjacency[current][i] > 0 && !visited[i]) {
                    visited[i] = true;
                    queue[rear++] = i;
                    reachable_count++;
                }
            }
        }
        conn.is_connected = (reachable_count == graph->node_count);
    }

    free(visited);
    free(queue);
    return conn;
}
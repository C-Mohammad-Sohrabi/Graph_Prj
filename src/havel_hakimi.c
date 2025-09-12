#include "havel_hakimi.h"

int compare_nodes(const void *a, const void *b) {
    Node *node_a = (Node *)a;
    Node *node_b = (Node *)b;
    if (node_a->degree != node_b->degree) {
        return node_b->degree - node_a->degree;
    }
    return node_a->original_index - node_b->original_index;
}

int compare_nodes_in_degree(const void *a, const void *b) {
    Node *node_a = (Node *)a;
    Node *node_b = (Node *)b;
    if (node_a->in_degree != node_b->in_degree) {
        return node_b->in_degree - node_a->in_degree;
    }
    return node_a->original_index - node_b->original_index;
}

bool havel_hakimi_undirected(Node *nodes, int n, FILE *dot_file, Graph *graph) {
    fprintf(dot_file, "graph G {\n");
    graph->node_count = n;
    graph->is_directed = false;
    graph->allow_bidirectional = false;
    graph->adjacency = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        graph->adjacency[i] = calloc(n, sizeof(int));
    }

    while (true) {
        qsort(nodes, n, sizeof(Node), compare_nodes);
        int idx = 0;
        while (idx < n && nodes[idx].degree == 0) {
            idx++;
        }
        if (idx == n) {
            break;
        }

        Node u = nodes[idx];
        int d = u.degree;
        if (d > n - 1 - idx) {
            return false;
        }

        int edges_added = 0;
        for (int i = idx + 1; i < n && edges_added < d; i++) {
            Node v = nodes[i];
            if (v.degree > 0 && u.original_index != v.original_index && graph->adjacency[u.original_index][v.original_index] == 0) {
                graph->adjacency[u.original_index][v.original_index] = 1;
                graph->adjacency[v.original_index][u.original_index] = 1;
                fprintf(dot_file, "  %d -- %d;\n", u.original_index, v.original_index);
                nodes[i].degree--;
                edges_added++;
            }
        }
        if (edges_added < d) {
            return false;
        }
        nodes[idx].degree = 0;
    }

    fprintf(dot_file, "}\n");
    return true;
}

bool havel_hakimi_directed(Node *nodes, int n, FILE *dot_file, Graph *graph, bool allow_bidirectional) {
    fprintf(dot_file, "digraph G {\n");
    graph->node_count = n;
    graph->is_directed = true;
    graph->allow_bidirectional = allow_bidirectional;
    graph->adjacency = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        graph->adjacency[i] = calloc(n, sizeof(int));
    }

    while (true) {
        Node *active_out = malloc(n * sizeof(Node));
        Node *active_in = malloc(n * sizeof(Node));
        int out_count = 0, in_count = 0;
        for (int i = 0; i < n; i++) {
            if (nodes[i].degree > 0) {
                active_out[out_count++] = nodes[i];
            }
            if (nodes[i].in_degree > 0) {
                active_in[in_count++] = nodes[i];
            }
        }
        if (out_count == 0 || in_count == 0) {
            free(active_out);
            free(active_in);
            break;
        }
        qsort(active_out, out_count, sizeof(Node), compare_nodes);
        Node u = active_out[0];
        qsort(active_in, in_count, sizeof(Node), compare_nodes_in_degree);

        bool edge_added = false;
        for (int i = 0; i < in_count; i++) {
            Node v = active_in[i];
            if (u.original_index != v.original_index && graph->adjacency[u.original_index][v.original_index] == 0 &&
                (allow_bidirectional || graph->adjacency[v.original_index][u.original_index] == 0)) {
                graph->adjacency[u.original_index][v.original_index] = 1;
                fprintf(dot_file, "  %d -> %d;\n", u.original_index, v.original_index);
                for (int j = 0; j < n; j++) {
                    if (nodes[j].original_index == v.original_index) {
                        nodes[j].in_degree--;
                        break;
                    }
                }
                edge_added = true;
                break;
            }
        }
        if (!edge_added) {
            free(active_out);
            free(active_in);
            return false;
        }
        for (int j = 0; j < n; j++) {
            if (nodes[j].original_index == u.original_index) {
                nodes[j].degree--;
                break;
            }
        }
        free(active_out);
        free(active_in);
    }

    for (int i = 0; i < n; i++) {
        if (nodes[i].degree != 0 || nodes[i].in_degree != 0) {
            return false;
        }
    }
    fprintf(dot_file, "}\n");
    return true;
}
#include "line_graph.h"

int extract_edges_from_adjacency(Graph *graph, Edge **edges) {
    int edge_count = 0;
    for (int i = 0; i < graph->node_count; i++) {
        for (int j = (graph->is_directed ? 0 : i + 1); j < graph->node_count; j++) {
            if (graph->adjacency[i][j] == 1) {
                edge_count++;
            }
        }
    }

    *edges = malloc(edge_count * sizeof(Edge));
    if (!*edges) {
        return 0;
    }

    int edge_index = 0;
    for (int i = 0; i < graph->node_count; i++) {
        for (int j = (graph->is_directed ? 0 : i + 1); j < graph->node_count; j++) {
            if (graph->adjacency[i][j] == 1) {
                (*edges)[edge_index].u = i;
                (*edges)[edge_index].v = j;
                edge_index++;
            }
        }
    }

    return edge_count;
}

void build_node_edges_mapping(Edge *edges, int edge_count, int node_count, int ***node_edges, int **node_edges_count) {
    *node_edges = malloc(node_count * sizeof(int *));
    *node_edges_count = calloc(node_count, sizeof(int));
    for (int i = 0; i < node_count; i++) {
        (*node_edges)[i] = NULL;
    }

    for (int i = 0; i < edge_count; i++) {
        (*node_edges_count)[edges[i].u]++;
        (*node_edges_count)[edges[i].v]++;
    }

    for (int i = 0; i < node_count; i++) {
        if ((*node_edges_count)[i] > 0) {
            (*node_edges)[i] = malloc((*node_edges_count)[i] * sizeof(int));
        }
    }

    int *current_counts = calloc(node_count, sizeof(int));
    for (int i = 0; i < edge_count; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        (*node_edges)[u][current_counts[u]++] = i;
        (*node_edges)[v][current_counts[v]++] = i;
    }
    free(current_counts);
}

void build_line_graph(Edge *edges, int edge_count, int **node_edges, int *node_edges_count, int node_count, AdjList **line_graph_adj) {
    *line_graph_adj = malloc(edge_count * sizeof(AdjList));
    for (int i = 0; i < edge_count; i++) {
        (*line_graph_adj)[i].adjacent = NULL;
        (*line_graph_adj)[i].count = 0;
        (*line_graph_adj)[i].capacity = 0;
    }

    for (int node = 0; node < node_count; node++) {
        int count = node_edges_count[node];
        int *incident_edges = node_edges[node];
        for (int i = 0; i < count; i++) {
            for (int j = i + 1; j < count; j++) {
                int e1 = incident_edges[i];
                int e2 = incident_edges[j];
                if ((*line_graph_adj)[e1].count >= (*line_graph_adj)[e1].capacity) {
                    (*line_graph_adj)[e1].capacity = (*line_graph_adj)[e1].capacity == 0 ? 1 : (*line_graph_adj)[e1].capacity * 2;
                    (*line_graph_adj)[e1].adjacent = realloc((*line_graph_adj)[e1].adjacent, (*line_graph_adj)[e1].capacity * sizeof(int));
                }
                (*line_graph_adj)[e1].adjacent[(*line_graph_adj)[e1].count++] = e2;
                if ((*line_graph_adj)[e2].count >= (*line_graph_adj)[e2].capacity) {
                    (*line_graph_adj)[e2].capacity = (*line_graph_adj)[e2].capacity == 0 ? 1 : (*line_graph_adj)[e2].capacity * 2;
                    (*line_graph_adj)[e2].adjacent = realloc((*line_graph_adj)[e2].adjacent, (*line_graph_adj)[e2].capacity * sizeof(int));
                }
                (*line_graph_adj)[e2].adjacent[(*line_graph_adj)[e2].count++] = e1;
            }
        }
    }
}

void generate_line_graph_dot(Edge *edges, int edge_count, AdjList *line_graph_adj, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to create line graph DOT file");
        return;
    }

    fprintf(file, "graph LineGraph {\n");
    fprintf(file, "  node [shape=circle];\n");
    for (int i = 0; i < edge_count; i++) {
        fprintf(file, "  E%d [label=\"E%d\\n(%d-%d)\"];\n", i, i, edges[i].u, edges[i].v);
    }
    for (int i = 0; i < edge_count; i++) {
        for (int j = 0; j < line_graph_adj[i].count; j++) {
            int neighbor = line_graph_adj[i].adjacent[j];
            if (i < neighbor) {
                fprintf(file, "  E%d -- E%d;\n", i, neighbor);
            }
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    printf("Line graph DOT file created: %s\n", filename);
}

void generate_line_graph(Graph *graph) {
    printf("\nGenerating line graph...\n");
    Edge *edges;
    int edge_count = extract_edges_from_adjacency(graph, &edges);
    if (edge_count == 0) {
        printf("No edges found in the original graph. Cannot create line graph.\n");
        return;
    }
    printf("Found %d edges in the original graph.\n", edge_count);
    int **node_edges;
    int *node_edges_count;
    build_node_edges_mapping(edges, edge_count, graph->node_count, &node_edges, &node_edges_count);
    AdjList *line_graph_adj;
    build_line_graph(edges, edge_count, node_edges, node_edges_count, graph->node_count, &line_graph_adj);
    generate_line_graph_dot(edges, edge_count, line_graph_adj, "dot_file/line_graph.dot");
    printf("Line graph DOT file generated: dot_file/line_graph.dot\n");
    free(edges);
    for (int i = 0; i < graph->node_count; i++) {
        free(node_edges[i]);
    }
    free(node_edges);
    free(node_edges_count);
    for (int i = 0; i < edge_count; i++) {
        free(line_graph_adj[i].adjacent);
    }
    free(line_graph_adj);
}
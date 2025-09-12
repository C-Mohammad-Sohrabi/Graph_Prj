#ifndef LINE_GRAPH_H
#define LINE_GRAPH_H

#include "structs.h"

int extract_edges_from_adjacency(Graph *graph, Edge **edges);
void build_node_edges_mapping(Edge *edges, int edge_count, int node_count, int ***node_edges, int **node_edges_count);
void build_line_graph(Edge *edges, int edge_count, int **node_edges, int *node_edges_count, int node_count, AdjList **line_graph_adj);
void generate_line_graph_dot(Edge *edges, int edge_count, AdjList *line_graph_adj, const char *filename);
void generate_line_graph(Graph *graph);

#endif // LINE_GRAPH_H
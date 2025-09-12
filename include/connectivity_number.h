#ifndef CONNECTIVITY_NUMBER_H
#define CONNECTIVITY_NUMBER_H

#include "structs.h"

/* Calculate the vertex connectivity number of the graph
 * Returns the minimum number of vertices needed to disconnect the graph */
int calculate_connectivity_number(Graph *graph);

/* Helper function to check if graph is connected after removing vertices */
bool is_connected_after_removal(Graph *graph, int *removed_vertices, int removed_count);

/* Find minimum vertex cut using brute force for small graphs */
int find_min_vertex_cut_bruteforce(Graph *graph, int **cut_vertices_out);

/* Ford-Fulkerson based approach for larger graphs */
int find_min_vertex_cut_maxflow(Graph *graph, int **cut_vertices_out);

/* Print the connectivity number analysis results */
void analyze_connectivity_number(Graph *graph);

#endif // CONNECTIVITY_NUMBER_H
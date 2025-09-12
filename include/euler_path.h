#ifndef EULER_PATH_H
#define EULER_PATH_H

#include "structs.h"
#include <stdbool.h>

/* Returns true if the subgraph of vertices with degree>0 is connected (undirected) */
bool is_connected_for_euler(Graph *graph, int *degrees);

/* Count adjacency (degree) of vertex u */
int count_adj(Graph *graph, int u);

/* Find and print an Euler trail (or cycle) using Hierholzer's algorithm for undirected graphs.
 * Prints the full vertex sequence of the Euler path/trail if exists. */
void find_euler_path(Graph *original_graph);

#endif // EULER_PATH_H

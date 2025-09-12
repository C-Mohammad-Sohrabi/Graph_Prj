#ifndef INDEPENDENT_SET_H
#define INDEPENDENT_SET_H

#include "structs.h"

/* Create complement of an undirected graph (returns malloc'd Graph*). 
 * Caller must free adjacency and graph struct. Returns NULL if original->is_directed. */
Graph *create_complement_graph(Graph *original);

/* Find maximum independent set of graph by computing maximum clique on complement.
 * Returns a Set* (caller must set_destroy). Returns NULL if none. */
Set *find_maximum_independent_set(Graph *graph);

/* Find a minimum vertex cover by using the relation:
 * minimum_vertex_cover_size = n - maximum_independent_set_size.
 * Returns a Set* (caller must set_destroy). Returns NULL on error. */
Set *find_minimum_vertex_cover(Graph *graph);

#endif // INDEPENDENT_SET_H

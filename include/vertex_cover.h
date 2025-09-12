#ifndef VERTEX_COVER_H
#define VERTEX_COVER_H

#include "structs.h"
#include "set_utils.h"

/* Exact via MIS (complement graph + max clique). Returns Set* (caller must set_destroy).
 * Returns NULL on error (e.g., directed graphs). */
Set *vertex_cover_exact_via_mis(Graph *graph);

/* If graph is bipartite, compute minimum vertex cover using Konig's theorem (Hopcroft-Karp).
 * Returns Set* (caller must set_destroy). Returns NULL if graph not bipartite or on error. */
Set *vertex_cover_bipartite_konig(Graph *graph);
    
/* Fast 2-approximation via maximal matching (greedy). Always returns a Set* (caller must set_destroy). */
Set *vertex_cover_approx(Graph *graph);

/* Helper that checks bipartiteness and produces a partition:
 * - left_mask and right_mask will be allocated (size graph->node_count) and must be freed by caller.
 * - returns true if bipartite and fills left_mask/right_mask (0/1), false otherwise. */
bool is_bipartite_partition(Graph *graph, char **left_mask_out, char **right_mask_out);

#endif // VERTEX_COVER_H

#ifndef CLIQUE_H
#define CLIQUE_H

#include "structs.h"

/* Find all (possibly duplicate) cliques (backtracking) */
void find_all_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***all_cliques, int *count);

/* Find maximal cliques (branch & bound style / Tomita-like pivot) */
void find_maximal_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***maximal_cliques, int *count);

/* Find one maximum clique (largest maximal clique found) */
Set *find_maximum_clique(Graph *graph);

/* Helper to print/analyze cliques */
void analyze_cliques(Graph *graph, int algorithm_choice);

#endif // CLIQUE_H

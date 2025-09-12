#ifndef HAVEL_HAKIMI_H
#define HAVEL_HAKIMI_H

#include "structs.h"

int compare_nodes(const void *a, const void *b);
int compare_nodes_in_degree(const void *a, const void *b);
bool havel_hakimi_undirected(Node *nodes, int n, FILE *dot_file, Graph *graph);
bool havel_hakimi_directed(Node *nodes, int n, FILE *dot_file, Graph *graph, bool allow_bidirectional);

#endif // HAVEL_HAKIMI_H
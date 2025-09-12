#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "independent_set.h"
#include "clique.h"
#include "set_utils.h"

/* Create complement graph for undirected graphs */
Graph *create_complement_graph(Graph *original) {
    if (original == NULL) return NULL;
    if (original->is_directed) {
        /* For now, don't support directed complements in this helper */
        return NULL;
    }

    Graph *complement = malloc(sizeof(Graph));
    complement->node_count = original->node_count;
    complement->is_directed = false;
    complement->allow_bidirectional = false;

    complement->adjacency = malloc(complement->node_count * sizeof(int *));
    for (int i = 0; i < complement->node_count; i++) {
        complement->adjacency[i] = malloc(complement->node_count * sizeof(int));
        for (int j = 0; j < complement->node_count; j++) {
            if (i == j) complement->adjacency[i][j] = 0;
            else complement->adjacency[i][j] = 1 - original->adjacency[i][j];
        }
    }
    return complement;
}

/* Use clique module on complement to obtain maximum independent set */
Set *find_maximum_independent_set(Graph *graph) {
    if (!graph) return NULL;
    if (graph->is_directed) return NULL; /* not supported here */

    Graph *comp = create_complement_graph(graph);
    if (!comp) return NULL;

    Set *max_clique = find_maximum_clique(comp); /* max clique in complement == max independent set */
    /* free complement adjacency but keep max_clique (it is independent) */
    for (int i = 0; i < comp->node_count; i++) free(comp->adjacency[i]);
    free(comp->adjacency);
    free(comp);

    return max_clique; /* caller must free with set_destroy */
}

/* Minimum vertex cover = V \ (maximum independent set) */
Set *find_minimum_vertex_cover(Graph *graph) {
    if (!graph) return NULL;
    if (graph->is_directed) return NULL; /* not supported */

    Set *mis = find_maximum_independent_set(graph);
    if (!mis) return NULL;

    /* mark MIS vertices */
    char *in_mis = calloc(graph->node_count, sizeof(char));
    for (int i = 0; i < mis->size; i++) {
        int v = mis->vertices[i];
        if (v >= 0 && v < graph->node_count) in_mis[v] = 1;
    }

    Set *cover = set_create(graph->node_count);
    for (int v = 0; v < graph->node_count; v++) {
        if (!in_mis[v]) set_add(cover, v);
    }

    free(in_mis);
    set_destroy(mis); /* free MIS; we already used it */
    return cover; /* caller must set_destroy */
}

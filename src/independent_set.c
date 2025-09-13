#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "independent_set.h"
#include "clique.h"
#include "set_utils.h"

/**
 * @file independent_set.c
 * @brief Maximum independent set algorithms using complement graph approach
 * @author Graph Theory Project Team
 * @date 2024
 */

/**
 * @brief Creates complement graph where edges exist iff they don't exist in original
 * 
 * The complement graph G' has the same vertices as G, but an edge (u,v) exists in G'
 * if and only if it doesn't exist in G. This transformation allows us to use:
 * Maximum Independent Set in G = Maximum Clique in G'
 * 
 * @param original Pointer to original undirected graph
 * @return Newly allocated complement graph, NULL if directed or allocation fails
 */
Graph *create_complement_graph(Graph *original) {
    if (original == NULL || original->is_directed) return NULL;

    Graph *complement = malloc(sizeof(Graph));
    complement->node_count = original->node_count;
    complement->is_directed = false;
    complement->allow_bidirectional = false;

    // Allocate adjacency matrix for complement
    complement->adjacency = malloc(complement->node_count * sizeof(int *));
    for (int i = 0; i < complement->node_count; i++) {
        complement->adjacency[i] = malloc(complement->node_count * sizeof(int));
        for (int j = 0; j < complement->node_count; j++) {
            if (i == j) 
                complement->adjacency[i][j] = 0;  // No self-loops
            else 
                complement->adjacency[i][j] = 1 - original->adjacency[i][j];  // Complement
        }
    }
    return complement;
}

/**
 * @brief Finds maximum independent set using complement graph and maximum clique
 * 
 * Uses the theoretical equivalence: Maximum Independent Set in G = Maximum Clique in complement(G)
 * This allows reusing efficient clique detection algorithms.
 */
Set *find_maximum_independent_set(Graph *graph) {
    if (!graph || graph->is_directed) return NULL;

    // Create complement graph
    Graph *comp = create_complement_graph(graph);
    if (!comp) return NULL;

    // Find maximum clique in complement = maximum independent set in original
    Set *max_clique = find_maximum_clique(comp);
    
    // Cleanup complement graph
    for (int i = 0; i < comp->node_count; i++) free(comp->adjacency[i]);
    free(comp->adjacency);
    free(comp);

    return max_clique;
}

/**
 * @brief Finds minimum vertex cover using relationship: Min VC = V \ Max IS
 */
Set *find_minimum_vertex_cover(Graph *graph) {
    if (!graph || graph->is_directed) return NULL;

    Set *mis = find_maximum_independent_set(graph);
    if (!mis) return NULL;

    // Mark vertices in maximum independent set
    char *in_mis = calloc(graph->node_count, sizeof(char));
    for (int i = 0; i < mis->size; i++) {
        int v = mis->vertices[i];
        if (v >= 0 && v < graph->node_count) in_mis[v] = 1;
    }

    // Vertex cover = all vertices NOT in maximum independent set
    Set *cover = set_create(graph->node_count);
    for (int v = 0; v < graph->node_count; v++) {
        if (!in_mis[v]) set_add(cover, v);
    }

    free(in_mis);
    set_destroy(mis);
    return cover;
}
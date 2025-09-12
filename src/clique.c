#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clique.h"
#include "set_utils.h"

/* find_all_cliques: backtracking enumerator (may include duplicates) */
void find_all_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***all_cliques, int *count) {
    if (C->size > 0) {
        Set *new_clique = set_create(C->size);
        memcpy(new_clique->vertices, C->vertices, C->size * sizeof(int));
        new_clique->size = C->size;
        (*all_cliques) = realloc(*all_cliques, (*count + 1) * sizeof(Set *));
        (*all_cliques)[(*count)++] = new_clique;
    }

    for (int i = 0; i < P->size; i++) {
        int v = P->vertices[i];

        Set *PP = set_create(P->size);
        Set *SS = set_create(S->size);

        for (int j = 0; j < P->size; j++) {
            int w = P->vertices[j];
            if (graph->adjacency[v][w]) set_add(PP, w);
        }
        for (int j = 0; j < S->size; j++) {
            int w = S->vertices[j];
            if (graph->adjacency[v][w]) set_add(SS, w);
        }

        set_add(C, v);
        find_all_cliques(graph, C, PP, SS, all_cliques, count);
        set_remove(C);

        set_destroy(PP);
        set_destroy(SS);

        /* move v from P -> S */
        for (int j = i; j < P->size - 1; j++) P->vertices[j] = P->vertices[j + 1];
        P->size--;
        i--;
        set_add(S, v);
    }
}

/* find_maximal_cliques: branch-and-bound-ish approach */
void find_maximal_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***maximal_cliques, int *count) {
    if (P->size == 0 && S->size == 0) {
        Set *new_clique = set_create(C->size);
        memcpy(new_clique->vertices, C->vertices, C->size * sizeof(int));
        new_clique->size = C->size;
        (*maximal_cliques) = realloc(*maximal_cliques, (*count + 1) * sizeof(Set *));
        (*maximal_cliques)[(*count)++] = new_clique;
        return;
    }

    /* choose pivot u with maximum degree into P∪S (heuristic) */
    int u = -1;
    int max_intersection_size = -1;
    for (int list = 0; list < 2; list++) {
        Set *sel = (list == 0) ? P : S;
        for (int i = 0; i < sel->size; i++) {
            int vertex = sel->vertices[i];
            int current_intersection_size = 0;
            for (int j = 0; j < P->size; j++) {
                if (graph->adjacency[vertex][P->vertices[j]]) current_intersection_size++;
            }
            if (current_intersection_size > max_intersection_size) {
                max_intersection_size = current_intersection_size;
                u = vertex;
            }
        }
    }

    /* P \ N(u) */
    Set *P_without_Nu = set_create(P->size);
    for (int i = 0; i < P->size; i++) {
        int v = P->vertices[i];
        if (u == -1 || !graph->adjacency[u][v]) set_add(P_without_Nu, v);
    }

    int *candidates = malloc(P_without_Nu->size * sizeof(int));
    int cand_size = P_without_Nu->size;
    memcpy(candidates, P_without_Nu->vertices, cand_size * sizeof(int));
    set_destroy(P_without_Nu);

    for (int i = 0; i < cand_size; i++) {
        int v = candidates[i];

        Set *P_intersect_Nv = set_create(graph->node_count);
        Set *S_intersect_Nv = set_create(graph->node_count);

        for (int j = 0; j < P->size; j++) {
            int w = P->vertices[j];
            if (graph->adjacency[v][w]) set_add(P_intersect_Nv, w);
        }
        for (int j = 0; j < S->size; j++) {
            int w = S->vertices[j];
            if (graph->adjacency[v][w]) set_add(S_intersect_Nv, w);
        }

        set_add(C, v);
        find_maximal_cliques(graph, C, P_intersect_Nv, S_intersect_Nv, maximal_cliques, count);
        set_remove(C);

        /* remove v from P and add to S */
        for (int k = 0; k < P->size; k++) {
            if (P->vertices[k] == v) {
                P->vertices[k] = P->vertices[P->size - 1];
                P->size--;
                break;
            }
        }
        set_add(S, v);

        set_destroy(P_intersect_Nv);
        set_destroy(S_intersect_Nv);
    }

    free(candidates);
}

/* find_maximum_clique: runs find_maximal_cliques and picks largest */
Set *find_maximum_clique(Graph *graph) {
    Set **maximal_cliques = NULL;
    int count = 0;

    Set *C = set_create(graph->node_count);
    Set *P = set_create(graph->node_count);
    Set *S = set_create(graph->node_count);

    for (int i = 0; i < graph->node_count; i++) set_add(P, i);

    find_maximal_cliques(graph, C, P, S, &maximal_cliques, &count);

    set_destroy(C);
    set_destroy(P);
    set_destroy(S);

    int max_size = 0;
    Set *max_clique = NULL;
    for (int i = 0; i < count; i++) {
        if (maximal_cliques[i]->size > max_size) {
            max_size = maximal_cliques[i]->size;
            max_clique = maximal_cliques[i];
        }
    }

    /* free other cliques, keep max_clique for caller */
    for (int i = 0; i < count; i++) {
        if (maximal_cliques[i] != max_clique) set_destroy(maximal_cliques[i]);
    }
    free(maximal_cliques);

    return max_clique; /* may be NULL if none found */
}

/* analyze_cliques: convenience printing wrapper (keeps previous behavior) */
void analyze_cliques(Graph *graph, int algorithm_choice) {
    Set *C = set_create(graph->node_count);
    Set *P = set_create(graph->node_count);
    Set *S = set_create(graph->node_count);
    for (int i = 0; i < graph->node_count; i++) set_add(P, i);

    Set **cliques = NULL;
    int count = 0;
    int max_clique_size = 0;

    if (algorithm_choice == 1) find_all_cliques(graph, C, P, S, &cliques, &count);
    else find_maximal_cliques(graph, C, P, S, &cliques, &count);

    for (int i = 0; i < count; i++) {
        if (cliques[i]->size > max_clique_size) max_clique_size = cliques[i]->size;
    }

    printf("\nClique Analysis:\n  Maximum clique size: %d\n  Non-trivial cliques (size >= 3):\n", max_clique_size);
    for (int i = 0; i < count; i++) {
        if (cliques[i]->size >= 3) {
            printf("    Clique: ");
            for (int j = 0; j < cliques[i]->size; j++) printf("%d ", cliques[i]->vertices[j]);
            printf("\n");
        }
        set_destroy(cliques[i]);
    }
    free(cliques);

    set_destroy(C);
    set_destroy(P);
    set_destroy(S);
}

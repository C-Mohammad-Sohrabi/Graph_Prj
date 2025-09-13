#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clique.h"
#include "set_utils.h"

/**
 * @file clique.c
 * @brief Clique detection algorithms implementation
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This file implements multiple algorithms for finding cliques in undirected graphs.
 * A clique is a subset of vertices where every pair of vertices is adjacent.
 * 
 * Implemented algorithms:
 * 1. Backtracking algorithm: Finds all cliques (may include duplicates)
 * 2. Bron-Kerbosch algorithm: Finds maximal cliques efficiently with pivot selection
 * 
 * Both algorithms use the fundamental sets:
 * - R (or C): Current clique being constructed
 * - P: Candidate vertices that can extend the current clique
 * - X (or S): Vertices already processed/excluded
 */

/**
 * @brief Finds all cliques using recursive backtracking algorithm
 * 
 * This function implements a recursive backtracking approach to enumerate all possible
 * cliques in the graph. The algorithm explores all combinations of vertices and
 * identifies which ones form cliques.
 * 
 * Algorithm Description:
 * 1. If current set C is non-empty, it's a clique - add to results
 * 2. For each candidate vertex v in P:
 *    - Create new candidate set P' = P ∩ N(v) (neighbors of v)
 *    - Create new excluded set S' = S ∩ N(v)
 *    - Recursively search with C ∪ {v}, P', S'
 *    - Move v from P to S for future iterations
 * 
 * Note: This algorithm may find duplicate cliques due to different discovery paths.
 * It's more exhaustive but less efficient than Bron-Kerbosch for maximal cliques.
 * 
 * @param graph Pointer to the graph structure
 * @param C Current clique set being constructed
 * @param P Candidate set of vertices that can extend current clique
 * @param S Excluded set of vertices already processed
 * @param all_cliques Pointer to dynamically allocated array of clique pointers
 * @param count Pointer to counter tracking number of cliques found
 */
void find_all_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***all_cliques, int *count) {
    /* ========================================================================
     * CLIQUE RECORDING: Store current clique if non-empty
     * ========================================================================*/
    
    // If current set C is non-empty, it represents a valid clique
    if (C->size > 0) {
        // Create a new set to store this clique
        Set *new_clique = set_create(C->size);
        memcpy(new_clique->vertices, C->vertices, C->size * sizeof(int));
        new_clique->size = C->size;
        
        // Dynamically expand the cliques array
        (*all_cliques) = realloc(*all_cliques, (*count + 1) * sizeof(Set *));
        (*all_cliques)[(*count)++] = new_clique;
    }

    /* ========================================================================
     * RECURSIVE EXPLORATION: Try extending clique with each candidate
     * ========================================================================*/
    
    // Try each vertex in the candidate set P
    for (int i = 0; i < P->size; i++) {
        int v = P->vertices[i];

        /* ====================================================================
         * NEIGHBOR INTERSECTION: Create new P and S sets
         * ====================================================================*/
        
        // Create new candidate set: P' = P ∩ N(v)
        Set *PP = set_create(P->size);
        for (int j = 0; j < P->size; j++) {
            int w = P->vertices[j];
            if (graph->adjacency[v][w]) {   // w is neighbor of v
                set_add(PP, w);
            }
        }
        
        // Create new excluded set: S' = S ∩ N(v)
        Set *SS = set_create(S->size);
        for (int j = 0; j < S->size; j++) {
            int w = S->vertices[j];
            if (graph->adjacency[v][w]) {   // w is neighbor of v
                set_add(SS, w);
            }
        }

        /* ====================================================================
         * RECURSIVE CALL: Extend clique with vertex v
         * ====================================================================*/
        
        // Add v to current clique
        set_add(C, v);
        
        // Recurse with extended clique and restricted candidate/excluded sets
        find_all_cliques(graph, C, PP, SS, all_cliques, count);
        
        // Backtrack: remove v from current clique
        set_remove(C);

        /* ====================================================================
         * CLEANUP AND STATE UPDATE: Prepare for next iteration
         * ====================================================================*/
        
        // Free temporary sets
        set_destroy(PP);
        set_destroy(SS);

        // Move vertex v from P to S (mark as processed)
        // Remove v from P by shifting remaining elements
        for (int j = i; j < P->size - 1; j++) {
            P->vertices[j] = P->vertices[j + 1];
        }
        P->size--;
        i--;  // Adjust index since we removed an element
        
        // Add v to excluded set S
        set_add(S, v);
    }
}

/**
 * @brief Finds maximal cliques using Bron-Kerbosch algorithm with pivot selection
 * 
 * Implements the Bron-Kerbosch algorithm with pivot heuristic for efficiently finding
 * all maximal cliques. A maximal clique is one that cannot be extended by adding
 * another vertex.
 * 
 * Algorithm with Pivot:
 * 1. If P and S are both empty, C is a maximal clique
 * 2. Choose pivot u ∈ P ∪ S with maximum |N(u) ∩ P|
 * 3. For each v ∈ P \ N(u):
 *    - Recursively call with R ∪ {v}, P ∩ N(v), S ∩ N(v)
 *    - Move v from P to S
 * 
 * The pivot selection reduces the branching factor by avoiding vertices
 * that are adjacent to the pivot, leading to significant performance improvements.
 * 
 * @param graph Pointer to the graph structure
 * @param C Current clique being constructed (R in classical notation)
 * @param P Candidate set of vertices
 * @param S Excluded set of vertices
 * @param maximal_cliques Pointer to array of maximal clique pointers
 * @param count Pointer to counter of maximal cliques found
 */
void find_maximal_cliques(Graph *graph, Set *C, Set *P, Set *S, Set ***maximal_cliques, int *count) {
    /* ========================================================================
     * BASE CASE: Check for maximal clique
     * ========================================================================*/
    
    // If both P and S are empty, C is a maximal clique
    if (P->size == 0 && S->size == 0) {
        Set *new_clique = set_create(C->size);
        memcpy(new_clique->vertices, C->vertices, C->size * sizeof(int));
        new_clique->size = C->size;
        
        (*maximal_cliques) = realloc(*maximal_cliques, (*count + 1) * sizeof(Set *));
        (*maximal_cliques)[(*count)++] = new_clique;
        return;
    }

    /* ========================================================================
     * PIVOT SELECTION: Choose vertex to minimize branching
     * ========================================================================*/
    
    // Find pivot u with maximum intersection with P (from P ∪ S)
    int u = -1;
    int max_intersection_size = -1;
    
    // Check vertices in both P and S for best pivot
    for (int list = 0; list < 2; list++) {
        Set *sel = (list == 0) ? P : S;
        
        for (int i = 0; i < sel->size; i++) {
            int vertex = sel->vertices[i];
            int current_intersection_size = 0;
            
            // Count |N(vertex) ∩ P|
            for (int j = 0; j < P->size; j++) {
                if (graph->adjacency[vertex][P->vertices[j]]) {
                    current_intersection_size++;
                }
            }
            
            // Update pivot if this vertex has larger intersection
            if (current_intersection_size > max_intersection_size) {
                max_intersection_size = current_intersection_size;
                u = vertex;
            }
        }
    }

    /* ========================================================================
     * CANDIDATE SET CONSTRUCTION: P \ N(u)
     * ========================================================================*/
    
    // Create set P \ N(u) - vertices not adjacent to pivot
    Set *P_without_Nu = set_create(P->size);
    for (int i = 0; i < P->size; i++) {
        int v = P->vertices[i];
        // Include v if no pivot or v is not adjacent to pivot
        if (u == -1 || !graph->adjacency[u][v]) {
            set_add(P_without_Nu, v);
        }
    }

    // Copy candidates for processing (since we'll modify P during iteration)
    int *candidates = malloc(P_without_Nu->size * sizeof(int));
    int cand_size = P_without_Nu->size;
    memcpy(candidates, P_without_Nu->vertices, cand_size * sizeof(int));
    set_destroy(P_without_Nu);

    /* ========================================================================
     * RECURSIVE EXPLORATION: Process each candidate
     * ========================================================================*/
    
    for (int i = 0; i < cand_size; i++) {
        int v = candidates[i];

        /* ====================================================================
         * SET INTERSECTION: Create P ∩ N(v) and S ∩ N(v)
         * ====================================================================*/
        
        // P_intersect_Nv = P ∩ N(v)
        Set *P_intersect_Nv = set_create(graph->node_count);
        for (int j = 0; j < P->size; j++) {
            int w = P->vertices[j];
            if (graph->adjacency[v][w]) {
                set_add(P_intersect_Nv, w);
            }
        }
        
        // S_intersect_Nv = S ∩ N(v)
        Set *S_intersect_Nv = set_create(graph->node_count);
        for (int j = 0; j < S->size; j++) {
            int w = S->vertices[j];
            if (graph->adjacency[v][w]) {
                set_add(S_intersect_Nv, w);
            }
        }

        /* ====================================================================
         * RECURSIVE CALL: Extend clique with vertex v
         * ====================================================================*/
        
        set_add(C, v);
        find_maximal_cliques(graph, C, P_intersect_Nv, S_intersect_Nv, maximal_cliques, count);
        set_remove(C);

        /* ====================================================================
         * STATE UPDATE: Move v from P to S
         * ====================================================================*/
        
        // Remove v from P
        for (int k = 0; k < P->size; k++) {
            if (P->vertices[k] == v) {
                P->vertices[k] = P->vertices[P->size - 1];
                P->size--;
                break;
            }
        }
        
        // Add v to S
        set_add(S, v);

        // Cleanup temporary sets
        set_destroy(P_intersect_Nv);
        set_destroy(S_intersect_Nv);
    }

    free(candidates);
}

/**
 * @brief Finds one maximum clique (largest among maximal cliques)
 * 
 * This function finds the maximum clique by first computing all maximal cliques
 * using the Bron-Kerbosch algorithm, then selecting the largest one.
 * 
 * The maximum clique is the largest possible clique in the graph, while
 * maximal cliques are cliques that cannot be extended further.
 * 
 * @param graph Pointer to the graph structure
 * @return Pointer to Set containing vertices of maximum clique, or NULL if none found
 */
Set *find_maximum_clique(Graph *graph) {
    /* ========================================================================
     * INITIALIZATION: Set up algorithm data structures
     * ========================================================================*/
    
    Set **maximal_cliques = NULL;
    int count = 0;

    // Initialize the three fundamental sets for Bron-Kerbosch
    Set *C = set_create(graph->node_count);  // Current clique (initially empty)
    Set *P = set_create(graph->node_count);  // Candidates (initially all vertices)
    Set *S = set_create(graph->node_count);  // Excluded (initially empty)

    // Add all vertices to candidate set P
    for (int i = 0; i < graph->node_count; i++) {
        set_add(P, i);
    }

    /* ========================================================================
     * MAXIMAL CLIQUE COMPUTATION: Find all maximal cliques
     * ========================================================================*/
    
    find_maximal_cliques(graph, C, P, S, &maximal_cliques, &count);

    // Cleanup algorithm sets
    set_destroy(C);
    set_destroy(P);
    set_destroy(S);

    /* ========================================================================
     * MAXIMUM SELECTION: Find largest maximal clique
     * ========================================================================*/
    
    int max_size = 0;
    Set *max_clique = NULL;
    
    // Find the maximal clique with maximum size
    for (int i = 0; i < count; i++) {
        if (maximal_cliques[i]->size > max_size) {
            max_size = maximal_cliques[i]->size;
            max_clique = maximal_cliques[i];
        }
    }

    /* ========================================================================
     * CLEANUP: Free all cliques except the maximum one
     * ========================================================================*/
    
    // Free all cliques except the maximum one
    for (int i = 0; i < count; i++) {
        if (maximal_cliques[i] != max_clique) {
            set_destroy(maximal_cliques[i]);
        }
    }
    free(maximal_cliques);

    return max_clique; // Caller must free this using set_destroy()
}

/**
 * @brief Analyzes and prints comprehensive clique information
 * 
 * This convenience function performs complete clique analysis and provides
 * formatted output. It allows choosing between different algorithms and
 * focuses on non-trivial cliques (size ≥ 3).
 * 
 * @param graph Pointer to the graph structure
 * @param algorithm_choice 1 for backtracking (all cliques), 2 for Bron-Kerbosch (maximal)
 */
void analyze_cliques(Graph *graph, int algorithm_choice) {
    /* ========================================================================
     * INITIALIZATION: Set up analysis
     * ========================================================================*/
    
    Set *C = set_create(graph->node_count);
    Set *P = set_create(graph->node_count);
    Set *S = set_create(graph->node_count);
    
    // Initialize candidate set with all vertices
    for (int i = 0; i < graph->node_count; i++) {
        set_add(P, i);
    }

    Set **cliques = NULL;
    int count = 0;
    int max_clique_size = 0;

    /* ========================================================================
     * CLIQUE COMPUTATION: Run chosen algorithm
     * ========================================================================*/
    
    if (algorithm_choice == 1) {
        find_all_cliques(graph, C, P, S, &cliques, &count);
    } else {
        find_maximal_cliques(graph, C, P, S, &cliques, &count);
    }

    // Find maximum clique size
    for (int i = 0; i < count; i++) {
        if (cliques[i]->size > max_clique_size) {
            max_clique_size = cliques[i]->size;
        }
    }

    /* ========================================================================
     * RESULTS PRESENTATION: Format and display findings
     * ========================================================================*/
    
    printf("\n=== Clique Analysis ===\n");
    printf("Algorithm used: %s\n", algorithm_choice == 1 ? "Backtracking (all cliques)" : "Bron-Kerbosch (maximal cliques)");
    printf("Maximum clique size: %d\n", max_clique_size);
    printf("Total cliques found: %d\n", count);
    printf("Non-trivial cliques (size ≥ 3):\n");
    
    // Display non-trivial cliques
    int non_trivial_count = 0;
    for (int i = 0; i < count; i++) {
        if (cliques[i]->size >= 3) {
            printf("  Clique %d (size %d): ", ++non_trivial_count, cliques[i]->size);
            for (int j = 0; j < cliques[i]->size; j++) {
                printf("%d ", cliques[i]->vertices[j]);
            }
            printf("\n");
        }
    }
    
    if (non_trivial_count == 0) {
        printf("  No non-trivial cliques found (all cliques have size < 3)\n");
    }

    /* ========================================================================
     * CLEANUP: Free all allocated memory
     * ========================================================================*/
    
    for (int i = 0; i < count; i++) {
        set_destroy(cliques[i]);
    }
    free(cliques);

    set_destroy(C);
    set_destroy(P);
    set_destroy(S);
}
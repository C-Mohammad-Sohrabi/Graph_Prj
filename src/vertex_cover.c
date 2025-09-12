#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "vertex_cover.h"
#include "independent_set.h" /* for create_complement_graph / find_maximum_independent_set */
#include "clique.h"
#include "set_utils.h"

/* ---------- small helpers for Set operations ---------- */

static bool set_contains(Set *s, int v) {
    for (int i = 0; i < s->size; i++) if (s->vertices[i] == v) return true;
    return false;
}

/* ---------- Exact via MIS (complement -> max clique) ---------- */

Set *vertex_cover_exact_via_mis(Graph *graph) {
    if (!graph) return NULL;
    if (graph->is_directed) return NULL; /* not supported */

    /* Create complement and find maximum clique on complement (== MIS in original) */
    Graph *comp = create_complement_graph(graph);
    if (!comp) return NULL;

    Set *max_clique = find_maximum_clique(comp);
    /* free complement adjacency now */
    for (int i = 0; i < comp->node_count; i++) free(comp->adjacency[i]);
    free(comp->adjacency);
    free(comp);

    if (!max_clique) return NULL;

    /* Vertex cover = V \ MIS */
    Set *vc = set_create(graph->node_count);
    for (int v = 0; v < graph->node_count; v++) {
        if (!set_contains(max_clique, v)) set_add(vc, v);
    }
    set_destroy(max_clique);
    return vc;
}

/* ---------- Approximation via maximal matching (2-approx) ---------- */

Set *vertex_cover_approx(Graph *graph) {
    if (!graph) return NULL;
    if (graph->is_directed) return NULL;

    int n = graph->node_count;
    char *matched = calloc(n, sizeof(char)); /* marks if vertex already matched/covered */
    Set *vc = set_create(n);

    for (int u = 0; u < n; u++) {
        if (matched[u]) continue;
        for (int v = 0; v < n; v++) {
            if (graph->adjacency[u][v] && !matched[v]) {
                /* take edge (u,v) into matching => add both endpoints to cover */
                if (!matched[u]) { set_add(vc, u); matched[u] = 1; }
                if (!matched[v]) { set_add(vc, v); matched[v] = 1; }
                break;
            }
        }
    }

    free(matched);
    return vc;
}

/* ---------- Bipartite: Hopcroft-Karp + Konig theorem ---------- */

/* Check bipartite and produce left/right mask arrays (malloc'd char arrays).
 * left_mask[i] == 1 if node i is in left set, right_mask[i] == 1 if in right set.
 * Caller must free the returned masks. */
bool is_bipartite_partition(Graph *graph, char **left_mask_out, char **right_mask_out) {
    if (!graph || graph->is_directed) return false;
    int n = graph->node_count;
    char *color = malloc(n); /* 0 = uncolored, 1 = left, 2 = right */
    memset(color, 0, n);

    /* BFS over components */
    int *queue = malloc(n * sizeof(int));
    for (int s = 0; s < n; s++) {
        if (color[s] != 0) continue;
        /* if isolated node, color as left by default */
        color[s] = 1;
        int head = 0, tail = 0;
        queue[tail++] = s;
        while (head < tail) {
            int u = queue[head++];
            for (int v = 0; v < n; v++) {
                if (!graph->adjacency[u][v]) continue;
                if (color[v] == 0) {
                    color[v] = (color[u] == 1) ? 2 : 1;
                    queue[tail++] = v;
                } else if (color[v] == color[u]) {
                    free(color);
                    free(queue);
                    return false; /* not bipartite */
                }
            }
        }
    }

    free(queue);
    /* produce masks */
    char *left = calloc(n, 1);
    char *right = calloc(n, 1);
    for (int i = 0; i < n; i++) {
        if (color[i] == 1) left[i] = 1;
        else if (color[i] == 2) right[i] = 1;
        else left[i] = 1; /* fallback */
    }
    free(color);
    *left_mask_out = left;
    *right_mask_out = right;
    return true;
}

/* Hopcroft-Karp implementation adapted for adjacency matrix + arbitrary index sets.
 * left_nodes: array of original indices for left side; left_n = number of left nodes.
 * right_nodes: array of original indices for right side; right_n likewise.
 *
 * Returns matching arrays pairU (size left_n) and pairV (size right_n), both malloc'd and
 * using -1 for unmatched. Caller must free them.
 */

static int INF_DIST = INT_MAX;

static void build_left_right_lists_from_masks(Graph *graph, char *left_mask,
                                              int **left_nodes_out, int *left_n_out,
                                              int **right_nodes_out, int *right_n_out) {
    int n = graph->node_count;
    int left_count = 0, right_count = 0;
    for (int i = 0; i < n; i++) {
        if (left_mask[i]) left_count++;
        else right_count++;
    }
    int *L = malloc(left_count * sizeof(int));
    int *R = malloc(right_count * sizeof(int));
    int li = 0, ri = 0;
    for (int i = 0; i < n; i++) {
        if (left_mask[i]) L[li++] = i;
        else R[ri++] = i;
    }
    *left_nodes_out = L;
    *left_n_out = left_count;
    *right_nodes_out = R;
    *right_n_out = right_count;
}

/* Hopcroft-Karp core */
static int hopcroft_karp(Graph *graph, int *left_nodes, int left_n, int *right_nodes, int right_n,
                         int **pairU_out, int **pairV_out) {
    if (left_n == 0 || right_n == 0) {
        *pairU_out = NULL; *pairV_out = NULL; return 0;
    }

    /* build adjacency list: for each left index i in [0..left_n-1], neighbors are indices j in [0..right_n-1]
       such that graph->adjacency[left_nodes[i]][ right_nodes[j] ] == 1 */
    int **adjL = malloc(left_n * sizeof(int *));
    int *adj_sizes = calloc(left_n, sizeof(int));
    for (int i = 0; i < left_n; i++) {
        /* count neighbors */
        int u = left_nodes[i];
        int cnt = 0;
        for (int j = 0; j < right_n; j++) {
            int v = right_nodes[j];
            if (graph->adjacency[u][v]) cnt++;
        }
        adjL[i] = malloc(cnt * sizeof(int));
        int k = 0;
        for (int j = 0; j < right_n; j++) {
            int v = right_nodes[j];
            if (graph->adjacency[u][v]) adjL[i][k++] = j; /* store neighbor as index in right_nodes */
        }
        adj_sizes[i] = cnt;
    }

    int *pairU = malloc(left_n * sizeof(int));
    int *pairV = malloc(right_n * sizeof(int));
    int *dist = malloc(left_n * sizeof(int));

    for (int i = 0; i < left_n; i++) pairU[i] = -1;
    for (int j = 0; j < right_n; j++) pairV[j] = -1;

    int matching = 0;

    /* BFS in HK to build layers */
    int bfs_queue_capacity = left_n;
    int *q = malloc(bfs_queue_capacity * sizeof(int));

    while (1) {
        /* BFS */
        int qh = 0, qt = 0;
        for (int i = 0; i < left_n; i++) {
            if (pairU[i] == -1) { dist[i] = 0; q[qt++] = i; }
            else dist[i] = INF_DIST;
        }
        int found_augmenting = 0;
        while (qh < qt) {
            int u = q[qh++];
            for (int k = 0; k < adj_sizes[u]; k++) {
                int v = adjL[u][k];
                int pu = pairV[v];
                if (pu == -1) {
                    found_augmenting = 1; /* shortest augmenting path exists via this free vertex */
                } else {
                    if (dist[pu] == INF_DIST) {
                        dist[pu] = dist[u] + 1;
                        q[qt++] = pu;
                    }
                }
            }
        }
        if (!found_augmenting) break;

        /* DFS (recursive-style using stack) to find augmenting paths from free U vertices */
        /* We'll implement a simple recursive DFS using helper function closure-like via static local functions is messy;
           so implement iterative augmentation using stack arrays and recursion simulation. For clarity and small graphs,
           we can implement a recursive function using function pointer; but to keep it simple, implement recursive function. */

        /* define recursive lambda via inner function using static helper - but C doesn't have nested functions portably.
           Instead implement iterative DFS with stack storing (u, next_neighbor_index). */

        int *next = calloc(left_n, sizeof(int));
        /* stack of left nodes */
        int *stack = malloc((left_n + 5) * sizeof(int));
        int stack_sz;

        for (int start = 0; start < left_n; start++) {
            if (pairU[start] != -1) continue;
            /* iterative DFS from `start` */
            stack_sz = 0;
            stack[stack_sz++] = start;
            while (stack_sz > 0) {
                int u = stack[stack_sz - 1];
                if (next[u] < adj_sizes[u]) {
                    int v = adjL[u][ next[u]++ ];
                    int pu = pairV[v];
                    if (pu == -1 || (dist[pu] == dist[u] + 1 && pairU[pu] != -1)) {
                        /* try to augment */
                        /* recursively attempt to find augmenting path from pu if exists */
                        if (pu != -1) {
                            stack[stack_sz++] = pu;
                        } else {
                            /* found path: flip along stack */
                            /* stack contains sequence of left indices representing alternating path */
                            /* Walk back and flip pairs along alternating edges */
                            int cur_v = v;
                            for (int idx = stack_sz - 1; idx >= 0; idx--) {
                                int cur_u = stack[idx];
                                /* find neighbor in adjL[cur_u] equal to cur_v */
                                /* set pairU[cur_u] = cur_v and pairV[cur_v] = cur_u */
                                pairU[cur_u] = cur_v;
                                pairV[cur_v] = cur_u;
                                /* if idx > 0, cur_v becomes previous matched partner in previous step: find previous v' for next flip */
                                if (idx > 0) {
                                    /* previous left node is stack[idx-1]; we need the v matched to it before (which is pairU[ stack[idx-1] ]) */
                                    cur_v = pairU[ stack[idx - 1] ];
                                }
                            }
                            matching++;
                            /* clear stack */
                            stack_sz = 0;
                        }
                    }
                } else {
                    /* backtrack */
                    stack_sz--;
                }
            }
        }

        free(stack);
        free(next);

        /* continue to next BFS round until no more augmenting paths */
    }

    /* cleanup adjacency lists */
    for (int i = 0; i < left_n; i++) free(adjL[i]);
    free(adjL);
    free(adj_sizes);
    free(dist);
    free(q);

    *pairU_out = pairU;
    *pairV_out = pairV;
    return matching;
}

/* After we have maximum matching (pairU, pairV) on left/right sets, derive min vertex cover:
   Standard procedure:
   - Start from unmatched left vertices and run alternating BFS/DFS following edges:
     * from left -> traverse non-matching edges to right
     * from right -> traverse matching edges to left
   - Let Z be set of visited vertices in this search.
   - Minimum vertex cover = (Left \ Z) union (Right ∩ Z).
*/
static Set *vertex_cover_from_matching(Graph *graph, int *left_nodes, int left_n, int *right_nodes, int right_n,
                                       int *pairU, int *pairV) {
    int n = graph->node_count;
    char *visitedL = calloc(left_n, 1);
    char *visitedR = calloc(right_n, 1);

    /* build adjacency for quick neighbor iteration */
    int **adjL = malloc(left_n * sizeof(int *));
    int *adjL_sz = calloc(left_n, sizeof(int));
    for (int i = 0; i < left_n; i++) {
        int u = left_nodes[i];
        int cnt = 0;
        for (int j = 0; j < right_n; j++) {
            int v = right_nodes[j];
            if (graph->adjacency[u][v]) cnt++;
        }
        adjL[i] = malloc((cnt > 0 ? cnt : 1) * sizeof(int));
        int k = 0;
        for (int j = 0; j < right_n; j++) {
            int v = right_nodes[j];
            if (graph->adjacency[u][v]) adjL[i][k++] = j;
        }
        adjL_sz[i] = k;
    }

    /* BFS queue of (isLeft, index) pairs — encode as signed int: positive = left idx, negative-1 = right idx */
    int qcap = left_n + right_n + 5;
    int *queue = malloc(qcap * sizeof(int));
    int qh = 0, qt = 0;

    /* enqueue all unmatched left vertices */
    for (int i = 0; i < left_n; i++) {
        if (pairU[i] == -1) {
            visitedL[i] = 1;
            queue[qt++] = i; /* encode left as non-negative */
        }
    }

    while (qh < qt) {
        int code = queue[qh++];
        if (code >= 0) {
            /* left index */
            int li = code;
            for (int k = 0; k < adjL_sz[li]; k++) {
                int rj = adjL[li][k];
                if (!visitedR[rj]) {
                    if (pairU[li] != rj) { /* traverse only via non-matching edges left->right */
                        visitedR[rj] = 1;
                        queue[qt++] = -1 - rj; /* encode right as negative */
                    }
                }
            }
        } else {
            /* right index */
            int rj = -1 - code;
            int paired_left = pairV[rj];
            if (paired_left != -1 && !visitedL[paired_left]) {
                visitedL[paired_left] = 1;
                queue[qt++] = paired_left;
            }
        }
    }

    /* Build vertex cover = (Left \ Z) union (Right ∩ Z) */
    Set *vc = set_create(n);
    for (int i = 0; i < left_n; i++) {
        int orig = left_nodes[i];
        if (!visitedL[i]) set_add(vc, orig);
    }
    for (int j = 0; j < right_n; j++) {
        int orig = right_nodes[j];
        if (visitedR[j]) set_add(vc, orig);
    }

    /* cleanup */
    for (int i = 0; i < left_n; i++) free(adjL[i]);
    free(adjL);
    free(adjL_sz);
    free(queue);
    free(visitedL);
    free(visitedR);

    return vc;
}

/* Wrapper that constructs left/right lists, finds matching, and gets vertex cover */
Set *vertex_cover_bipartite_konig(Graph *graph) {
    if (!graph) return NULL;
    if (graph->is_directed) return NULL;

    char *left_mask = NULL;
    char *right_mask = NULL;
    if (!is_bipartite_partition(graph, &left_mask, &right_mask)) {
        return NULL; /* not bipartite */
    }

    int *left_nodes = NULL, *right_nodes = NULL;
    int left_n = 0, right_n = 0;
    build_left_right_lists_from_masks(graph, left_mask, &left_nodes, &left_n, &right_nodes, &right_n);

    free(left_mask);
    free(right_mask);

    int *pairU = NULL, *pairV = NULL;
    int match_size = hopcroft_karp(graph, left_nodes, left_n, right_nodes, right_n, &pairU, &pairV);

    /* If no matching arrays (empty sides), handle trivial cases */
    if (left_n == 0 || right_n == 0) {
        /* trivial cover: all non-isolated vertices (or none) */
        Set *vc = set_create(graph->node_count);
        for (int i = 0; i < graph->node_count; i++) {
            int deg = 0;
            for (int j = 0; j < graph->node_count; j++) if (graph->adjacency[i][j]) deg++;
            if (deg > 0) set_add(vc, i);
        }
        if (pairU) free(pairU);
        if (pairV) free(pairV);
        free(left_nodes);
        free(right_nodes);
        return vc;
    }

    /* derive vertex cover using visited sets from alternating BFS */
    Set *vc = vertex_cover_from_matching(graph, left_nodes, left_n, right_nodes, right_n, pairU, pairV);

    free(pairU);
    free(pairV);
    free(left_nodes);
    free(right_nodes);
    return vc;
}

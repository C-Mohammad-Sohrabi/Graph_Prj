#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "structs.h"
#include "set_utils.h"
#include "clique.h"
#include "independent_set.h"
#include "connectivity.h"
#include "euler_path.h"
#include "havel_hakimi.h"
#include "line_graph.h"
#include "vertex_cover.h"
#include "connectivity_number.h"

int main() {
    /* Create build directory and clean up previous files */
    system("mkdir -p build/dot_files 2> /dev/null");
    system("mkdir -p build/images 2> /dev/null");
    
    const char *files[] = {
        "build/dot_files/graph.dot",
        "build/images/graph.png",
        "build/dot_files/line_graph.dot",
        "build/images/line_graph.png"
    };
    int num_files = sizeof(files) / sizeof(files[0]);
    for (int i = 0; i < num_files; i++) {
        if (access(files[i], F_OK) == 0) remove(files[i]);
    }

    int n;
    char graph_type[16];
    bool allow_bidirectional = false;
    int clique_algorithm_choice = 0;
    char line_graph_choice[8] = "no";
    char max_indep_choice[8] = "no";
    char euler_choice[8] = "no";
    char vertex_cover_choice[8] = "no";
    char connectivity_num_choice[8] = "no";

    printf("Is the graph directed or undirected? (Enter 'directed' or 'undirected'): ");
    if (scanf("%15s", graph_type) != 1) return 1;
    bool is_directed = strcmp(graph_type, "directed") == 0;

    if (!is_directed) {
        printf("\nFor clique analysis, choose an algorithm:\n1. Backtracking (all cliques)\n2. Branch & Bound (maximal cliques)\nEnter your choice (1 or 2): ");
        if (scanf("%d", &clique_algorithm_choice) != 1) return 1;
        printf("Generate line graph? (yes/no): ");
        scanf("%7s", line_graph_choice);
        printf("Find maximum independent set? (yes/no): ");
        scanf("%7s", max_indep_choice);
        printf("Find Euler path? (yes/no): ");
        scanf("%7s", euler_choice);
        printf("Find minimum vertex cover? (yes/no): ");
        scanf("%7s", vertex_cover_choice);
        printf("Calculate connectivity number (vertex connectivity)? (yes/no): ");
        scanf("%7s", connectivity_num_choice);
    } else {
        char bidir[8];
        printf("Allow bidirectional edges? (yes/no): ");
        scanf("%7s", bidir);
        allow_bidirectional = strcmp(bidir, "yes") == 0;
    }

    printf("Enter the number of nodes: ");
    if (scanf("%d", &n) != 1) return 1;

    Node *nodes = malloc(n * sizeof(Node));
    int *degrees = malloc(n * sizeof(int));
    int *in_degrees = is_directed ? malloc(n * sizeof(int)) : NULL;

    if (is_directed) {
        printf("Enter the out-degree sequence separated by spaces:\n");
        for (int i = 0; i < n; i++) scanf("%d", &degrees[i]);
        printf("Enter the in-degree sequence separated by spaces:\n");
        for (int i = 0; i < n; i++) scanf("%d", &in_degrees[i]);
        int out_sum = 0, in_sum = 0;
        for (int i = 0; i < n; i++) { out_sum += degrees[i]; in_sum += in_degrees[i]; }
        if (out_sum != in_sum) { printf("sum(out) != sum(in). Invalid.\n"); free(nodes); free(degrees); free(in_degrees); return 1; }
    } else {
        printf("Enter the degree sequence separated by spaces:\n");
        for (int i = 0; i < n; i++) scanf("%d", &degrees[i]);
        int sum = 0; for (int i = 0; i < n; i++) sum += degrees[i];
        if (sum % 2 != 0) { printf("Sum of degrees must be even.\n"); free(nodes); free(degrees); return 1; }
    }

    for (int i = 0; i < n; i++) {
        nodes[i].original_index = i;
        nodes[i].degree = degrees[i];
        nodes[i].in_degree = is_directed ? in_degrees[i] : 0;
    }

    FILE *dot_file = fopen("build/dot_files/graph.dot", "w");
    if (!dot_file) { 
        perror("open dot file"); 
        free(nodes); 
        free(degrees); 
        if (is_directed) free(in_degrees); 
        return 1; 
    }

    Graph graph;
    bool result;
    if (is_directed) result = havel_hakimi_directed(nodes, n, dot_file, &graph, allow_bidirectional);
    else result = havel_hakimi_undirected(nodes, n, dot_file, &graph);
    fclose(dot_file);

    if (!result) {
        printf("Not a valid graphical sequence.\n");
        free(nodes); free(degrees); if (is_directed) free(in_degrees);
        return 0;
    }

    printf("Graph generated: build/dot_files/graph.dot\n");
    system("dot -Tpng build/dot_files/graph.dot -o build/images/graph.png 2> /dev/null");
    printf("Preview: build/images/graph.png\n");

    Connectivity conn = check_connectivity(&graph);
    if (graph.is_directed) {
        printf("Connectivity: Strong=%s Weak=%s One-sided=%s\n",
               conn.is_strong ? "Yes":"No", conn.is_weak ? "Yes":"No", conn.is_one_sided ? "Yes":"No");
    } else {
        printf("Connectivity: %s\n", conn.is_connected ? "Connected" : "Disconnected");
        
        if (clique_algorithm_choice == 1 || clique_algorithm_choice == 2) {
            analyze_cliques(&graph, clique_algorithm_choice);
        }

        if (strcmp(line_graph_choice, "yes") == 0) {
            generate_line_graph(&graph);
            system("dot -Tpng build/dot_files/line_graph.dot -o build/images/line_graph.png 2> /dev/null");
            printf("Line graph: build/dot_files/line_graph.dot and build/images/line_graph.png\n");
        }

        if (strcmp(euler_choice, "yes") == 0) {
            find_euler_path(&graph);
        }

        if (strcmp(max_indep_choice, "yes") == 0) {
            Set *mis = find_maximum_independent_set(&graph);
            if (mis) {
                printf("\nMaximum Independent Set (vertices): ");
                for (int i = 0; i < mis->size; i++) printf("%d ", mis->vertices[i]);
                printf("\nSize: %d\n", mis->size);
                set_destroy(mis);
            } else {
                printf("No independent set found or unsupported graph type.\n");
            }
        }

        if (strcmp(vertex_cover_choice, "yes") == 0) {
            printf("\nChoose vertex cover algorithm:\n");
            printf("1. Exact via MIS (complement + max clique) [exponential]\n");
            printf("2. Konig (bipartite) via maximum matching [polynomial but only bipartite]\n");
            printf("3. 2-approx via maximal matching [fast approximate]\n");
            printf("Enter your choice (1/2/3): ");
            int vc_choice = 3;
            if (scanf("%d", &vc_choice) != 1) vc_choice = 3;

            Set *vc = NULL;
            if (vc_choice == 1) vc = vertex_cover_exact_via_mis(&graph);
            else if (vc_choice == 2) vc = vertex_cover_bipartite_konig(&graph);
            else vc = vertex_cover_approx(&graph);

            if (vc) {
                printf("\nVertex Cover (size=%d): ", vc->size);
                for (int i = 0; i < vc->size; i++) printf("%d ", vc->vertices[i]);
                printf("\n");
                set_destroy(vc);
            } else {
                printf("Could not compute vertex cover with the selected method (maybe graph not bipartite or error).\n");
            }
        }

        if (strcmp(connectivity_num_choice, "yes") == 0) {
            analyze_connectivity_number(&graph);
        }
    }

    for (int i = 0; i < graph.node_count; i++) free(graph.adjacency[i]);
    free(graph.adjacency);

    free(nodes);
    free(degrees);
    if (is_directed) free(in_degrees);
    return 0;
}
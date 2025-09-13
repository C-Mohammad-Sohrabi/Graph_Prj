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

/**
 * @file main.c
 * @brief Main program for comprehensive graph theory analysis
 * @author Graph Theory Project Team
 * @date 2024
 *
 * This is the main driver program that provides an interactive interface
 * for comprehensive graph theory analysis. The program allows users to:
 *
 * 1. Create graphs from degree sequences using Havel-Hakimi algorithm
 * 2. Analyze various graph properties and structures
 * 3. Generate visualizations in DOT and PNG formats
 * 4. Choose from multiple algorithms for different problems
 *
 * Supported analyses:
 * - Graph connectivity (strong, weak, one-sided for directed; connected for undirected)
 * - Clique detection (backtracking or Bron-Kerbosch algorithms)
 * - Maximum independent set computation
 * - Minimum vertex cover (exact, bipartite, or approximation algorithms)
 * - Eulerian path/cycle detection
 * - Line graph generation
 * - Vertex connectivity number calculation
 *
 * The program creates necessary directory structure and manages all file I/O
 * and memory management automatically.
 */

/**
 * @brief Main function - Interactive graph theory analysis program
 *
 * Provides a comprehensive interactive interface for graph theory analysis.
 * The program flow:
 *
 * 1. Setup Phase:
 *    - Creates build directories for output files
 *    - Cleans up any existing output files
 *    - Prompts user for graph type and analysis preferences
 *
 * 2. Input Phase:
 *    - Accepts degree sequences from user
 *    - Validates sequences (sum must be even for undirected, sum(out) = sum(in) for directed)
 *    - Creates Node structures for Havel-Hakimi algorithm
 *
 * 3. Graph Construction:
 *    - Uses Havel-Hakimi algorithm to construct graph from degree sequence
 *    - Generates DOT file for visualization
 *    - Creates PNG image using Graphviz (if available)
 *
 * 4. Analysis Phase:
 *    - Performs requested analyses based on user choices
 *    - For undirected graphs: cliques, independent sets, vertex covers, etc.
 *    - For directed graphs: connectivity analysis
 *
 * 5. Cleanup:
 *    - Frees all allocated memory
 *    - Closes files and releases resources
 *
 * @return 0 on successful completion, 1 on error
 *
 * @complexity Varies by chosen algorithms, from O(V+E) to O(3^(n/3))
 *
 * @note Creates build/dot_files/ and build/images/ directories
 * @note Requires Graphviz for PNG generation (gracefully handles absence)
 * @note All user input is validated before processing
 * @note Memory management is handled automatically
 */
int main()
{
    /* ========================================================================
     * SETUP PHASE: Directory creation and file cleanup
     * ========================================================================*/

    // Create necessary directory structure for output files
    system("mkdir -p build/dot_files 2> /dev/null");
    system("mkdir -p build/images 2> /dev/null");

    // Clean up any existing output files from previous runs
    const char *files[] = {
        "build/dot_files/graph.dot",      // Main graph DOT file
        "build/images/graph.png",         // Main graph PNG image
        "build/dot_files/line_graph.dot", // Line graph DOT file
        "build/images/line_graph.png"     // Line graph PNG image
    };
    int num_files = sizeof(files) / sizeof(files[0]);
    for (int i = 0; i < num_files; i++)
    {
        if (access(files[i], F_OK) == 0)
            remove(files[i]);
    }

    /* ========================================================================
     * VARIABLE DECLARATIONS: User preferences and graph data
     * ========================================================================*/

    int n;                                  // Number of vertices
    char graph_type[16];                    // "directed" or "undirected"
    bool allow_bidirectional = false;       // For directed graphs
    int clique_algorithm_choice = 0;        // 1=backtracking, 2=branch&bound
    char line_graph_choice[8] = "no";       // Generate line graph?
    char max_indep_choice[8] = "no";        // Find maximum independent set?
    char euler_choice[8] = "no";            // Find Euler path?
    char vertex_cover_choice[8] = "no";     // Find minimum vertex cover?
    char connectivity_num_choice[8] = "no"; // Calculate connectivity number?

    /* ========================================================================
     * USER INPUT PHASE: Graph type and analysis preferences
     * ========================================================================*/

    // Determine graph type (directed vs undirected)
    printf("Is the graph directed or undirected? (Enter 'directed' or 'undirected'): ");
    if (scanf("%15s", graph_type) != 1)
        return 1;
    bool is_directed = strcmp(graph_type, "directed") == 0;

    // For undirected graphs: collect analysis preferences
    if (!is_directed)
    {
        // Choose clique detection algorithm
        printf("\nFor clique analysis, choose an algorithm:\n");
        printf("1. Backtracking (all cliques)\n");
        printf("2. Branch & Bound (maximal cliques)\n");
        printf("Enter your choice (1 or 2): ");
        if (scanf("%d", &clique_algorithm_choice) != 1)
            return 1;

        // Collect preferences for other analyses
        printf("\nGenerate line graph? (yes/no): ");
        scanf("%7s", line_graph_choice);

        printf("\nFind maximum independent set? (yes/no): ");
        scanf("%7s", max_indep_choice);

        printf("\nFind Euler path? (yes/no): ");
        scanf("%7s", euler_choice);

        printf("\nFind minimum vertex cover? (yes/no): ");
        scanf("%7s", vertex_cover_choice);

        printf("\nCalculate connectivity number (vertex connectivity)? (yes/no): ");
        scanf("%7s", connectivity_num_choice);
    }
    else
    {
        // For directed graphs: ask about bidirectional edges
        char bidir[8];
        printf("\nAllow bidirectional edges? (yes/no): ");
        scanf("%7s", bidir);
        allow_bidirectional = strcmp(bidir, "yes") == 0;
    }

    /* ========================================================================
     * DEGREE SEQUENCE INPUT: Collect and validate vertex degrees
     * ========================================================================*/

    printf("\nEnter the number of nodes: ");
    if (scanf("%d", &n) != 1)
        return 1;

    // Allocate memory for node structures and degree arrays
    Node *nodes = malloc(n * sizeof(Node));
    int *degrees = malloc(n * sizeof(int));
    int *in_degrees = is_directed ? malloc(n * sizeof(int)) : NULL;

    if (is_directed)
    {
        // For directed graphs: collect both out-degrees and in-degrees
        printf("\nEnter the out-degree sequence separated by spaces:\n");
        for (int i = 0; i < n; i++)
            scanf("%d", &degrees[i]);

        printf("\nEnter the in-degree sequence separated by spaces:\n");
        for (int i = 0; i < n; i++)
            scanf("%d", &in_degrees[i]);

        // Validate: sum of out-degrees must equal sum of in-degrees
        int out_sum = 0, in_sum = 0;
        for (int i = 0; i < n; i++)
        {
            out_sum += degrees[i];
            in_sum += in_degrees[i];
        }
        if (out_sum != in_sum)
        {
            printf("Error: sum(out-degrees) != sum(in-degrees). Invalid sequence.\n");
            free(nodes);
            free(degrees);
            free(in_degrees);
            return 1;
        }
    }
    else
    {
        // For undirected graphs: collect degree sequence
        printf("Enter the degree sequence separated by spaces:\n");
        for (int i = 0; i < n; i++)
            scanf("%d", &degrees[i]);

        // Validate: sum of degrees must be even (handshaking lemma)
        int sum = 0;
        for (int i = 0; i < n; i++)
            sum += degrees[i];
        if (sum % 2 != 0)
        {
            printf("Error: Sum of degrees must be even (handshaking lemma).\n");
            free(nodes);
            free(degrees);
            return 1;
        }
    }

    /* ========================================================================
     * NODE INITIALIZATION: Prepare data for Havel-Hakimi algorithm
     * ========================================================================*/

    // Initialize Node structures with degree information and original indices
    for (int i = 0; i < n; i++)
    {
        nodes[i].original_index = i;                          // Preserve original vertex numbering
        nodes[i].degree = degrees[i];                         // Out-degree for directed, degree for undirected
        nodes[i].in_degree = is_directed ? in_degrees[i] : 0; // In-degree only for directed
    }

    /* ========================================================================
     * GRAPH CONSTRUCTION: Havel-Hakimi algorithm execution
     * ========================================================================*/

    // Open DOT file for graph visualization output
    FILE *dot_file = fopen("build/dot_files/graph.dot", "w");
    if (!dot_file)
    {
        perror("Error: Cannot create DOT file");
        free(nodes);
        free(degrees);
        if (is_directed)
            free(in_degrees);
        return 1;
    }

    // Execute appropriate Havel-Hakimi algorithm based on graph type
    Graph graph;
    bool result;
    if (is_directed)
        result = havel_hakimi_directed(nodes, n, dot_file, &graph, allow_bidirectional);
    else
        result = havel_hakimi_undirected(nodes, n, dot_file, &graph);
    fclose(dot_file);

    // Check if degree sequence was graphical (realizable)
    if (!result)
    {
        printf("Error: Not a valid graphical sequence.\n");
        free(nodes);
        free(degrees);
        if (is_directed)
            free(in_degrees);
        return 0;
    }

    /* ========================================================================
     * VISUALIZATION GENERATION: Create DOT and PNG files
     * ========================================================================*/

    printf("Graph generated successfully!\n");
    printf("DOT file: build/dot_files/graph.dot\n");

    // Generate PNG image using Graphviz (if available)
    system("dot -Tpng build/dot_files/graph.dot -o build/images/graph.png 2> /dev/null");
    printf("PNG visualization: build/images/graph.png\n");

    /* ========================================================================
     * CONNECTIVITY ANALYSIS: Basic graph connectivity properties
     * ========================================================================*/

    // Analyze connectivity properties for both directed and undirected graphs
    Connectivity conn = check_connectivity(&graph);

    if (graph.is_directed)
    {
        // For directed graphs: report strong, weak, and one-sided connectivity
        printf("\n=== Connectivity Analysis ===\n");
        printf("Strong connectivity: %s\n", conn.is_strong ? "Yes" : "No");
        printf("Weak connectivity: %s\n", conn.is_weak ? "Yes" : "No");
        printf("One-sided connectivity: %s\n", conn.is_one_sided ? "Yes" : "No");
    }
    else
    {
        // For undirected graphs: report basic connectivity
        printf("\n=== Connectivity Analysis ===\n");
        printf("Graph connectivity: %s\n", conn.is_connected ? "Connected" : "Disconnected");

        /* ====================================================================
         * UNDIRECTED GRAPH ANALYSES: Various graph theory algorithms
         * ====================================================================*/

        // Clique Analysis: Find cliques using chosen algorithm
        if (clique_algorithm_choice == 1 || clique_algorithm_choice == 2)
        {
            analyze_cliques(&graph, clique_algorithm_choice);
        }

        // Line Graph Generation: Create line graph if requested
        if (strcmp(line_graph_choice, "yes") == 0)
        {
            generate_line_graph(&graph);
            // Generate PNG for line graph
            system("dot -Tpng build/dot_files/line_graph.dot -o build/images/line_graph.png 2> /dev/null");
            printf("Line graph files: build/dot_files/line_graph.dot and build/images/line_graph.png\n");
        }

        // Eulerian Path Analysis: Find Euler paths/cycles if requested
        if (strcmp(euler_choice, "yes") == 0)
        {
            find_euler_path(&graph);
        }

        // Maximum Independent Set: Find largest independent set
        if (strcmp(max_indep_choice, "yes") == 0)
        {
            Set *mis = find_maximum_independent_set(&graph);
            if (mis)
            {
                printf("\n=== Maximum Independent Set Analysis ===\n");
                printf("Maximum Independent Set vertices: ");
                for (int i = 0; i < mis->size; i++)
                    printf("%d ", mis->vertices[i]);
                printf("\nSize: %d\n", mis->size);
                set_destroy(mis);
            }
            else
            {
                printf("Error: Could not compute maximum independent set.\n");
            }
        }

        // Vertex Cover Analysis: Find minimum vertex cover using chosen method
        if (strcmp(vertex_cover_choice, "yes") == 0)
        {
            printf("\n=== Vertex Cover Analysis ===\n");
            printf("Choose vertex cover algorithm:\n");
            printf("1. Exact via Maximum Independent Set [optimal but exponential]\n");
            printf("2. König's theorem for bipartite graphs [polynomial, optimal for bipartite]\n");
            printf("3. 2-approximation via maximal matching [fast, approximate]\n");
            printf("Enter your choice (1/2/3): ");

            int vc_choice = 3; // Default to approximation
            if (scanf("%d", &vc_choice) != 1)
                vc_choice = 3;

            // Execute chosen vertex cover algorithm
            Set *vc = NULL;
            if (vc_choice == 1)
                vc = vertex_cover_exact_via_mis(&graph);
            else if (vc_choice == 2)
                vc = vertex_cover_bipartite_konig(&graph);
            else
                vc = vertex_cover_approx(&graph);

            // Display results
            if (vc)
            {
                printf("Minimum Vertex Cover (size=%d): ", vc->size);
                for (int i = 0; i < vc->size; i++)
                    printf("%d ", vc->vertices[i]);
                printf("\n");
                set_destroy(vc);
            }
            else
            {
                printf("Could not compute vertex cover with selected method.\n");
                printf("(Graph may not be bipartite for König's method)\n");
            }
        }

        // Connectivity Number Analysis: Calculate vertex connectivity
        if (strcmp(connectivity_num_choice, "yes") == 0)
        {
            analyze_connectivity_number(&graph);
        }
    }

    /* ========================================================================
     * CLEANUP PHASE: Free all allocated memory and resources
     * ========================================================================*/

    // Free adjacency matrix
    for (int i = 0; i < graph.node_count; i++)
    {
        free(graph.adjacency[i]);
    }
    free(graph.adjacency);

    // Free input data structures
    free(nodes);
    free(degrees);
    if (is_directed)
        free(in_degrees);

    printf("\n=== Analysis Complete ===\n");
    printf("All output files are saved in the build/ directory.\n");

    return 0;
}
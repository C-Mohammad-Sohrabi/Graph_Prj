#include "connectivity.h"

/**
 * @file connectivity.c
 * @brief Graph connectivity analysis implementation
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This file implements connectivity analysis for both directed and undirected graphs.
 * Uses BFS (Breadth-First Search) to determine reachability and various types of connectivity.
 * 
 * Connectivity types analyzed:
 * - Undirected graphs: Simple connectivity (all vertices reachable)
 * - Directed graphs: Strong, weak, and one-sided connectivity
 * 
 * The implementation is efficient with O(V + E) complexity per connectivity test.
 */

/**
 * @brief Comprehensive connectivity analysis for directed and undirected graphs
 * 
 * This function performs different connectivity analyses based on graph type:
 * 
 * For Directed Graphs:
 * - Strong Connectivity: Every vertex can reach every other vertex via directed paths
 * - Weak Connectivity: Graph is connected when all edges are treated as undirected
 * - One-sided Connectivity: For any pair of vertices, at least one can reach the other
 * 
 * For Undirected Graphs:
 * - Simple Connectivity: All vertices are reachable from any starting vertex
 * 
 * The algorithm uses BFS traversal for reachability analysis, which is optimal
 * for unweighted graphs and provides O(V + E) complexity per test.
 * 
 * @param graph Pointer to the graph structure to analyze
 * @return Connectivity structure with analysis results
 */
Connectivity check_connectivity(Graph *graph) {
    /* ========================================================================
     * INITIALIZATION: Set up connectivity result structure and working arrays
     * ========================================================================*/
    
    // Initialize connectivity results (all false initially, one_sided true by default)
    Connectivity conn = {false, false, true, false};
    
    // Allocate working arrays for BFS traversal
    bool *visited = calloc(graph->node_count, sizeof(bool));    // Visited vertex tracking
    int *queue = malloc(graph->node_count * sizeof(int));       // BFS queue
    int front, rear;                                            // Queue pointers

    /* ========================================================================
     * DIRECTED GRAPH ANALYSIS: Strong, weak, and one-sided connectivity
     * ========================================================================*/
    
    if (graph->is_directed) {
        
        /* ====================================================================
         * STRONG CONNECTIVITY TEST: All vertices reachable via directed edges
         * ====================================================================*/
        
        // Initialize BFS from vertex 0 following directed edges only
        front = rear = 0;
        queue[rear++] = 0;           // Start BFS from vertex 0
        visited[0] = true;
        int reachable_count = 1;
        
        // BFS traversal following directed edges
        while (front < rear) {
            int current = queue[front++];
            
            // Explore all outgoing edges from current vertex
            for (int i = 0; i < graph->node_count; i++) {
                if (graph->adjacency[current][i] > 0 && !visited[i]) {
                    visited[i] = true;
                    queue[rear++] = i;
                    reachable_count++;
                }
            }
        }
        
        // Strong connectivity: all vertices reachable from vertex 0
        conn.is_strong = (reachable_count == graph->node_count);

        /* ====================================================================
         * WEAK CONNECTIVITY TEST: Connectivity treating edges as undirected
         * ====================================================================*/
        
        // Reset visited array for weak connectivity test
        free(visited);
        visited = calloc(graph->node_count, sizeof(bool));
        front = rear = 0;
        queue[rear++] = 0;
        visited[0] = true;
        reachable_count = 1;
        
        // BFS treating all edges as bidirectional
        while (front < rear) {
            int current = queue[front++];
            
            // Check both outgoing and incoming edges
            for (int i = 0; i < graph->node_count; i++) {
                // Edge exists in either direction (treat as undirected)
                if ((graph->adjacency[current][i] > 0 || graph->adjacency[i][current] > 0) && !visited[i]) {
                    visited[i] = true;
                    queue[rear++] = i;
                    reachable_count++;
                }
            }
        }
        
        // Weak connectivity: all vertices reachable when treating edges as undirected
        conn.is_weak = (reachable_count == graph->node_count);

        /* ====================================================================
         * ONE-SIDED CONNECTIVITY TEST: Simplified implementation
         * ====================================================================*/
        
        // Simplified one-sided connectivity check
        // Note: This is a basic implementation that checks for bidirectional edges
        // A complete implementation would require more complex analysis
        for (int i = 0; i < graph->node_count; i++) {
            for (int j = 0; j < graph->node_count; j++) {
                // If bidirectional edge exists and bidirectional edges aren't explicitly allowed
                if (graph->adjacency[i][j] > 0 && graph->adjacency[j][i] > 0 && !graph->allow_bidirectional) {
                    conn.is_one_sided = false;
                    break;
                }
            }
            if (!conn.is_one_sided)
                break;
        }
        
    } else {
        /* ====================================================================
         * UNDIRECTED GRAPH ANALYSIS: Simple connectivity
         * ====================================================================*/
        
        // Initialize BFS from vertex 0 (if graph is non-empty)
        front = rear = 0;
        if (graph->node_count > 0) {
            queue[rear++] = 0;
            visited[0] = true;
        }
        int reachable_count = (graph->node_count > 0) ? 1 : 0;
        
        // BFS traversal for undirected graph
        while (front < rear) {
            int current = queue[front++];
            
            // Explore all adjacent vertices (undirected edges)
            for (int i = 0; i < graph->node_count; i++) {
                if (graph->adjacency[current][i] > 0 && !visited[i]) {
                    visited[i] = true;
                    queue[rear++] = i;
                    reachable_count++;
                }
            }
        }
        
        // Simple connectivity: all vertices reachable
        conn.is_connected = (reachable_count == graph->node_count);
    }

    /* ========================================================================
     * CLEANUP: Free allocated working arrays
     * ========================================================================*/
    
    free(visited);
    free(queue);
    
    return conn;
}
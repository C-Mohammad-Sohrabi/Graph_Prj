/**
 * @file line_graph.h
 * @brief Line graph generation from original graphs
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This module implements line graph generation, where the line graph L(G)
 * of a graph G is constructed such that:
 * - Each edge of G becomes a vertex in L(G)
 * - Two vertices in L(G) are adjacent iff their corresponding edges in G share a common vertex
 * 
 * Line graphs are useful in various graph theory applications and provide
 * insights into the edge structure of the original graph.
 * 
 * The implementation process:
 * 1. Extract all edges from the original graph
 * 2. Create mapping from vertices to incident edges
 * 3. Connect edges in line graph if they share a common vertex
 * 4. Generate visualization (DOT format)
 * 
 * Time Complexity: O(E²) where E is the number of edges
 * Space Complexity: O(E²) for storing adjacency information
 */

#ifndef LINE_GRAPH_H
#define LINE_GRAPH_H

#include "structs.h"

/**
 * @brief Extracts all edges from graph's adjacency matrix
 * 
 * Scans the adjacency matrix to identify all edges and creates an array
 * of Edge structures. For undirected graphs, only considers upper triangular
 * part to avoid duplicate edges. For directed graphs, considers all entries.
 * 
 * @param graph Pointer to the input graph
 * @param edges Pointer to Edge array pointer (allocated by function)
 * @return Number of edges extracted
 * 
 * @complexity O(V²) where V is the number of vertices
 * 
 * @pre graph must have valid adjacency matrix
 * @pre edges must be a valid Edge** pointer
 * @post *edges points to newly allocated array of Edge structures
 * @post Caller must free the edges array
 * @post Returns 0 if no edges found or allocation fails
 * 
 * @note For undirected graphs: edge (u,v) stored once with u < v
 * @note For directed graphs: each directed edge stored separately
 */
int extract_edges_from_adjacency(Graph *graph, Edge **edges);

/**
 * @brief Creates mapping from vertices to their incident edges
 * 
 * Builds a data structure that maps each vertex to the list of edges
 * incident to it. This is essential for line graph construction as
 * we need to know which edges share common vertices.
 * 
 * @param edges Array of Edge structures
 * @param edge_count Number of edges in the array
 * @param node_count Number of vertices in original graph
 * @param node_edges Pointer to array of int* arrays (allocated by function)
 * @param node_edges_count Pointer to array of counts (allocated by function)
 * 
 * @complexity O(E) where E is the number of edges
 * 
 * @pre edges must be valid array of size edge_count
 * @pre node_edges and node_edges_count must be valid pointers
 * @post *node_edges contains for each vertex an array of incident edge indices
 * @post *node_edges_count contains for each vertex the number of incident edges
 * @post Caller must free all allocated arrays
 * 
 * @note Each edge appears in the incident lists of both its endpoints
 * @note Edge indices correspond to positions in the original edges array
 */
void build_node_edges_mapping(Edge *edges, int edge_count, int node_count, 
                             int ***node_edges, int **node_edges_count);

/**
 * @brief Constructs the line graph adjacency structure
 * 
 * Creates the line graph where each edge from the original graph becomes
 * a vertex, and two line graph vertices are adjacent iff their corresponding
 * original edges share a common vertex.
 * 
 * For each vertex in the original graph:
 * - Take all pairs of edges incident to that vertex
 * - Connect the corresponding vertices in the line graph
 * 
 * @param edges Array of original graph edges
 * @param edge_count Number of edges (vertices in line graph)
 * @param node_edges Mapping from vertices to incident edges
 * @param node_edges_count Array of incident edge counts
 * @param node_count Number of vertices in original graph
 * @param line_graph_adj Pointer to line graph adjacency list (allocated by function)
 * 
 * @complexity O(E²) in worst case, O(E × max_degree²) more precisely
 * 
 * @pre All input arrays must be properly initialized
 * @pre line_graph_adj must be a valid AdjList** pointer
 * @post *line_graph_adj contains adjacency lists for the line graph
 * @post Caller must free all adjacency list arrays
 * 
 * @note Two line graph vertices are adjacent iff original edges share a vertex
 * @note Line graph is always undirected, even if original graph is directed
 */
void build_line_graph(Edge *edges, int edge_count, int **node_edges, int *node_edges_count, 
                     int node_count, AdjList **line_graph_adj);

/**
 * @brief Generates DOT file for line graph visualization
 * 
 * Creates a DOT format file for visualizing the line graph. Each vertex
 * in the line graph is labeled with its corresponding edge from the original
 * graph (e.g., "E0 (2-5)" for edge between vertices 2 and 5).
 * 
 * @param edges Array of original graph edges
 * @param edge_count Number of edges (vertices in line graph)
 * @param line_graph_adj Adjacency lists of the line graph
 * @param filename Output filename for DOT file
 * 
 * @complexity O(E + line_graph_edges) where E is original edge count
 * 
 * @pre edges must be valid array of size edge_count
 * @pre line_graph_adj must contain valid adjacency lists
 * @pre filename must be a valid file path
 * @post Creates DOT file at specified location
 * @post Prints confirmation message
 * 
 * @note Each line graph vertex shows its corresponding original edge
 * @note Generated file can be processed with Graphviz for visualization
 */
void generate_line_graph_dot(Edge *edges, int edge_count, AdjList *line_graph_adj, 
                           const char *filename);

/**
 * @brief Main function to generate complete line graph
 * 
 * High-level function that orchestrates the complete line graph generation
 * process. It handles all steps from edge extraction to DOT file generation
 * and manages all intermediate data structures.
 * 
 * Process:
 * 1. Extract edges from original graph
 * 2. Build vertex-to-edges mapping
 * 3. Construct line graph adjacency structure
 * 4. Generate DOT file for visualization
 * 5. Clean up all allocated memory
 * 
 * @param graph Pointer to the original graph
 * 
 * @complexity O(E²) where E is the number of edges
 * 
 * @pre graph must be a valid graph with adjacency matrix
 * @post Creates line graph DOT file: "dot_file/line_graph.dot"
 * @post Prints status messages during generation
 * @post All intermediate memory is freed
 * 
 * @note Handles graphs with no edges gracefully (prints appropriate message)
 * @note Creates necessary directory structure for output files
 * @note Manages all memory allocation and cleanup internally
 */
void generate_line_graph(Graph *graph);

#endif
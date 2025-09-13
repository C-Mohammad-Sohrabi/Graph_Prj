/**
 * @file structs.h
 * @brief Core data structures for the Graph Theory Project
 * @author Graph Theory Project Team
 * @date 2024
 * 
 * This header file contains all the fundamental data structures used throughout
 * the graph theory algorithms implementation. It includes structures for graphs,
 * nodes, edges, sets, and connectivity analysis results.
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/**
 * @struct Node
 * @brief Represents a graph vertex with degree information
 * 
 * This structure is primarily used in the Havel-Hakimi algorithm for graph
 * construction from degree sequences. It maintains both the original vertex
 * index and its degree information for both directed and undirected graphs.
 */
typedef struct {
    int original_index;
    int degree;    // For undirected: degree; For directed: out-degree
    int in_degree; // Only used for directed graphs
} Node;

/**
 * @struct Graph
 * @brief Main graph representation using adjacency matrix
 * 
 * This structure represents a graph using an adjacency matrix approach.
 * It supports both directed and undirected graphs with optional bidirectional
 * edges for directed graphs. The adjacency matrix uses boolean values (0/1).
 */
typedef struct {
    int **adjacency; // Boolean adjacency matrix (0 or 1)
    int node_count;
    bool is_directed;
    bool allow_bidirectional; // For directed graphs: true if bidirectional edges are allowed
} Graph;

/**
 * @struct Edge
 * @brief Represents an edge between two vertices
 * 
 * Simple structure to represent a graph edge with its two endpoint vertices.
 * Used primarily in line graph generation and edge-based algorithms.
 */
typedef struct {
    int u;
    int v;
} Edge;

/**
 * @struct AdjList
 * @brief Adjacency list representation for specific algorithms
 * 
 * Dynamic adjacency list structure used in algorithms that require
 * efficient neighbor iteration, such as line graph generation.
 */
typedef struct {
    int *adjacent;
    int count;
    int capacity;
} AdjList;

/**
 * @struct Set
 * @brief Dynamic set data structure for vertex collections
 * 
 * Generic set implementation for storing collections of vertices.
 * Used extensively in clique detection, independent set algorithms,
 * and vertex cover computations.
 */
typedef struct {
    int *vertices;
    int size;
    int capacity;
} Set;

/**
 * @struct Connectivity
 * @brief Results of connectivity analysis
 * 
 * Structure containing the results of various connectivity tests
 * for both directed and undirected graphs.
 */
typedef struct {
    bool is_strong;
    bool is_weak;
    bool is_one_sided;
    bool is_connected;
} Connectivity;

#endif 
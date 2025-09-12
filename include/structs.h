#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    int original_index;
    int degree;    // For undirected: degree; For directed: out-degree
    int in_degree; // Only used for directed graphs
} Node;

typedef struct {
    int **adjacency; // Boolean adjacency matrix (0 or 1)
    int node_count;
    bool is_directed;
    bool allow_bidirectional; // For directed graphs: true if bidirectional edges are allowed
} Graph;

typedef struct {
    int u;
    int v;
} Edge;

typedef struct {
    int *adjacent;
    int count;
    int capacity;
} AdjList;

typedef struct {
    int *vertices;
    int size;
    int capacity;
} Set;

typedef struct {
    bool is_strong;
    bool is_weak;
    bool is_one_sided;
    bool is_connected;
} Connectivity;

#endif // STRUCTS_H
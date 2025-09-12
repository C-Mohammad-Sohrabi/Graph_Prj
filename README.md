# Advanced Graph Theory Project

A comprehensive C implementation of various graph theory algorithms and analysis tools. This project provides implementations for graph construction, connectivity analysis, clique detection, independent sets, vertex covers, Euler paths, line graphs, and connectivity numbers.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Algorithms Implemented](#algorithms-implemented)
- [Data Structures](#data-structures)
- [Examples](#examples)
- [Dependencies](#dependencies)
- [Contributing](#contributing)

## Overview

This project implements a wide range of graph theory algorithms in C, focusing on both theoretical correctness and practical efficiency. The program can handle both directed and undirected graphs, construct graphs from degree sequences, and perform various analyses including connectivity, clique detection, independent sets, and more.

## Features

- **Graph Construction**: Build graphs from degree sequences using the Havel-Hakimi algorithm
- **Connectivity Analysis**: Determine strong, weak, and one-sided connectivity for directed graphs
- **Clique Detection**: Find all cliques or maximal cliques using backtracking or branch-and-bound
- **Independent Sets**: Find maximum independent sets using complement graph techniques
- **Vertex Cover**: Multiple algorithms including exact, approximation, and bipartite-specific methods
- **Euler Paths**: Find Eulerian paths and cycles using Hierholzer's algorithm
- **Line Graphs**: Generate line graphs from original graphs
- **Connectivity Numbers**: Calculate vertex connectivity using exact and approximation methods
- **Visualization**: Generate DOT files and PNG images for graph visualization

## Project Structure

```
Graph_Project/
├── src/                    # Source files
│   ├── main.c             # Main program entry point
│   ├── havel_hakimi.c     # Graph construction from degree sequences
│   ├── connectivity.c     # Graph connectivity analysis
│   ├── clique.c           # Clique detection algorithms
│   ├── independent_set.c  # Maximum independent set algorithms
│   ├── vertex_cover.c     # Vertex cover algorithms
│   ├── euler_path.c       # Euler path/cycle detection
│   ├── line_graph.c       # Line graph generation
│   ├── connectivity_number.c # Vertex connectivity calculation
│   └── set_utils.c        # Set data structure utilities
├── include/               # Header files
│   ├── structs.h          # Core data structures
│   └── *.h               # Algorithm-specific headers
├── build/                 # Build artifacts
│   ├── obj/              # Object files
│   ├── dot_files/        # Generated DOT files
│   └── images/           # Generated PNG images
├── Makefile              # Build configuration
└── README.md            # This file
```

## Build Instructions

### Prerequisites

- GCC compiler with C11 support
- Make build system
- Graphviz (for DOT file visualization)

### Building the Project

```bash
# Build the project
make

# Build and run
make run

# Build with debug information
make debug

# Clean build artifacts
make clean

# Install system-wide (requires sudo)
make install
```

## Usage

Run the program and follow the interactive prompts:

```bash
./build/graph_program
```

The program will ask you to:

1. **Choose graph type**: Directed or undirected
2. **Enter degree sequences**: Provide the degree sequence for vertices
3. **Select algorithms**: Choose which analyses to perform
4. **View results**: Generated graphs and analysis results

### Example Session

```
Is the graph directed or undirected? (Enter 'directed' or 'undirected'): undirected

For clique analysis, choose an algorithm:
1. Backtracking (all cliques)
2. Branch & Bound (maximal cliques)
Enter your choice (1 or 2): 2

Generate line graph? (yes/no): yes
Find maximum independent set? (yes/no): yes
Find Euler path? (yes/no): yes
Find minimum vertex cover? (yes/no): yes
Calculate connectivity number (vertex connectivity)? (yes/no): yes

Enter the number of nodes: 4
Enter the degree sequence separated by spaces:
2 2 2 2
```

## Algorithms Implemented

### 1. Havel-Hakimi Algorithm

**Purpose**: Constructs a graph from a given degree sequence.

**Implementation**: 
- **Undirected**: Sorts vertices by degree (descending) and connects highest-degree vertex to next highest-degree vertices
- **Directed**: Handles both in-degree and out-degree sequences, with optional bidirectional edges

**Time Complexity**: O(n²) where n is the number of vertices

**Files**: `src/havel_hakimi.c`, `include/havel_hakimi.h`

**Algorithm Details**:
1. Sort vertices by degree in descending order
2. Take the vertex with highest degree
3. Connect it to the next d vertices (where d is its degree)
4. Reduce degrees of connected vertices
5. Repeat until all degrees are zero or sequence is invalid

### 2. Connectivity Analysis

**Purpose**: Determines various types of connectivity in graphs.

**Types Analyzed**:
- **Undirected**: Connected or disconnected
- **Directed**: Strong, weak, and one-sided connectivity

**Implementation**: Uses BFS traversal to check reachability

**Time Complexity**: O(V + E) where V is vertices and E is edges

**Files**: `src/connectivity.c`, `include/connectivity.h`

**Algorithm Details**:
- **Strong Connectivity**: All vertices reachable from any vertex following directed edges
- **Weak Connectivity**: All vertices reachable treating edges as undirected
- **One-sided Connectivity**: For any pair of vertices, at least one can reach the other

### 3. Clique Detection

**Purpose**: Finds cliques (complete subgraphs) in undirected graphs.

**Algorithms Implemented**:

#### 3.1 Backtracking Algorithm (All Cliques)
- Enumerates all possible cliques
- Uses recursive backtracking with pruning
- **Time Complexity**: O(3^(n/3)) - exponential

#### 3.2 Branch and Bound (Maximal Cliques)
- Finds only maximal cliques using Bron-Kerbosch algorithm
- Uses pivot selection heuristic for efficiency
- **Time Complexity**: O(3^(n/3)) worst case, but typically much faster

**Files**: `src/clique.c`, `include/clique.h`

**Algorithm Details** (Bron-Kerbosch):
1. Maintain three sets: R (current clique), P (candidates), X (excluded)
2. Choose pivot vertex to minimize branching
3. For each vertex in P\N(pivot), recursively extend clique
4. Move processed vertices from P to X

### 4. Independent Set

**Purpose**: Finds maximum independent sets (vertices with no edges between them).

**Implementation**: 
- Converts to maximum clique problem on complement graph
- Uses clique detection algorithms on the complement

**Time Complexity**: O(3^(n/3)) - same as clique detection

**Files**: `src/independent_set.c`, `include/independent_set.h`

**Algorithm Details**:
1. Create complement graph G' where edges exist iff they don't exist in G
2. Find maximum clique in G'
3. Maximum clique in G' = maximum independent set in G

### 5. Vertex Cover

**Purpose**: Finds minimum vertex covers (vertices that cover all edges).

**Three Algorithms Implemented**:

#### 5.1 Exact Algorithm via Maximum Independent Set
- Uses relationship: Vertex Cover = V \ Maximum Independent Set
- **Time Complexity**: O(3^(n/3)) - exponential but exact

#### 5.2 König's Theorem for Bipartite Graphs
- Uses Hopcroft-Karp maximum matching algorithm
- Applies König's theorem: |minimum vertex cover| = |maximum matching|
- **Time Complexity**: O(E√V) - polynomial for bipartite graphs

#### 5.3 2-Approximation Algorithm
- Uses maximal matching approach
- Guarantees solution ≤ 2 × optimal
- **Time Complexity**: O(E) - linear and fast

**Files**: `src/vertex_cover.c`, `include/vertex_cover.h`

**Algorithm Details** (König's Theorem):
1. Check if graph is bipartite using BFS coloring
2. Find maximum matching using Hopcroft-Karp algorithm
3. Build alternating tree from unmatched vertices
4. Vertex cover = (Left \ Visited) ∪ (Right ∩ Visited)

### 6. Euler Path Detection

**Purpose**: Finds Eulerian paths and cycles in undirected graphs.

**Implementation**: Uses Hierholzer's algorithm

**Time Complexity**: O(E) where E is the number of edges

**Files**: `src/euler_path.c`, `include/euler_path.h`

**Algorithm Details**:
1. Check Eulerian conditions:
   - Eulerian cycle: all vertices have even degree
   - Eulerian path: exactly 0 or 2 vertices have odd degree
2. Use Hierholzer's algorithm:
   - Start from appropriate vertex
   - Follow edges, removing them as traversed
   - When stuck, backtrack and add to path
   - Continue until all edges used

### 7. Line Graph Generation

**Purpose**: Creates line graphs where edges of original graph become vertices.

**Implementation**: 
- Extracts edges from adjacency matrix
- Creates new graph where edges are adjacent if they share a vertex

**Time Complexity**: O(E²) where E is the number of edges

**Files**: `src/line_graph.c`, `include/line_graph.h`

**Algorithm Details**:
1. Extract all edges from original graph
2. Create mapping from vertices to incident edges
3. Connect edges in line graph if they share a common vertex
4. Generate DOT file for visualization

### 8. Connectivity Number (Vertex Connectivity)

**Purpose**: Calculates the minimum number of vertices whose removal disconnects the graph.

**Two Approaches**:

#### 8.1 Exact Algorithm (Small Graphs)
- Brute force: tries all possible vertex combinations
- **Time Complexity**: O(2^n × (V + E)) - exponential

#### 8.2 Approximation Algorithm (Large Graphs)
- Uses minimum degree as approximation
- **Time Complexity**: O(V²) - polynomial

**Files**: `src/connectivity_number.c`, `include/connectivity_number.h`

**Algorithm Details** (Exact):
1. For k = 1 to n-2:
   - Try all combinations of k vertices to remove
   - Check if remaining graph is connected using DFS
   - Return k if disconnection found
2. If no disconnection found, return n-1 (complete graph)

## Data Structures

### Core Structures

```c
// Graph representation using adjacency matrix
typedef struct {
    int **adjacency;        // Boolean adjacency matrix
    int node_count;         // Number of vertices
    bool is_directed;       // Graph type
    bool allow_bidirectional; // For directed graphs
} Graph;

// Set data structure for vertex collections
typedef struct {
    int *vertices;          // Array of vertex indices
    int size;              // Current number of vertices
    int capacity;          // Maximum capacity
} Set;

// Edge representation
typedef struct {
    int u, v;              // Endpoint vertices
} Edge;

// Connectivity analysis results
typedef struct {
    bool is_strong;        // Strong connectivity (directed)
    bool is_weak;          // Weak connectivity (directed)
    bool is_one_sided;     // One-sided connectivity (directed)
    bool is_connected;     // Connectivity (undirected)
} Connectivity;
```

## Examples

### Example 1: Simple Undirected Graph

Input:
- Graph type: undirected
- Vertices: 4
- Degree sequence: [2, 2, 2, 2]

Output:
- Valid graphical sequence
- Connected graph
- Eulerian cycle exists
- Various cliques and independent sets found

### Example 2: Directed Graph

Input:
- Graph type: directed
- Vertices: 3
- Out-degrees: [1, 1, 1]
- In-degrees: [1, 1, 1]

Output:
- Valid directed graphical sequence
- Connectivity analysis (strong/weak/one-sided)
- Generated visualization files

## Dependencies

### Required
- **GCC**: C compiler with C11 support
- **Make**: Build system
- **Standard C Library**: malloc, stdio, stdlib, string, math

### Optional
- **Graphviz**: For generating PNG images from DOT files
  ```bash
  # Ubuntu/Debian
  sudo apt-get install graphviz
  
  # macOS
  brew install graphviz
  
  # CentOS/RHEL
  sudo yum install graphviz
  ```

## Output Files

The program generates several output files:

- `build/dot_files/graph.dot`: DOT representation of the main graph
- `build/images/graph.png`: PNG visualization of the main graph
- `build/dot_files/line_graph.dot`: DOT representation of the line graph (if generated)
- `build/images/line_graph.png`: PNG visualization of the line graph (if generated)

## Performance Considerations

### Algorithm Complexity Summary

| Algorithm | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| Havel-Hakimi | O(n²) | O(n²) | Graph construction |
| Connectivity | O(V + E) | O(V) | BFS-based |
| Clique Detection | O(3^(n/3)) | O(n²) | Exponential |
| Independent Set | O(3^(n/3)) | O(n²) | Via complement |
| Vertex Cover (Exact) | O(3^(n/3)) | O(n²) | Via MIS |
| Vertex Cover (Bipartite) | O(E√V) | O(V + E) | König's theorem |
| Vertex Cover (Approx) | O(E) | O(V) | 2-approximation |
| Euler Path | O(E) | O(E) | Hierholzer's |
| Line Graph | O(E²) | O(E²) | Edge-based |
| Connectivity Number | O(2^n × (V+E)) | O(V) | Exact (small graphs) |

### Scalability

- **Small graphs (n ≤ 12)**: All algorithms work efficiently
- **Medium graphs (12 < n ≤ 50)**: Exponential algorithms may be slow
- **Large graphs (n > 50)**: Use approximation algorithms for NP-hard problems

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

### Code Style

- Follow C11 standard
- Use meaningful variable names
- Comment complex algorithms
- Maintain consistent indentation (4 spaces)
- Free all allocated memory

## License

This project is open source and available under the MIT License.

## References

1. **Havel-Hakimi Algorithm**: Havel, V. (1955), Hakimi, S. L. (1962)
2. **Bron-Kerbosch Algorithm**: Bron, C. & Kerbosch, J. (1973)
3. **Hierholzer's Algorithm**: Hierholzer, C. (1873)
4. **König's Theorem**: König, D. (1931)
5. **Hopcroft-Karp Algorithm**: Hopcroft, J. E. & Karp, R. M. (1973)

## Contact

For questions, issues, or contributions, please open an issue on the project repository.

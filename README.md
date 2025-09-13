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
- [Performance Considerations](#performance-considerations)
- [Dependencies](#dependencies)
- [Output Files](#output-files)
- [Contributing](#contributing)
- [License](#license)
- [References](#references)

## Overview

This project implements a wide range of graph theory algorithms in C, focusing on both theoretical correctness and practical efficiency. The program can handle both directed and undirected graphs, construct graphs from degree sequences, and perform various analyses including connectivity, clique detection, independent sets, and more.

The implementation follows rigorous academic standards with comprehensive documentation, memory management, and error handling. All algorithms are implemented from scratch with detailed comments explaining the theoretical foundations and algorithmic choices.

## Features

- **Graph Construction**: Build graphs from degree sequences using the Havel-Hakimi algorithm
- **Connectivity Analysis**: Determine strong, weak, and one-sided connectivity for directed graphs
- **Clique Detection**: Find all cliques or maximal cliques using backtracking or Bron-Kerbosch algorithms
- **Independent Sets**: Find maximum independent sets using complement graph techniques
- **Vertex Cover**: Multiple algorithms including exact, approximation, and bipartite-specific methods
- **Euler Paths**: Find Eulerian paths and cycles using Hierholzer's algorithm
- **Line Graphs**: Generate line graphs from original graphs
- **Connectivity Numbers**: Calculate vertex connectivity using exact and approximation methods
- **Visualization**: Generate DOT files and PNG images for graph visualization
- **Interactive Interface**: User-friendly command-line interface with algorithm selection
- **Memory Management**: Automatic cleanup and error handling

## Project Structure

```
Graph_Prj/
├── src/                    # Source files
│   ├── main.c             # Main program entry point with interactive interface
│   ├── havel_hakimi.c     # Graph construction from degree sequences
│   ├── connectivity.c     # Graph connectivity analysis
│   ├── clique.c           # Clique detection algorithms (backtracking & Bron-Kerbosch)
│   ├── independent_set.c  # Maximum independent set algorithms
│   ├── vertex_cover.c     # Vertex cover algorithms (exact, König's, approximation)
│   ├── euler_path.c       # Euler path/cycle detection using Hierholzer's algorithm
│   ├── line_graph.c       # Line graph generation
│   ├── connectivity_number.c # Vertex connectivity calculation
│   └── set_utils.c        # Set data structure utilities
├── include/               # Header files
│   ├── structs.h          # Core data structures (Graph, Node, Set, etc.)
│   ├── havel_hakimi.h     # Graph construction function declarations
│   ├── connectivity.h     # Connectivity analysis function declarations
│   ├── clique.h           # Clique detection function declarations
│   ├── independent_set.h  # Independent set function declarations
│   ├── vertex_cover.h     # Vertex cover function declarations
│   ├── euler_path.h       # Euler path function declarations
│   ├── line_graph.h       # Line graph function declarations
│   ├── connectivity_number.h # Connectivity number function declarations
│   └── set_utils.h        # Set utilities function declarations
├── build/                 # Build artifacts (created automatically)
│   ├── obj/              # Object files
│   ├── dot_files/        # Generated DOT files
│   ├── images/           # Generated PNG images
│   └── graph_program     # Compiled executable
├── Makefile              # Build configuration
├── .gitignore           # Git ignore rules
└── README.md            # This file
```

## Build Instructions

### Prerequisites

- **GCC**: C compiler with C11 support
- **Make**: Build system
- **Graphviz** (optional): For generating PNG images from DOT files

### Building the Project

```bash
# Clone the repository
git clone <repository-url>
cd Graph_Prj

# Build the project
make

# Run the program
./build/graph_program

# Clean build artifacts
make clean
```

### Build Targets

- `make` or `make all`: Build the complete project
- `make clean`: Remove all build artifacts
- Object files are automatically created in `build/obj/`
- The executable is created as `build/graph_program`

## Usage

Run the program and follow the interactive prompts:

```bash
./build/graph_program
```

### Interactive Session Flow

1. **Choose graph type**: Directed or undirected
2. **Select algorithms**: Choose which analyses to perform (for undirected graphs)
3. **Enter degree sequences**: Provide the degree sequence for vertices
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

Graph generated successfully!
DOT file: build/dot_files/graph.dot
PNG visualization: build/images/graph.png

=== Connectivity Analysis ===
Graph connectivity: Connected

=== Clique Analysis ===
Algorithm used: Bron-Kerbosch (maximal cliques)
Maximum clique size: 3
Total cliques found: 2
Non-trivial cliques (size ≥ 3):
  Clique 1 (size 3): 0 1 2
  Clique 2 (size 3): 1 2 3
```

## Algorithms Implemented

### 1. Havel-Hakimi Algorithm

**Purpose**: Constructs a graph from a given degree sequence.

**Implementation Details**:
- **Undirected**: Sorts vertices by degree (descending) and connects highest-degree vertex to next highest-degree vertices
- **Directed**: Handles both in-degree and out-degree sequences, with optional bidirectional edges
- **Validation**: Checks graphical sequence validity (handshaking lemma for undirected, degree sum equality for directed)

**Time Complexity**: O(n²) where n is the number of vertices

**Files**: `src/havel_hakimi.c`, `include/havel_hakimi.h`

**Algorithm Steps**:
1. Sort vertices by degree in descending order
2. Take the vertex with highest degree d
3. Connect it to the next d vertices with highest degrees
4. Reduce degrees of connected vertices
5. Repeat until all degrees are zero or sequence is invalid

### 2. Connectivity Analysis

**Purpose**: Determines various types of connectivity in graphs.

**Types Analyzed**:
- **Undirected**: Connected or disconnected
- **Directed**: Strong, weak, and one-sided connectivity

**Implementation**: Uses BFS traversal to check reachability between vertices

**Time Complexity**: O(V + E) where V is vertices and E is edges

**Files**: `src/connectivity.c`, `include/connectivity.h`

**Connectivity Types**:
- **Strong Connectivity**: All vertices reachable from any vertex following directed edges
- **Weak Connectivity**: All vertices reachable treating edges as undirected
- **One-sided Connectivity**: For any pair of vertices, at least one can reach the other

### 3. Clique Detection

**Purpose**: Finds cliques (complete subgraphs) in undirected graphs.

**Two Algorithms Implemented**:

#### 3.1 Backtracking Algorithm (All Cliques)
- Enumerates all possible cliques using recursive exploration
- Uses three sets: Current clique (C), Candidates (P), Excluded (S)
- **Time Complexity**: O(3^(n/3)) - exponential
- **Use Case**: When you need all cliques, including non-maximal ones

#### 3.2 Bron-Kerbosch Algorithm (Maximal Cliques)
- Finds only maximal cliques using sophisticated pivot selection
- Implements pivot heuristic to minimize branching factor
- **Time Complexity**: O(3^(n/3)) worst case, but typically much faster
- **Use Case**: When you need only maximal cliques (most common requirement)

**Files**: `src/clique.c`, `include/clique.h`

**Bron-Kerbosch Algorithm Details**:
1. Maintain three sets: R (current clique), P (candidates), X (excluded)
2. Choose pivot vertex u ∈ P ∪ X with maximum |N(u) ∩ P|
3. For each vertex v ∈ P \ N(u), recursively extend clique
4. Move processed vertices from P to X

### 4. Independent Set

**Purpose**: Finds maximum independent sets (vertices with no edges between them).

**Implementation Strategy**: 
- Converts to maximum clique problem on complement graph
- Uses clique detection algorithms on the complement
- Leverages the mathematical relationship: MIS(G) = MC(Ḡ)

**Time Complexity**: O(3^(n/3)) - same as clique detection

**Files**: `src/independent_set.c`, `include/independent_set.h`

**Algorithm Steps**:
1. Create complement graph Ḡ where edges exist iff they don't exist in G
2. Find maximum clique in Ḡ using Bron-Kerbosch algorithm
3. Maximum clique in Ḡ = maximum independent set in G

### 5. Vertex Cover

**Purpose**: Finds minimum vertex covers (vertices that cover all edges).

**Three Algorithms Implemented**:

#### 5.1 Exact Algorithm via Maximum Independent Set
- Uses relationship: Vertex Cover = V \ Maximum Independent Set
- **Time Complexity**: O(3^(n/3)) - exponential but exact
- **Guarantee**: Optimal solution

#### 5.2 König's Theorem for Bipartite Graphs
- Uses Hopcroft-Karp maximum matching algorithm
- Applies König's theorem: |minimum vertex cover| = |maximum matching|
- **Time Complexity**: O(E√V) - polynomial for bipartite graphs
- **Guarantee**: Optimal for bipartite graphs

#### 5.3 2-Approximation Algorithm
- Uses maximal matching approach
- Guarantees solution ≤ 2 × optimal
- **Time Complexity**: O(E) - linear and fast
- **Guarantee**: 2-approximation

**Files**: `src/vertex_cover.c`, `include/vertex_cover.h`

**König's Algorithm Steps**:
1. Check if graph is bipartite using BFS coloring
2. Find maximum matching using Hopcroft-Karp algorithm
3. Build alternating tree from unmatched vertices
4. Vertex cover = (Left \ Visited) ∪ (Right ∩ Visited)

### 6. Euler Path Detection

**Purpose**: Finds Eulerian paths and cycles in undirected graphs.

**Implementation**: Uses Hierholzer's algorithm with degree validation

**Time Complexity**: O(E) where E is the number of edges

**Files**: `src/euler_path.c`, `include/euler_path.h`

**Algorithm Steps**:
1. **Validation Phase**: Check Eulerian conditions
   - Eulerian cycle: all vertices have even degree
   - Eulerian path: exactly 0 or 2 vertices have odd degree
2. **Construction Phase**: Use Hierholzer's algorithm
   - Start from appropriate vertex (odd degree for path, any for cycle)
   - Follow edges, removing them as traversed
   - When stuck, backtrack and add to path
   - Continue until all edges used

### 7. Line Graph Generation

**Purpose**: Creates line graphs where edges of original graph become vertices.

**Implementation**: 
- Extracts all edges from adjacency matrix
- Creates new graph where edges are adjacent if they share a vertex
- Generates DOT file for visualization

**Time Complexity**: O(E²) where E is the number of edges

**Files**: `src/line_graph.c`, `include/line_graph.h`

**Algorithm Steps**:
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
- **Use Case**: Small graphs (n ≤ 12)

#### 8.2 Approximation Algorithm (Large Graphs)
- Uses minimum degree as approximation
- **Time Complexity**: O(V²) - polynomial
- **Use Case**: Large graphs where exact computation is infeasible

**Files**: `src/connectivity_number.c`, `include/connectivity_number.h`

**Exact Algorithm Steps**:
1. For k = 1 to n-2:
   - Try all combinations of k vertices to remove
   - Check if remaining graph is connected using DFS
   - Return k if disconnection found
2. If no disconnection found, return n-1 (complete graph)

## Data Structures

### Core Structures

```c
/**
 * Graph representation using adjacency matrix
 */
typedef struct {
    int **adjacency;        // Boolean adjacency matrix (0/1)
    int node_count;         // Number of vertices
    bool is_directed;       // Graph type flag
    bool allow_bidirectional; // For directed graphs
} Graph;

/**
 * Dynamic set data structure for vertex collections
 */
typedef struct {
    int *vertices;          // Array of vertex indices
    int size;              // Current number of vertices
    int capacity;          // Maximum capacity
} Set;

/**
 * Node structure for Havel-Hakimi algorithm
 */
typedef struct {
    int original_index;     // Original vertex number
    int degree;            // Out-degree (directed) or degree (undirected)
    int in_degree;         // In-degree (directed graphs only)
} Node;

/**
 * Edge representation
 */
typedef struct {
    int u, v;              // Endpoint vertices
} Edge;

/**
 * Connectivity analysis results
 */
typedef struct {
    bool is_strong;        // Strong connectivity (directed)
    bool is_weak;          // Weak connectivity (directed)
    bool is_one_sided;     // One-sided connectivity (directed)
    bool is_connected;     // Connectivity (undirected)
} Connectivity;
```

## Examples

### Example 1: Simple Undirected Graph

**Input:**
```
Graph type: undirected
Vertices: 4
Degree sequence: [2, 2, 2, 2]
```

**Output:**
```
Graph generated successfully!
=== Connectivity Analysis ===
Graph connectivity: Connected

=== Clique Analysis ===
Maximum clique size: 3
Total cliques found: 2

=== Euler Path Analysis ===
Eulerian cycle exists: Yes
Path: 0 -> 1 -> 2 -> 3 -> 0
```

### Example 2: Directed Graph

**Input:**
```
Graph type: directed
Vertices: 3
Out-degrees: [1, 1, 1]
In-degrees: [1, 1, 1]
```

**Output:**
```
Graph generated successfully!
=== Connectivity Analysis ===
Strong connectivity: Yes
Weak connectivity: Yes
One-sided connectivity: Yes
```

### Example 3: Complex Analysis

**Input:**
```
Graph type: undirected
Vertices: 6
Degree sequence: [3, 3, 3, 3, 2, 2]
All analyses: yes
```

**Output:**
```
=== Connectivity Analysis ===
Graph connectivity: Connected

=== Clique Analysis ===
Maximum clique size: 4
Non-trivial cliques found: 3

=== Maximum Independent Set Analysis ===
Maximum Independent Set: {1, 4}
Size: 2

=== Vertex Cover Analysis ===
Minimum Vertex Cover (size=4): 0 2 3 5

=== Euler Path Analysis ===
Eulerian path exists: No (4 vertices have odd degree)

=== Connectivity Number Analysis ===
Vertex connectivity: 2
```

## Performance Considerations

### Algorithm Complexity Summary

| Algorithm | Time Complexity | Space Complexity | Scalability | Notes |
|-----------|----------------|------------------|-------------|-------|
| Havel-Hakimi | O(n²) | O(n²) | Excellent | Graph construction |
| Connectivity | O(V + E) | O(V) | Excellent | BFS-based |
| Clique (Backtracking) | O(3^(n/3)) | O(n²) | Poor | All cliques |
| Clique (Bron-Kerbosch) | O(3^(n/3)) | O(n²) | Moderate | Maximal cliques, pivot optimization |
| Independent Set | O(3^(n/3)) | O(n²) | Moderate | Via complement graph |
| Vertex Cover (Exact) | O(3^(n/3)) | O(n²) | Poor | Via MIS |
| Vertex Cover (Bipartite) | O(E√V) | O(V + E) | Good | König's theorem |
| Vertex Cover (Approx) | O(E) | O(V) | Excellent | 2-approximation |
| Euler Path | O(E) | O(E) | Excellent | Hierholzer's algorithm |
| Line Graph | O(E²) | O(E²) | Moderate | Edge-based construction |
| Connectivity Number (Exact) | O(2^n × (V+E)) | O(V) | Very Poor | Brute force |
| Connectivity Number (Approx) | O(V²) | O(V) | Good | Min-degree heuristic |

### Scalability Guidelines

- **Small graphs (n ≤ 12)**: All algorithms work efficiently
- **Medium graphs (12 < n ≤ 50)**: Exponential algorithms may be slow, use approximations
- **Large graphs (n > 50)**: Use only polynomial-time algorithms
- **Very large graphs (n > 1000)**: Consider specialized data structures and algorithms

### Memory Usage

- **Adjacency Matrix**: O(n²) space - suitable for dense graphs
- **Set Operations**: Dynamic allocation with automatic resizing
- **Temporary Storage**: Algorithms clean up automatically

## Dependencies

### Required
- **GCC**: C compiler with C11 support
- **Make**: Build system
- **Standard C Library**: malloc, stdio, stdlib, string, math, stdbool

### Optional
- **Graphviz**: For generating PNG images from DOT files
  ```bash
  # Ubuntu/Debian
  sudo apt-get install graphviz
  
  # macOS
  brew install graphviz
  
  # CentOS/RHEL/Fedora
  sudo dnf install graphviz
  ```

## Output Files

The program automatically creates the following directory structure and files:

```
build/
├── obj/                    # Object files (*.o)
├── dot_files/             # DOT format files
│   ├── graph.dot         # Main graph visualization
│   └── line_graph.dot    # Line graph (if generated)
├── images/               # PNG visualizations
│   ├── graph.png         # Main graph image
│   └── line_graph.png    # Line graph image (if generated)
└── graph_program         # Compiled executable
```

### File Descriptions

- **DOT files**: Text-based graph description in Graphviz DOT language
- **PNG files**: Visual graph representations (requires Graphviz)
- **Object files**: Intermediate compilation artifacts
- **Executable**: The main program binary

## Contributing

We welcome contributions to improve the project! Here's how to contribute:

### Getting Started
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests if applicable
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

### Code Style Guidelines

- **C Standard**: Follow C11 standard
- **Naming**: Use descriptive variable and function names
- **Comments**: Document complex algorithms and data structures
- **Indentation**: Use 4 spaces consistently
- **Memory Management**: Always free allocated memory
- **Error Handling**: Check return values and handle errors gracefully

### Areas for Contribution

- **Algorithm Optimization**: Improve existing algorithm implementations
- **New Algorithms**: Add additional graph theory algorithms
- **Data Structures**: Implement more efficient data structures
- **Visualization**: Enhance graph visualization capabilities
- **Testing**: Add comprehensive test suites
- **Documentation**: Improve code documentation and examples

## License

This project is open source and available under the MIT License.

```
MIT License

Copyright (c) 2024 Graph Theory Project Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## References

### Academic Papers and Books

1. **Havel-Hakimi Algorithm**: 
   - Havel, V. (1955). "A remark on the existence of finite graphs"
   - Hakimi, S. L. (1962). "On realizability of a set of integers as degrees of the vertices of a linear graph"

2. **Bron-Kerbosch Algorithm**: 
   - Bron, C. & Kerbosch, J. (1973). "Algorithm 457: finding all cliques of an undirected graph"

3. **Hierholzer's Algorithm**: 
   - Hierholzer, C. (1873). "Über die Möglichkeit, einen Linienzug ohne Wiederholung der Kanten zu umfahren"

4. **König's Theorem**: 
   - König, D. (1931). "Graphs and matrices"

5. **Hopcroft-Karp Algorithm**: 
   - Hopcroft, J. E. & Karp, R. M. (1973). "An n^5/2 algorithm for maximum matchings in bipartite graphs"

### Textbooks

- **Introduction to Algorithms** by Cormen, Leiserson, Rivest, and Stein
- **Graph Theory** by Reinhard Diestel
- **Algorithmic Graph Theory** by David Joyner, Minh Van Nguyen, and Nathann Cohen

### Online Resources

- [Graph Theory Algorithms](https://en.wikipedia.org/wiki/Category:Graph_algorithms)
- [Graphviz Documentation](https://graphviz.org/documentation/)
- [C11 Standard Reference](https://en.cppreference.com/w/c)

## Contact

For questions, issues, or contributions:

- **Issues**: Open an issue on the project repository
- **Discussions**: Use the repository's discussion section
- **Pull Requests**: Submit improvements via pull requests

---

**Project Status**: Active Development  
**Last Updated**: 2024  
**Version**: 1.0.0

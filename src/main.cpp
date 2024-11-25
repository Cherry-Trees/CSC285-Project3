/**
 *  @author Jamie Miles
 *  @date   11/23/2024
 *  
 *  Fibonacci heap implementation of Dijkstra's algorithm => O(E+VlogV)
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "FibonacciHeap_Impl.hpp"
#include "NNSet.hpp" // NNSet from Project 1.

// Edge struct.
struct Edge {
    std::string to;     // Vertex this edge is pointing to.
    std::size_t weight; // Weight of this edge.

    // Compare two edges (required for unordered_set).
    bool operator==(const Edge &other) const {
        return to == other.to && weight == other.weight;
    }
    // Hash the edge (required for unordered_set).
    struct Hash {
        static std::size_t operator()(const Edge &e) {
            return std::hash<std::string>{}(e.to) ^ std::hash<std::size_t>{}(e.weight);
        }
    };
};

// Vertex struct containing airport information.
// Also acts as a singly-linked list to track min-cost path.
struct Vertex {
    std::string name;  // Airport name.
    std::size_t cost;  // Min-cost to travel here.
    std::size_t index; // Index to key with.
    Vertex *prev;      // Hold the previous airport in the min-cost path.
    std::unordered_set<Edge, Edge::Hash> adj;   // Adjacency set.
    
    Vertex(): 
        name(), 
        cost(0), 
        prev(0) {}
    Vertex(const std::string name, const std::size_t cost, const std::size_t index): 
        name(name), 
        cost(cost), 
        index(index), 
        prev(0) {}

    // Key functor to specify which Vertex variable represents the key.
    struct Key {
        static std::size_t &operator()(Vertex &r) {return r.cost;}
        static std::size_t &operator()(Vertex *r) {return r->cost;}

        // Comparison functor to specify how the min-heap will sort keys.
        struct Less {
            static bool operator()(const std::size_t first, const std::size_t second) {
                return first < second;
            }
        };
    };

    // Heap node that this Vertex is being stored in.
    FibonacciHeap<Vertex *, Vertex::Key, Vertex::Key::Less>::Node *node;
};

int main() {

    // Get file name, source name, and destination name.
    std::string file_name, src_name, dest_name;
    std::size_t airport_cost;

    std::cout << "Flight data file name? ";
    std::cin >> file_name;
    
    std::cout << "Cost of using one airport? ";
    std::cin >> airport_cost;

    std::cout << "Airports traveling between? ";
    std::cin >> src_name >> dest_name;

    std::ifstream in;
    in.open(file_name);
    
    // Keep track of number of vertices and construct the graph.
    std::size_t V = 0;
    std::unordered_map<std::string, Vertex> graph; // Key by airport name.

    // Read input file.
    while (!in.eof()) {
        std::string from, to;
        std::size_t cost;
        in >> from >> to >> cost;

        if (from.empty() || to.empty()) break;
        if (graph.insert({from, Vertex(from, SIZE_MAX, V)}).second) {++V;}
        if (graph.insert({to,   Vertex(from, SIZE_MAX, V)}).second) {++V;}

        graph[from].adj.insert({to, cost + 2*airport_cost});
        graph[to].adj.insert({from, cost + 2*airport_cost});
    }
    in.close();
    
    // Construct the min heap.
    FibonacciHeap<Vertex *, Vertex::Key, Vertex::Key::Less> heap;

    // Keep track of indices we've visited.
    NNSet visited(V);

    // Get pointers to source and destination graph for readability.
    Vertex *src = &graph[src_name];
    Vertex *dest = &graph[dest_name];

    // Push all graph to the heap.
    for (auto & [v_name, v] : graph) {
        v.name = v_name;
        v.cost = SIZE_MAX;
        v.node = heap.push(&v);
    }

    // Set the source cost to 0.
    src->cost = 0;
    heap.decrease_key(src->node, 0);

    // Dijkstra’s algorithm.
    while (!heap.empty()) {
        Vertex *x = heap.pop(); // Pop the min vertex off the heap. This is an O(logn) operation.
        visited.add(x->index); // Add the vertex to the visited set.

        // Iterate through airports adj to x that haven't been visited.
        for (auto &e : graph[x->name].adj) {
            Vertex *v = &graph[e.to]; // Get pointer to the vertex for readability.
            if (!visited.contains(v->index)) { 
                if (x->cost + e.weight < v->cost) {
                    v->cost = x->cost + e.weight;
                    v->prev = x; // Link the adj vertex v to x.
                    heap.decrease_key(v->node, v->cost); // Decrease adj key in the heap.
                }
            }
        }
    }

    // Print min-cost to travel from source to destination.
    std::cout << "Min cost from " << src->name 
              << " to " << dest->name 
              << " is " << dest->cost << std::endl;

    // Print min-cost path.
    // Since the path is linked dest --> ... --> src, print it using backwards recursion.
    auto print_path = [](Vertex *v, bool parent, auto &&print_path) -> void {
        if (v->prev)
            print_path(v->prev, 0, print_path);
        std::string arrow = (parent) ? "" : " --> ";
        std::cout << v->name << arrow;
    }; print_path(dest, 1, print_path);

    return 0;
}

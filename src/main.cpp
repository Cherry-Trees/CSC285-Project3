/**
 *  @author Jamie Miles
 *  @date   11/23/2024
 *  
 *  Fibonacci heap implementation of Dijkstra's algorithm => O(E+VlogV)
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Graph.hpp"
#include "FibonacciHeap_Impl.hpp"
#include "NNSet.hpp" // NNSet from Project 1.

// Struct containing airport information.
// Acts as a singly-linked list to track min-cost path.
struct Record {
    std::string name;  // Airport name.
    std::size_t cost;  // Min-cost to travel here.
    std::size_t index; // Index to key with.
    Record *prev;      // Hold the previous airport in the min-cost path.
    
    Record(): name(), cost(0), prev(0) {}
    Record(const std::string name, const std::size_t cost, const std::size_t index): name(name), cost(cost), index(index), prev(0) {}

    // Key functor to specify which Record variable represents the key.
    struct Key {
        static std::size_t &operator()(Record &r) {return r.cost;}
        static std::size_t &operator()(Record *r) {return r->cost;}

        // Comparison functor to specify how the min-heap will sort keys.
        struct Less {
            static bool operator()(const std::size_t first, const std::size_t second) {
                return first < second;
            }
        };
    };

    // Heap node that this record is being stored in.
    FibonacciHeap<Record *, Record::Key, Record::Key::Less>::Node *node;
};

int main() {

    // Construct graph object.
    Graph flights;

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
    
    // Keep track of number of vertices and store records.
    std::size_t V = 0;
    std::unordered_map<std::string, Record> records; // Key by airport name.

    // Read input file.
    while (!in.eof()) {
        std::string from, to;
        std::size_t cost;
        in >> from >> to >> cost;

        if (from.empty() || to.empty()) break;
        if (flights.addVertex(from)) records.insert({from, Record(from, SIZE_MAX, V++)});
        if (flights.addVertex(to)) records.insert({to, Record(to, SIZE_MAX, V++)});
        flights.addEdge(from, to, cost + 2*airport_cost); // Cost to leave and arrive at airport = 2 * airport_cost
    }
    in.close();
    
    // Construct the min heap.
    FibonacciHeap<Record *, Record::Key, Record::Key::Less> heap;

    // Keep track of indices we've visited.
    NNSet visited(V);

    // Get pointers to source and destination records for readability.
    Record *src = &records[src_name];
    Record *dest = &records[dest_name];

    // Push all records to the heap.
    for (auto &v_name : flights.getVertices()) {
        Record *v = &records[v_name];
        v->name = v_name;
        v->cost = SIZE_MAX;
        v->node = heap.push(v);
    }

    // Set the source cost to 0.
    src->cost = 0;
    heap.decrease_key(src->node, 0);

    // Dijkstra’s algorithm.
    while (!heap.empty()) {
        Record *x = heap.pop(); // Pop the min record off the heap. This is an O(logn) operation.
        visited.add(x->index); // Add the record to the visited set.

        // Iterate through airports adj to x that haven't been visited.
        for (auto &v_name : flights.getAdjacencies(x->name)) {
            Record *v = &records[v_name]; // Get pointer to the record for readability.
            if (!visited.contains(v->index)) { 
                int weight = flights.getEdgeCost(x->name, v->name);
                if (x->cost + weight < v->cost) {
                    v->cost = x->cost + weight;
                    v->prev = x; // Link the adj record to x.
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
    auto print_path = [](Record *r, bool parent, auto &&print_path) -> void {
        if (r->prev)
            print_path(r->prev, 0, print_path);
        std::string arrow = (parent) ? "" : " --> ";
        std::cout << r->name << arrow;
    }; print_path(dest, 1, print_path);

    return 0;
}
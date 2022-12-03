
#pragma once

#include <heap.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace local {

    struct edge_to {
        edge_to(uint32_t v, uint32_t w) : _v(v), _w(w) {}

        uint32_t _v;
        uint32_t _w;
    };

    struct vertex : public HeapNode {

        std::vector<edge_to> *_edges;

        vertex() : HeapNode(), _edges(nullptr) {}

        explicit vertex(std::vector<edge_to> *edges);
    };

    class DIMACS_graph {
    public:
        uint32_t edges_count, nodes_count;

        explicit DIMACS_graph(std::string input_file_path, uint32_t heap_arity);

    private:
        static inline uint32_t position_to_array(uint32_t position);

        std::vector<std::vector<edge_to>> edges;
        std::vector<vertex> nodes;
        MinKHeap<vertex> *distance_heap;
    };
}
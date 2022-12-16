
#pragma once

#include <heap.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

namespace local {

    struct edge_to {
        edge_to() = default;

        edge_to(uint32_t v, uint32_t w) : _v(v), _w(w) {}

        uint32_t _v;
        uint32_t _w;

//        friend std::ostream& operator<<(std::ostream& os, const edge_to& edgeTo);
    };

    struct vertex : public HeapNode {
        uint32_t id;
        edge_to *_edges_storage;
        uint32_t _edges_count;
        bool _visited;

        vertex() : HeapNode(), _edges_storage(nullptr), _edges_count(0), _visited(false) {}

        vertex(edge_to *edges, uint32_t out_edges);

//        friend std::ostream& operator<<(std::ostream& os, const vertex& vtx);
    };

    class DIMACS_graph {
    public:
        uint32_t edges_count, nodes_count;

        explicit DIMACS_graph(std::string input_file_path);

        uint32_t shortest_path_length(uint32_t u, uint32_t v, uint32_t k);

    private:
        static inline uint32_t position_to_array(uint32_t position);

        std::vector<edge_to> edges;
        std::vector<vertex> nodes;;
    };
}


#pragma once

#include <heap.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

namespace local {

    struct edge_to {
        edge_to() = default;

        edge_to(uint32_t v, int32_t w) : _v(v), _w(w) {}

        uint32_t _v;
        int32_t _w;
    };

    struct vertex : public HeapNode {
        int32_t _potential;
        bool _free;

        vertex() : HeapNode(), _free(true), _potential(0) {}
    };

    class BipCompGraph {
    public:
        int32_t edges_count, nodes_count;

        explicit BipCompGraph(std::istream &in);

        int32_t get_augmenting_path_end_node_from(int32_t u, std::vector<int32_t> &dist, std::vector<int32_t>& pred, std::vector<int>& match);

    private:
        int32_t get_weight(int32_t u, int32_t v);

        std::vector<edge_to> edges;
        std::vector<vertex> nodes;
    };
}

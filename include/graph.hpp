
#pragma once

#include <heap.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

namespace local {

    enum class MagicNodes {
        NULL_NODE = -1
    };

    struct edge_to {
        edge_to() = default;

        edge_to(uint32_t v, int32_t w) : _v(v), _w(w) {}

        uint32_t _v;
        int32_t _w;
    };

    struct vertex : HeapNode {
        int32_t id, _potential;
        bool _free;

        vertex() : id(static_cast<int> (MagicNodes::NULL_NODE)),
                   _potential(0),
                   _free(true){}
    };

    class BipCompGraph {
    public:

        enum class Partition {
            S,
            T
        };

        int32_t edges_count, nodes_count;

        explicit BipCompGraph(std::istream &in);

        int32_t getMaximumMatching(std::vector<int> &matching);

        friend std::ostream &operator<<(std::ostream &os, const BipCompGraph &graph);

        void getMaximumPerfectMatching(std::vector<int> &matching);

        [[nodiscard]] Partition getPartition(int n) const;

        bool validNodeIndex(int n) const;

        int32_t expected_maximum_match;

    private:

        int32_t get_weight_johnson(int32_t u, int32_t v);

        int32_t get_weight_raw(int32_t u, int32_t v);

        int32_t set_weight(int32_t u, int32_t v, int32_t w);

        vertex * get_augmenting_path_end_node(std::vector<int32_t> &dist,
                                              std::vector<int32_t> &pred,
                                              std::vector<int> &match);

        int32_t net_score_over_new_augmenting_path(int32_t s, int32_t t, const std::vector<int32_t> &pred);

        void update_matching(int32_t s,
                             int32_t t,
                             const std::vector<int32_t> &pred,
                             std::vector<int32_t> &match);

        void update_potentials(const std::vector<int> &dist);
        std::vector<edge_to> edges;
        std::vector<vertex> nodes;
    };

    std::ostream &operator<<(std::ostream &os, const local::BipCompGraph &graph);
}

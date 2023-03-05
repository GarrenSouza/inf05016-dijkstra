
#include <graph.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <cassert>

local::BipCompGraph::BipCompGraph(std::istream &in) {
    int32_t n, w, max_w = INT32_MIN;

    std::string line, dummy;

    std::cin >> n;

    nodes_count = 2 * n;
    edges_count = n * n;

    nodes.resize(nodes_count);
    edges.resize(edges_count);

    for (int i = 0; i < n; ++i) {
        uint32_t dest = n + i;
        for (int j = 0; j < n; ++j) {
            in >> w;
            edges[i] = {dest, w};
            if (w > max_w)
                max_w = w;
        }
    }
    for (int i = 0; i < n; ++i) {
        nodes[i]._potential = max_w;
    }
}

std::ostream &operator<<(std::ostream &os, const local::edge_to &edgeTo) {
    os << "{\n\"v\" : " << edgeTo._v << ",\n";
    os << "\"w\" : " << edgeTo._w << "\n";
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const local::vertex &dt) {
    os << "{";
    os << "free: " << dt._free << ",\n";
    os << "potential: " << dt._potential << ",\n";
    os << "}";
    return os;
}

int32_t
local::BipCompGraph::get_augmenting_path_end_node_from(int32_t u, std::vector<int32_t> &dist, std::vector<int32_t> &pred,
                                                       std::vector<int> &match) {
    struct custom_less {
        bool operator()(const std::pair<int, int> &a, const std::pair<int, int> &b) {
            return a.first < b.first;
        }
    };

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, custom_less> pq;

    auto decrease_key = [this, &dist, &pred, &pq](int32_t local, int32_t neighbour) {
        auto w = get_weight(local, neighbour);
        // consider Johnson's weighting (apply potential)
        if (dist[neighbour] > dist[local] + w) {
            dist[neighbour] = dist[local] + w;
            pred[neighbour] = local;
            pq.emplace(dist[neighbour], neighbour);
        }
    };

    pq.emplace(0, u);
    dist[u] = 0;

    while (!pq.empty()) {
        auto local = pq.top().second;

        pq.pop();

        if (local < nodes_count / 2) {
            for (int32_t neighbour = nodes_count / 2; neighbour < nodes_count; ++neighbour)
                if (neighbour != match[local])
                    decrease_key(local, neighbour);
        } else {
            if (match[local] < nodes_count)
                decrease_key(local, match[local]);
            else
                return local;
        }
    }

    return 0;
}

int32_t local::BipCompGraph::get_weight(int32_t u, int32_t v) {
    assert((u < nodes_count / 2 && v >= nodes_count / 2) || (u >= nodes_count / 2 && v < nodes_count / 2));
    if (u >= nodes_count / 2)
        std::swap(u, v);
    return edges[v * nodes_count / 2 + u]._w - (nodes[v]._potential - nodes[u]._potential);
}

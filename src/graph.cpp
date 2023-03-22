
#include <graph.hpp>
#include <heap.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <cassert>
#include <cstring>

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
            edges[i * n + j] = {dest, w};
            if (w > max_w)
                max_w = w;
        }
    }

    for (int i = 0; i < nodes_count; ++i) {
        if (i >= n)
            nodes[i]._potential = -max_w;
        nodes[i].id = i;
    }

    in >> expected_maximum_match;
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

local::vertex *local::BipCompGraph::get_augmenting_path_end_node(std::vector<int32_t> &dist,
                                                                 std::vector<int32_t> &pred,
                                                                 std::vector<int> &match) {

    local::MinKHeap<vertex> pq(4);

    auto decrease_key = [this, &dist, &pred, &pq](int32_t local, int32_t neighbour) {
        auto w = get_weight_johnson(local, neighbour);
        if (dist[neighbour] == INT32_MAX) {
            dist[neighbour] = dist[local] + w;
            pred[neighbour] = local;
            pq.insert(&nodes[neighbour]);
        } else if (dist[local] + w < dist[neighbour]) {
            dist[neighbour] = dist[local] + w;
            pred[neighbour] = local;
            pq.update(&nodes[neighbour]);
        }
    };

    std::vector<bool> explored(nodes_count, false);

    for (int i = 0; i < nodes_count; ++i) {
        if (i < nodes_count / 2 && !validNodeIndex(match[i])) {
            pq.insert(&nodes[i]);
            nodes[i].key = 0;
            dist[i] = 0;
        } else {
            nodes[i].key = UINT32_MAX;
            dist[i] = INT32_MAX;
        }
    }

    vertex *closest_free_node_in_T;

    closest_free_node_in_T = nullptr;

    while (!pq.is_empty()) {

        vertex *u = pq.pop_min();
        if (!explored[u->id]) {
            explored[u->id] = true;
            switch (getPartition(u->id)) {
                case Partition::S:
                    for (int32_t neighbour = nodes_count / 2; neighbour < nodes_count; ++neighbour)
                        if (neighbour != match[u->id])
                            decrease_key(u->id, neighbour);
                    break;
                case Partition::T:
                    decrease_key(u->id, match[u->id]);
                    if (!validNodeIndex(match[u->id]) &&
                        (!closest_free_node_in_T || dist[u->id] < dist[closest_free_node_in_T->id]))
                        closest_free_node_in_T = u;
                    break;
                default:
                    std::stringstream ss;
                    ss << "Unexpected set for node " << u;
                    throw std::runtime_error(ss.str());
            }
        }
    }

    return closest_free_node_in_T;
}

int32_t local::BipCompGraph::get_weight_johnson(int32_t u, int32_t v) {
    assert((u < nodes_count / 2 && v >= nodes_count / 2) || (u >= nodes_count / 2 && v < nodes_count / 2));
    if (u >= nodes_count / 2)
        std::swap(u, v);
    return edges[(v - nodes_count / 2) * nodes_count / 2 + u]._w - (nodes[v]._potential - nodes[u]._potential);
}

local::BipCompGraph::Partition local::BipCompGraph::getPartition(int n) const {
    if (0 <= n && n <= nodes_count) {
        if (n < nodes_count / 2)
            return Partition::S;
        else
            return Partition::T;
    }
    std::stringstream ss;
    ss << "Node at index " << n << " does not belong to S, neither to T";

    throw std::runtime_error(ss.str());
}

int32_t local::BipCompGraph::getMaximumMatching(std::vector<int32_t> &matching) {

    // normalize arc weights
    for (auto &e: edges)
        e._w *= -1;

    int32_t last_sum = INT32_MIN, current_sum = 0, net_score;

    vertex *closest_free_node;

    std::vector<int32_t>
            distance(nodes_count, INT32_MAX),
            predecessor(nodes_count, static_cast<int32_t>(MagicNodes::NULL_NODE));

    while (current_sum > last_sum) {
        last_sum = current_sum;

        // reset the distance to nodes in T


        closest_free_node = get_augmenting_path_end_node(distance, predecessor, matching);
        if (closest_free_node &&
            (net_score = net_score_over_new_augmenting_path(static_cast<int32_t>(MagicNodes::NULL_NODE),
                                                            closest_free_node->id,
                                                            predecessor)) >= 0) {
            update_matching(static_cast<int32_t>(MagicNodes::NULL_NODE), closest_free_node->id, predecessor, matching);
            update_potentials(distance);
            current_sum += net_score;
        } else
            break;
    }
    return current_sum;
}

void local::BipCompGraph::getMaximumPerfectMatching(std::vector<int32_t> &matching) {

}

std::ostream &local::operator<<(std::ostream &os, const local::BipCompGraph &graph) {
    os << "edges_count: " << graph.edges_count << ", nodes_count: " << graph.nodes_count << '\n';
    for (int32_t i = 0; i < graph.nodes_count / 2; ++i) {
        os << graph.edges[i * graph.nodes_count / 2]._w;
        for (int32_t j = 1; j < graph.nodes_count / 2; ++j) {
            os << ' ' << graph.edges[i * graph.nodes_count / 2 + j]._w;
        }
        os << '\n';
    }
    return os;
}

bool local::BipCompGraph::validNodeIndex(int32_t n) const {
    return 0 <= n && n < nodes_count;
}

int32_t
local::BipCompGraph::net_score_over_new_augmenting_path(int32_t s, int32_t t, const std::vector<int32_t> &pred) {
    int32_t new_score = 0, old_score = 0;
    bool new_path = true;
    for (int i = t; pred[i] != static_cast<int32_t>(MagicNodes::NULL_NODE); i = pred[i]) {
        if (new_path) {
            new_score += -1 * get_weight_raw(pred[i], i);
        } else {
            old_score += get_weight_raw(pred[i], i);
        }
        new_path = !new_path;
    }
    return new_score - old_score;
}

void local::BipCompGraph::update_matching(int32_t s,
                                          int32_t t,
                                          const std::vector<int32_t> &pred,
                                          std::vector<int32_t> &match) {
    bool new_path = true;
    for (int i = t; pred[i] != static_cast<int32_t>(MagicNodes::NULL_NODE); i = pred[i]) {
        match[i] = new_path ? pred[i] : static_cast<int32_t>(MagicNodes::NULL_NODE);
        if(new_path){
            match[i] =pred[i];
            match[pred[i]] = i;
        } else {
            match[i] =  static_cast<int32_t>(MagicNodes::NULL_NODE);
            match[pred[i]] =  static_cast<int32_t>(MagicNodes::NULL_NODE);
        }
        set_weight(pred[i], i, -1 * get_weight_raw(pred[i], i));
        new_path = !new_path;
    }
}

int32_t local::BipCompGraph::set_weight(int32_t u, int32_t v, int32_t w) {
    assert((u < nodes_count / 2 && v >= nodes_count / 2) || (u >= nodes_count / 2 && v < nodes_count / 2));
    if (u >= nodes_count / 2)
        std::swap(u, v);
    edges[(v - nodes_count/2) * nodes_count / 2 + u]._w = w;
    return w;
}

void local::BipCompGraph::update_potentials(const std::vector<int> &dist) {
    for (int i = 0; i < nodes_count; ++i) {
        nodes[i]._potential += dist[i];
    }
}

int32_t local::BipCompGraph::get_weight_raw(int32_t u, int32_t v) {
    assert((u < nodes_count / 2 && v >= nodes_count / 2) || (u >= nodes_count / 2 && v < nodes_count / 2));
    if (u >= nodes_count / 2)
        std::swap(u, v);
    return edges[(v - nodes_count / 2) * nodes_count / 2 + u]._w;
}

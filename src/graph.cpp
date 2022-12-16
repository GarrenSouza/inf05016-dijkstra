
#include <graph.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <assert.h>

std::vector<int> *edges_per_vertex(std::string input_file_path) {
    auto *counters = new std::vector<int>;

    std::fstream in(input_file_path);
    std::string line, dummy;

    while (line.substr(0, 4) != "p sp")
        getline(in, line);

    std::stringstream linestr;

    uint32_t vertices, edges;

    linestr.str(line);
    linestr >> dummy >> dummy >> vertices >> edges;

    counters->resize(vertices);


    uint32_t i = 0;
    while (i < vertices) {
        counters->at(i) = 0;
        i++;
    }

    i = 0;
    while (i < edges) {
        getline(in, line);
        if (line.substr(0, 2) == "a ") {
            std::stringstream arc(line);
            uint32_t u, v, w;
            char _;
            arc >> _ >> u >> v >> w;
            counters->at(u - 1) += 1;
            i++;
        }
    }

    i = 1;
    if (!counters->empty()) counters->at(0) -= 1;
    while (i < vertices) {
        counters->at(i) += counters->at(i - 1);
        i++;
    }
    return counters;
}

local::DIMACS_graph::DIMACS_graph(std::string input_file_path) {
    std::vector<int> *edge_counters = edges_per_vertex(input_file_path);

    uint32_t i;

    std::fstream in(input_file_path);
    std::string line, dummy;

    while (line.substr(0, 4) != "p sp") {
        getline(in, line);
    }

    std::stringstream linestr;

    linestr.str(line);
    linestr >> dummy >> dummy >> nodes_count >> edges_count;

    nodes.resize(nodes_count);
    edges.resize(edges_count);

    i = 0;
    while (i < edges_count) {
        getline(in, line);
        if (line.substr(0, 2) == "a ") {
            std::stringstream arc(line);
            uint32_t u, v, w;
            char _;
            arc >> _ >> u >> v >> w;
            edge_to &edge = edges[edge_counters->at(position_to_array(u))];
            edge._v = v;
            edge._w = w;
            edge_counters->at(position_to_array(u)) -= 1;
            i++;
        }
    }

    i = 0;
    while (i < nodes_count) {
        nodes[i]._edges_storage = &edges[edge_counters->at(i) + 1];
        if (i == nodes_count - 1)
            nodes[i]._edges_count = edges.size() - 1 - edge_counters->at(i);
        else
            nodes[i]._edges_count = edge_counters->at(i + 1) - edge_counters->at(i);
        i++;
    }

    delete edge_counters;
}

uint32_t local::DIMACS_graph::position_to_array(uint32_t position) {
    return position - 1;
}

std::ostream &operator<<(std::ostream &os, const local::edge_to &edgeTo) {
    os << "{\n\"v\" : " << edgeTo._v << ",\n";
    os << "\"w\" : " << edgeTo._w << "\n";
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const local::vertex &dt) {
    os << "{\n \"edge_count\" : " << dt._edges_count;
    if(dt._edges_count > 0){
        os << "\n," << dt._edges_storage[0];
        for (int i = 1; i < dt._edges_count; i++) {
            os << ",\n" << dt._edges_storage[i];
        }
    }
    os << "\n}";
    return os;
}

uint32_t local::DIMACS_graph::shortest_path_length(uint32_t u, uint32_t v, uint32_t k) {
    MinKHeap<vertex> distance_heap(k);
    for (auto &item: nodes) {
        item.key = ~0;
        item._visited = false;
    }
    nodes[position_to_array(u)].key = 0;
    distance_heap.insert(&nodes[position_to_array(u)]);
    while (!distance_heap.is_empty() && !nodes[position_to_array(v)]._visited) {
        vertex *current_node = distance_heap.pop_min();
        current_node->_visited = true;
        for (int i = 0; i < current_node->_edges_count; i++) {
            edge_to *edge_to_neighbour = &current_node->_edges_storage[i];
            vertex *neighbour = &nodes[position_to_array(edge_to_neighbour->_v)];
            if (!neighbour->_visited) {
                if (neighbour->key == ~0) {
                    neighbour->key = current_node->key + edge_to_neighbour->_w;
                    distance_heap.insert(neighbour);
                } else if (current_node->key + edge_to_neighbour->_w < neighbour->key) {
                    neighbour->key = current_node->key + edge_to_neighbour->_w;
                    distance_heap.update(neighbour);
                }
            }
        }
    }
    return nodes[position_to_array(v)].key;
}

local::vertex::vertex(edge_to *edges, uint32_t out_edges) : _edges_storage(edges), _edges_count(out_edges) {}


#include <graph.hpp>
#include <sstream>
#include <iostream>
#include <fstream>

local::DIMACS_graph::DIMACS_graph(std::string input_file_path, uint32_t heap_arity) {
    std::fstream in(input_file_path);
    std::string line, dummy;

    while (line.substr(0, 4) != "p sp")
        getline(in, line);

    std::stringstream linestr;

    linestr.str(line);
    linestr >> dummy >> dummy >> nodes_count >> edges_count;

    nodes.resize(nodes_count);
    edges.resize(nodes_count);

    uint32_t i = 0;

    while (i < edges_count) {
        getline(in, line);
        if (line.substr(0, 2) == "a ") {
            std::stringstream arc(line);
            uint32_t u, v, w;
            char _;
            arc >> _ >> u >> v >> w;
            edges[position_to_array(u)].emplace_back(v, w);
            i++;
        }
    }

    i = 0;
    while (i < nodes_count) {
        nodes[i]._edges = &edges[i];
        i++;
    }

    distance_heap = new MinKHeap<vertex>(nodes, heap_arity);
}

uint32_t local::DIMACS_graph::position_to_array(uint32_t position) {
    return position - 1;
}

local::vertex::vertex(std::vector<edge_to> *edges) : _edges(edges) {}


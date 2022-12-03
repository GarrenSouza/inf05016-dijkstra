
#include <iostream>
#include <chrono>

#include <graph.hpp>

uint64_t bytes_allocated = 0;
uint64_t bytes_freed = 0;

void *operator new(size_t size) {
    bytes_allocated += size;
    return malloc(size);
}

void operator delete(void *memory, size_t size) {
    bytes_freed += size;
    free(memory);
}

struct value : public local::HeapNode {
    explicit value(uint64_t v) : local::HeapNode() {
        key = v;
    }

    friend std::ostream &operator<<(std::ostream &os, const value &dt);
};

std::ostream &operator<<(std::ostream &os, const value &dt) {
    os << "value { " << dt.key << " at " << dt.position << " }";
    return os;
}

int main() {
    std::string file_path("/home/gsouza/Documents/UFRGS/inf05016-dijkstra/data/USA-road-d.USA.gr");
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    local::DIMACS_graph g(file_path, 2);
    std::cout << "It took " << (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - start)).count()
              << " milliseconds to read the graph" << std::endl;
//    value a(896);
//    value b(256);
//    value c(3);
//    value d(12);
//    value e(145);
//
//    value f(1);
//
//    std::vector<value> collection = {a, b, c, d, e};
//    local::MinKHeap<value> heap(collection, 3);
//    heap.make_heap();
//    heap.insert(&f);
//    collection[0].key = 2;
//    heap.update(&collection[0]);
//    std::cout << heap << std::endl;

    std::cout << "Memory Allocated: " << bytes_allocated << " bytes" << std::endl;
    std::cout << "Memory Freed: " << bytes_freed << " bytes" << std::endl;
    std::cout << "Current in use: " << bytes_allocated - bytes_freed << " bytes" << std::endl;
    return 0;
}

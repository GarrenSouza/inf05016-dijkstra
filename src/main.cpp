
#include <iostream>

#include <heap.hpp>

struct value {
    int _v;

    value(int v) : _v(v) {}

    bool operator>(const value &other) {
        return _v > other._v;
    }

    friend std::ostream &operator<<(std::ostream &os, const value &dt);
};

std::ostream &operator<<(std::ostream &os, const value &dt) {
    os << "value { " << dt._v << " }";
    return os;
}

int main() {
    value a(896);
    value b(256);
    value c(-10);
    value d(12);
    value e(145);

    value f(-20);

    std::vector<value> collection = {a, b, c, d, e};
    local::MinKHeap<value> heap(collection, 3);
    heap.insert(&f);
    for (auto &item: collection) std::cout << item._v << " ";
    std::cout << std::endl;
    std::cout << heap << std::endl;
    return 0;
}

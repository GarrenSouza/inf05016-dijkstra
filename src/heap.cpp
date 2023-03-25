
#include "heap.hpp"

namespace local {

    bool HeapNode::operator>(const HeapNode &other) {
        return key > other.key;
    }
}


#pragma once

#include <vector>
#include <cstdint>
#include <ostream>

namespace local {

    /**
     * Heap node model used in the current MinKHeap, every type T that uses the heap must inherit from HeapNode
     */
    struct HeapNode {
        uint32_t key;
        uint32_t position;

        HeapNode() : key(~0), position(0) {}

        bool operator>(const HeapNode &other);
    };

    /**
     * K-ary min heap that deals only with pointers to instances of type T (for performance reasons)
     * @tparam T The type of the underlying data to be organized into a (min) heap. The Heapify step is implemented as in:
     * https://core.ac.uk/download/pdf/82135122.pdf.
     */
    template<typename T>
    class MinKHeap {
        std::vector<HeapNode *> _references;
        std::vector<T> &_data;
        uint32_t _next_available_slot;
        uint32_t _k;
        uint32_t _size, _capacity;

        void AHU_make_heap(uint32_t last_item_position);

        void sift_up(int32_t node_index);

        void sift_down(int32_t node_index);

        void interchange(int32_t a, int32_t b);

        int32_t get_smallest_child_index(int32_t parent_index);

        int32_t heap_2_array_position(int32_t heap_index);

        int32_t array_2_heap_position(int32_t array_index);

    public:
        /**
         * Initializes an instance of MinKHeap. Notice that the structure deals exclusively with pointers to the underlying
         * data, so make sure your underlying *data* is safe while working with this heap implementation. This holds for
         * objects inserted later on and objects popped out from the structure. It deals with pointers and pointers only.
         *
         * Obs: The underlying data must support the ">" operator.
         *_size
         * @param data The underlying data container from where the pointers are going to be derived
         * @param k The degree for each node
         */
        MinKHeap(std::vector<T> &data, uint32_t k);

        /**
         * Inserts the item into the heap in O(log k)
         * @param item_ptr Pointer to the data to be included in the heap
         */
        void insert(T *item_ptr);

        /**
         * Updates the heap to reflect changes in a given heap node
         * @param item_ptr Pointer to the item that has been updated
         */
        void update(T *item_ptr);

        /**
         * Returns a pointer to the minimum value in the underlying data in O(1)
         * @return Pointer to the minimum value in the heap
         */
        T *get_min();

        /**
         * Pops AND Returns a pointer to the minimum value in the underlying data, making sure the heap invariant applies.
         * Runs in O(log k)
         * @return Pointer to the minimum value in the heap
         */
        T *pop_min();

        void make_heap();

        void make_heap(uint32_t last_item_position);

        friend std::ostream &operator<<(std::ostream &os, const MinKHeap<T> &dt) {
            os << "MinHeap { ";
            os << "k: " << dt._k << ", ";
            os << "size: " << dt._size << ", ";
            os << "values: { ";
            for (int i = 0; i < dt._size - 1; i++)
                os << *static_cast<T *>(dt._references[i]) << ", ";
            if (dt._size > 0) os << *static_cast<T *>(dt._references[dt._size - 1]);
            os << " } }";
            return os;
        }
    };

    template<typename T>
    void MinKHeap<T>::update(T *item_ptr) {
        auto *heap_node = static_cast<HeapNode *>(item_ptr);
        if (heap_node->position < 1 || heap_node->position > _size)
            throw std::runtime_error("@MinKHeap<T>::update | trying to update heap with node at invalid position");
        uint32_t position = heap_2_array_position(heap_node->position);
        sift_up(position);
        sift_down(position);
    }

    template<typename T>
    void MinKHeap<T>::make_heap(uint32_t last_item_position) {
        AHU_make_heap(last_item_position);
    }

    template<typename T>
    void MinKHeap<T>::make_heap() {
        AHU_make_heap(_size);
    }

    template<typename T>
    MinKHeap<T>::MinKHeap(std::vector<T> &data, uint32_t k) : _data(data), _k(k) {
        static_assert(std::is_base_of<HeapNode, T>::value, "@MinKHeap<T>::MinKHeap | T is not derived of HeapNode");
        _references.resize(data.size());
        _size = _references.size();
        _capacity = _size;

        for (int i = 0; i < _capacity; i++) {
            _references[i] = static_cast<HeapNode *>(&data[i]);
            _references[i]->position = array_2_heap_position(i);
        }

        _references.push_back(nullptr);
        _next_available_slot = _references.size() - 1;
    }

    template<typename T>
    inline int32_t MinKHeap<T>::array_2_heap_position(int32_t array_index) {
        return array_index + 1;
    }

    template<typename T>
    inline int32_t MinKHeap<T>::heap_2_array_position(int32_t heap_index) {
        return heap_index - 1;
    }

    template<typename T>
    int32_t MinKHeap<T>::get_smallest_child_index(int32_t parent_index) {
        int32_t first_child_index = heap_2_array_position(array_2_heap_position(parent_index) * _k - (_k - 2));
        int32_t smallest_child_index = parent_index;
        for (int32_t j = first_child_index; j < first_child_index + _k && j < _size && _references[j] != nullptr; j++)
            if ((*_references[smallest_child_index]) > (*_references[j]))
                smallest_child_index = j;
        return (smallest_child_index == parent_index) ?
               -1 : smallest_child_index;
    }

    template<typename T>
    inline void MinKHeap<T>::interchange(int32_t a, int32_t b) {
        HeapNode *aux = _references[a];
        _references[a] = _references[b];
        _references[b] = aux;

        uint32_t aux_position = _references[a]->position;
        _references[a]->position = _references[b]->position;
        _references[b]->position = aux_position;
    }

    template<typename T>
    T *MinKHeap<T>::pop_min() {
        if (_size > 0) {
            HeapNode *min = _references[0];
            _references[0] = _references[_size - 1];
            _references[_size - 1] = nullptr;
            if (_references[0]) {
                _references[0]->position = 1;
                sift_down(0);
            }
            _size -= 1;

            min->position = 0;
            return static_cast<T *>(min);
        } else throw std::runtime_error("MinKHeap::pop_min -> Heap is empty");
    }

    //(Aho, Hopcroft, and Ullman, 1974)
    template<typename T>
    void MinKHeap<T>::AHU_make_heap(uint32_t last_item_position) {
        int first_non_leaf_index;
        int mod_over_k = _size % _k;

        /**
         *
         * Founding Child: The child from a node r in a position p such that p = r*k (for a k-ary heap)
         *
         * Founding children are important in in-place heap storages like this one where the references are stored in
         * a contiguous manner. In degrees greater than 2 there must be siblings to the left of the founding child, take
         * a k=3 (min) heap for example:
         *
         * 1 <-root
         * 2 (3) 4
         * 5 6 7
         *
         * In this case 3 is the founding child of 1, and 2 is to the left of it (something that does not happen with k=2)
         *
         *
         * For a position n it holds that:
         *
         * # In the case n % k is at most 1
         *   The last element in the data (rightmost element) is
         *     1) a founding child
         *     2) 1 position to the right of a founding child (his sibling)
         *   In that case truncating the division over k always gives us the floor
         *   indicating where the rightmost non-leaf node is
         *
         * # Every other (reasonable) case
         *   The last element in the data (rightmost element) is to the left of the "virtual non-existing" founding child,
         *   so we find where the founding child would be in order to locate the rightmost non-leaf node (its parent)
         */

        if (mod_over_k <= 1)
            first_non_leaf_index = heap_2_array_position(last_item_position / _k);
        else
            first_non_leaf_index = heap_2_array_position((last_item_position + (_k - mod_over_k)) / _k);

        for (int32_t i = first_non_leaf_index;
             i >= heap_2_array_position(1);
             i--)
            sift_down(i);
    }

    template<typename T>
    T *MinKHeap<T>::get_min() {
        if (_size > 0) return static_cast<T *>(_references[0]);
        else throw std::runtime_error("@MinKHeap::get_min | Heap is empty");
    }

    template<typename T>
    void MinKHeap<T>::insert(T *item_ptr) {
        _references[_next_available_slot] = static_cast<HeapNode *>(item_ptr);
        _references[_next_available_slot]->position = _next_available_slot + 1;
        _size += 1;

        sift_up(_next_available_slot);

        _references.push_back(nullptr);
        _next_available_slot += 1;
    }

    template<typename T>
    void MinKHeap<T>::sift_up(int32_t node_index) {
        if (node_index != 0) {
            int parent_index;

            node_index = array_2_heap_position(node_index);
            int mod_over_k = node_index % _k;

            if (mod_over_k <= 1)
                parent_index = heap_2_array_position(node_index / _k);
            else
                parent_index = heap_2_array_position((node_index + (_k - mod_over_k)) / _k);
            node_index = heap_2_array_position(node_index);

            if (*_references[parent_index] > *_references[node_index]) {
                interchange(node_index, parent_index);
                sift_up(parent_index);
            }
        }
    }

    //(Aho, Hopcroft, and Ullman, 1974)
    template<typename T>
    void MinKHeap<T>::sift_down(int32_t node_index) {
        int32_t smallest_child_index = get_smallest_child_index(node_index);
        if (smallest_child_index != -1) {
            interchange(node_index, smallest_child_index);
            sift_down(smallest_child_index);
        }
    }
}

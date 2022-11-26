
#include <vector>
#include <cstdint>

namespace local {

    /**
     * K-ary min heap that deals only with pointers to instances of type T (for performance reasons)
     * @tparam T The type of the underlying data to be organized into a (min) heap. The Heapify step is implemented as in:
     * https://core.ac.uk/download/pdf/82135122.pdf.
     */
    template<typename T>
    class MinKHeap {
        std::vector<T *> _storage;
        int _next_available_slot;
        int _k;
        int _size;

        void AHU_make_heap();

        void sift_up(int64_t node_index);

        void sift_down(int64_t node_index);

        void interchange(int64_t a, int64_t b);

        int64_t get_smallest_child_index(int64_t parent_index);

        int64_t heap_2_array_position(int64_t heap_index);

        int64_t array_2_heap_position(int64_t array_index);

    public:
        /**
         * Initializes an instance of MinKHeap. Notice that the structure deals exclusively with pointers to the underlying
         * data, so make sure your underlying *data* is safe while working with this heap implementation. This holds for
         * objects inserted later on and objects popped out from the structure. It deals with pointers and pointers only.
         *
         * Obs: The underlying data must support the ">" operator.
         *
         * @param data The underlying data container from where the pointers are going to be derived
         * @param k The degree for each node
         */
        MinKHeap(std::vector<T> &data, int k);

        /**
         * Inserts the item into the heap in O(log k)
         * @param item_ptr Pointer to the data to be included in the heap
         */
        void insert(T *item_ptr);

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

        friend std::ostream &operator<<(std::ostream &os, const MinKHeap<T> &dt) {
            os << "MinHeap { ";
            os << "k: " << dt._k << ", ";
            os << "size: " << dt._size << ", ";
            os << "values: { ";
            for (int i = 0; i < dt._size - 1; i++)
                os << *dt._storage[i] << ", ";
            if (dt._size > 0) os << *dt._storage[dt._size - 1];
            os << "} }";
            return os;
        }
    };

    template<typename T>
    MinKHeap<T>::MinKHeap(std::vector<T> &data, int k) : _k(k) {
        _storage.resize(data.size());
        for (int i = 0; i < _storage.size(); i++)
            _storage[i] = &data[i];

        _size = _storage.size();

        _storage.push_back(nullptr);
        _next_available_slot = _storage.size() - 1;

        AHU_make_heap();
    }

    template<typename T>
    inline int64_t MinKHeap<T>::array_2_heap_position(int64_t array_index) {
        return array_index + 1;
    }

    template<typename T>
    inline int64_t MinKHeap<T>::heap_2_array_position(int64_t heap_index) {
        return heap_index - 1;
    }

    template<typename T>
    int64_t MinKHeap<T>::get_smallest_child_index(int64_t parent_index) {
        int64_t first_child_index = heap_2_array_position(array_2_heap_position(parent_index) * _k - (_k - 2));
        int64_t smallest_child_index = parent_index;
        for (int64_t j = first_child_index; j < first_child_index + _k && j < _size && _storage[j] != nullptr; j++)
            if ((*_storage[smallest_child_index]) > (*_storage[j]))
                smallest_child_index = j;
        return (smallest_child_index == parent_index) ?
               -1 : smallest_child_index;
    }

    template<typename T>
    inline void MinKHeap<T>::interchange(int64_t a, int64_t b) {
        T *aux = _storage[a];
        _storage[a] = _storage[b];
        _storage[b] = aux;
    }

    template<typename T>
    T *MinKHeap<T>::pop_min() {
        if (_size > 0) {
            T *min = _storage[0];
            _storage[0] = _storage[_size - 1];
            _storage[_size - 1] = nullptr;
            sift_down(0);
            _size -= 1;

            return min;
        } else throw std::runtime_error("MinKHeap::pop_min -> Heap is empty");
    }

    //(Aho, Hopcroft, and Ullman, 1974)
    template<typename T>
    void MinKHeap<T>::AHU_make_heap() {
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
            first_non_leaf_index = heap_2_array_position(_size / _k);
        else
            first_non_leaf_index = heap_2_array_position((_size + (_k - mod_over_k)) / _k);

        for (int64_t i = first_non_leaf_index;
             i >= heap_2_array_position(1);
             i--)
            sift_down(i);
    }

    template<typename T>
    T *MinKHeap<T>::get_min() {
        if (_size > 0) return _storage[0];
        else throw std::runtime_error("MinKHeap::get_min -> Heap is empty");
    }

    template<typename T>
    void MinKHeap<T>::insert(T *item_ptr) {
        _storage[_next_available_slot] = item_ptr;
        _size += 1;

        sift_up(_next_available_slot);

        _storage.push_back(nullptr);
        _next_available_slot += 1;
    }

    template<typename T>
    void MinKHeap<T>::sift_up(int64_t node_index) {
        if (node_index != 0) {
            int parent_index;

            node_index = array_2_heap_position(node_index);
            int mod_over_k = node_index % _k;

            if (mod_over_k <= 1)
                parent_index = heap_2_array_position(node_index / _k);
            else
                parent_index = heap_2_array_position((node_index + (_k - mod_over_k)) / _k);
            node_index = heap_2_array_position(node_index);

            if (_storage[parent_index] > _storage[node_index]) {
                interchange(node_index, parent_index);
                sift_up(parent_index);
            }
        }
    }

    //(Aho, Hopcroft, and Ullman, 1974)
    template<typename T>
    void MinKHeap<T>::sift_down(int64_t node_index) {
        int64_t smallest_child_index = get_smallest_child_index(node_index);
        if (smallest_child_index != -1) {
            interchange(node_index, smallest_child_index);
            sift_down(smallest_child_index);
        }
    }
}

#ifndef REFERABLE_ARRAY_H
#define REFERABLE_ARRAY_H

#include "sk_utility.h"

namespace sk {

namespace detail {

/*
 * array_node must be a POD type, otherwise, GCC will complaint:
 *     error: (perhaps the 'offsetof' macro was used incorrectly)
 * in function referable_arry::index(...)
 * so, do NOT define any ctor/dtor for it.
 */
template<typename T>
struct array_node {
    bool used;
    char data[sizeof(T)];
};

} // namespace detail

/*
 * referable_array is an array with fixed size, the difference between fixed_vector
 * and referable_array is: the element index in the former one may change, while it
 * will not change in the latter one
 *
 * so, if there may be references between these elements, use the latter one.
 * this container is used to implement fixed_list, fixed_set, fixed_map, etc.
 */
template<typename T, size_t N>
struct referable_array {
    static_assert(sizeof(T) >= sizeof(size_t), "type size not enough");

    typedef detail::array_node<T> node;
    static const size_t npos = static_cast<size_t>(-1);

    size_t used_count;
    size_t free_head;
    char memory[sizeof(node) * N];

    referable_array() { }
    ~referable_array() { clear(); }

    /*
     * Note: we cannot just put the nodes in array into this container, as this
     * is a referable container, the data type T may contains index reference,
     * so we MUST keep the index unchanged, so does the assignment operator.
     */
    referable_array(const referable_array& array) { __copy(array); }

    referable_array& operator=(const referable_array& array) {
        if (this == &array)
            return *this;

        clear();
        __copy(array);
        return *this;
    }

	void init() {
		used_count = 0;
		free_head = 0;
		__init();
	}

    void uninit() { clear(); }

    void __init() {
        // link the free slots
        for (size_t i = 0; i < N; ++i) {
            node *n = __at(i);
            new (n) node();
            n->used = false;
            *cast_ptr(size_t, n->data) = (i == N - 1) ? npos : i + 1;
        }
    }

    void __copy(const referable_array& array) {
        for (size_t i = 0; i < array.capacity(); ++i) {
            const node *on = array.__at(i); // old node
            node *nn = __at(i);             // new node

            if (on->used) {
                nn->used = true;
                const T* ot = cast_ptr(T, const_cast<char*>(on->data)); // old T
                T* nt = cast_ptr(T, nn->data);                          // new T
                new (nt) T(*ot);
            } else {
                nn->used = false;
                const size_t* oi = cast_ptr(size_t, const_cast<char*>(on->data)); // old index
                size_t* ni = cast_ptr(size_t, nn->data);                          // new index
                *ni = *oi;
            }
        }

        used_count = array.used_count;
        free_head = array.free_head;
    }

    node *__at(size_t index) {
        return cast_ptr(node, memory) + index;
    }

    const node *__at(size_t index) const {
        return cast_ptr(node, const_cast<char*>(memory)) + index;
    }

    /*
     * Note: this function does NOT check whether the element's
     * destructor has been called, the caller should do the job
     */
    void __add_free(size_t index) {
        node *n = __at(index);
        n->used = false;
        *cast_ptr(size_t, n->data) = free_head;

        free_head = index;
        --used_count;
    }

    /*
     * Note: this function does NOT check whether the element's
     * constructor has been called, the caller should do the job
     */
    void __add_used(size_t index) {
        __at(index)->used = true;
        ++used_count;
    }

    size_t size()     const { return used_count; }
    size_t capacity() const { return N; }

    bool full()  const {
        if (used_count >= N) {
            LOG_CHECK_ERROR(free_head == npos);
            return true;
        }
        return false;
    }

    bool empty() const {
        if (used_count <= 0) {
            LOG_CHECK_RETVAL(free_head != npos, false);
            return true;
        }
        return false;
    }

    void clear() {
        for (size_t i = 0; i < N; ++i) {
            node *n = __at(i);
            if (!n->used)
                continue;

            T *t = cast_ptr(T, n->data);
            t->~T();
            __add_free(i);
        }
    }

    T *at(size_t index) {
        LOG_CHECK_RETVAL(index < N, NULL);

        node *n = __at(index);
        if (n->used)
            return cast_ptr(T, n->data);

        return NULL;
    }

    const T *at(size_t index) const {
        LOG_CHECK_RETVAL(index < N, NULL);

        const node *n = __at(index);
        if (n->used)
            return cast_ptr(T, const_cast<char*>(n->data));

        return NULL;
    }

    void erase(size_t index) {
        LOG_CHECK_RETNONE(index < N);

        node *n = __at(index);
        if (!n->used)
            return;

        T *t = cast_ptr(T, n->data);
        t->~T();
        __add_free(index);
    }

    template<typename... Args>
    pair<T*, size_t> emplace(Args&&... args) {
        size_t _npos = npos;
        pair<T*, size_t> p(NULL, _npos);

        if (full())
            return p;

        LOG_CHECK_RETVAL(free_head != npos, p);

        node *n = __at(free_head);
        p.second = free_head;
        __add_used(free_head);
        free_head = *cast_ptr(size_t, n->data);

        T *t = cast_ptr(T, n->data);
        new (t) T(std::forward<Args>(args)...);
        p.first = t;

        return p;
    }

    size_t index(const T *t) const {
        LOG_CHECK_RETVAL(t, npos);

        node *n = cast_ptr(node, char_ptr(const_cast<T*>(t)) - offsetof(node, data));
        LOG_CHECK_ERROR(n->used);

        size_t offset = char_ptr(n) - memory;
        //LOG_CHECK_ERROR(offset % sizeof(node) == 0);

        size_t index = offset / sizeof(node);
        LOG_CHECK_RETVAL(index < N, npos);

        return index;
    }
};

} // namespace sk

#endif // REFERABLE_ARRAY_H

#ifndef FIXED_LIST_H
#define FIXED_LIST_H

#include <iterator>
#include "container/referable_array.h"

namespace sk {
namespace detail {

template<typename T>
struct list_node {
    static const size_t npos = static_cast<size_t>(-1);

    size_t next;
    size_t prev;
    T      data;

    list_node(const T& t) : next(npos), prev(npos), data(t) {}
    list_node() : next(npos), prev(npos) {}
};

/*
 * Note:
 *   1. T should be a fixed_list type here
 *   2. C stands for constness, true makes this iterator a const iterator
 */
template<typename T, bool C>
struct list_iterator {
    typedef T                                                     list_type;
    typedef typename T::node                                      node_type;
    typedef typename T::value_type                                value_type;
    typedef ptrdiff_t                                             difference_type;
    typedef list_iterator<T, C>                                   self;
    typedef std::bidirectional_iterator_tag                       iterator_category;
    typedef typename if_<C, const list_type*, list_type*>::type   list_pointer;
    typedef typename if_<C, const node_type*, node_type*>::type   node_pointer;
    typedef typename if_<C, const value_type*, value_type*>::type pointer;
    typedef typename if_<C, const value_type&, value_type&>::type reference;

    list_pointer l;
    node_pointer n;

    explicit list_iterator(list_pointer l) : l(l), n(NULL) {}
    list_iterator(list_pointer l, node_pointer n) : l(l), n(n) {}

    /*
     * make a templated copy constructor here to support construction of
     * const iterator from mutable iterator
     */
    template<bool B>
    list_iterator(const list_iterator<T, B>& s) : l(s.l), n(s.n) {}

    self& operator=(const self& s) {
        if (this == &s)
            return *this;

        l = s.l;
        n = s.n;

        return *this;
    }

    reference operator*() const { return n->data; }
    pointer operator->() const { return &n->data; }

    self& operator++()   { n = l->__next(n); return *this; }
    self operator++(int) { self tmp(*this); n = l->__next(n); return tmp; }
    self& operator--()   { n = l->__prev(n); return *this; }
    self operator--(int) { self tmp(*this); n = l->__prev(n); return tmp; }

    bool operator==(const self& x) const { return n == x.n && l == x.l; }
    bool operator!=(const self& x) const { return !(*this == x); }

    bool operator==(const list_iterator<T, !C>& x) const { return n == x.n && l == x.l; }
    bool operator!=(const list_iterator<T, !C>& x) const { return !(*this == x); }
};

} // namespace detail

template<typename T, size_t N>
struct fixed_list {
    typedef T                                  value_type;
    typedef detail::list_node<value_type>      node;
    typedef referable_array<node, N>           impl_type;
    typedef fixed_list<T, N>                   self;
    typedef detail::list_iterator<self, false> iterator;
    typedef detail::list_iterator<self, true>  const_iterator;

    static const size_t npos = node::npos;

    impl_type list;
    size_t head;
    size_t tail;

    fixed_list() {}
    ~fixed_list() { }

    fixed_list(const fixed_list& list) : list(list.list), head(list.head), tail(list.tail) {}

    fixed_list& operator=(const fixed_list& list) {
        if (this == &list)
            return *this;

        this->list = list.list;
        head = list.head;
        tail = list.tail;
        return *this;
    }

    node *__construct(const T& t, size_t& index) {
        pair<node*, size_t> p = list.emplace(t);
        index = p.second;
        return p.first;
    }

    void __destroy(size_t index) {
        if (index == npos)
            return;

        list.erase(index);
    }

    node *__node(size_t index) {
        return index == npos ? NULL : list.at(index);
    }

    const node *__node(size_t index) const {
        return index == npos ? NULL : list.at(index);
    }

    node *__prev(node *n) {
        if (!n)
            return NULL;

        return n->prev == npos ? NULL : __node(n->prev);
    }

    const node *__prev(const node *n) const {
        if (!n)
            return NULL;

        return n->prev == npos ? NULL : __node(n->prev);
    }

    node *__next(node *n) {
        if (!n)
            return NULL;

        return n->next == npos ? NULL : __node(n->next);
    }

    const node *__next(const node *n) const {
        if (!n)
            return NULL;

        return n->next == npos ? NULL : __node(n->next);
    }

    node *__head() {
        return head == npos ? NULL : __node(head);
    }

    const node *__head() const {
        return head == npos ? NULL : __node(head);
    }

    node *__tail() {
        return tail == npos ? NULL : __node(tail);
    }

    const node *__tail() const {
        return tail == npos ? NULL : __node(tail);
    }

    size_t __index(const node *n) const {
        return n ? list.index(n) : npos;
    }

    bool empty() const { return list.empty(); }

    bool full()  const { return list.full(); }

    size_t size() const { return list.size(); }

    size_t capacity() const { return list.capacity(); }

    size_t index(const T *t) const {
        node *n = cast_ptr(node, char_ptr(const_cast<T*>(t)) - offsetof(node, data));
        return __index(n);
    }

    T *front() {
        return empty() ? NULL : &list.at(head)->data;
    }

    const T *front() const {
        return empty() ? NULL : &list.at(head)->data;
    }

    T *back() {
        return empty() ? NULL : &list.at(tail)->data;
    }

    const T *back() const {
        return empty() ? NULL : &list.at(tail)->data;
    }

    iterator at(size_t index) {
        node *n = list.at(index);
        return n ? iterator(this, n) : end();
    }

    const_iterator at(size_t index) const {
        node *n = list.at(index);
        return n ? const_iterator(this, n) : end();
    }

	void init() { list.init(); head = tail = npos; }
    void uninit() { clear(); }

	void clear() { list.clear(); head = tail = npos; }

    int insert(iterator it, const T& t) {
        if (full())
            return -ENOMEM;

        node *old_node = it.n;
        if (!old_node) // empty node, it must be the end() iterator
            return push_back(t);

        size_t old_idx = __index(old_node);

        size_t new_idx;
        node *new_node = __construct(t, new_idx);
        LOG_CHECK_RETVAL(new_node, -EFAULT);

        new_node->prev = old_node->prev;
        new_node->next = old_idx;

        node *prev = __prev(old_node);
        if (!prev)
            head = new_idx;
        else {
            LOG_CHECK_ERROR(prev->next == old_idx);
            prev->next = new_idx;
        }
        old_node->prev = new_idx;

        return 0;
    }

    void __erase(node *n) {
        LOG_CHECK_RETNONE(n);

        size_t idx = __index(n);

        node *prev = __prev(n);
        node *next = __next(n);

        if (prev)
            prev->next = n->next;
        else {
            LOG_CHECK_ERROR(head == idx);
            head = n->next;
        }

        if (next)
            next->prev = n->prev;
        else {
            LOG_CHECK_ERROR(tail == idx);
            tail = n->prev;
        }

        __destroy(idx);
    }

    iterator erase(iterator it) {
        if (empty() || !it.n)
            return end();

        node *next = __next(it.n);
        __erase(it.n);

        // no matter next is null or not, this will always work
        return iterator(this, next);
    }

    iterator erase_at(size_t index) {
        iterator itr = at(index);
        LOG_CHECK_RETVAL(itr != end(), end());

        erase(itr);
    }

    iterator find(const T& value) {
        for (auto it = this->begin(); it != this->end(); ++it) {
            if (*it == value)
                return it;
        }

        return this->end();
    }

    template<typename Pred>
    iterator find_if(Pred p) {
        for (auto it = this->begin(); it != this->end(); ++it) {
            if (p(*it))
                return it;
        }

        return this->end();
    }

    T* emplace_back() {
        if (full())
            return NULL;

        pair<node*, size_t> p = list.emplace();
        size_t idx = p.second;
        node *n = p.first;
        LOG_CHECK_RETVAL(n, NULL);

        n->prev = tail;
        n->next = npos;

        node *back = __tail();
        if (back)
            back->next = idx;

        tail = idx;

        if (head == npos)
            head = idx;

        return &n->data;
    }

    int force_push_back(const T& t) {
        if (full()) pop_front();
        return push_back(t);
    }

    int push_back(const T& t) {
        if (full())
            return -ENOMEM;

        size_t idx;
        node *n = __construct(t, idx);
        LOG_CHECK_RETVAL(n, -EFAULT);

        n->prev = tail;
        n->next = npos;

        node *back = __tail();
        if (back)
            back->next = idx;

        tail = idx;

        if (head == npos)
            head = idx;

        return idx;
    }

    void pop_back() {
        if (!empty())
            __erase(__tail());
    }

    int push_front(const T& t) {
        if (full())
            return -ENOMEM;

        size_t idx;
        node *n = __construct(t, idx);
        LOG_CHECK_RETVAL(n, -EFAULT);

        n->prev = npos;
        n->next = head;

        node *front = __head();
        if (front)
            front->prev = idx;

        head = idx;

        if (tail == npos)
            tail = idx;

        return idx;
    }

    void pop_front() {
        if (!empty())
            __erase(__head());
    }

    iterator begin() { return iterator(this, __head()); }
    iterator end()   { return iterator(this); }
	iterator rbegin() { return iterator(this); }
	iterator rend() { return iterator(this, __tail()); }
    const_iterator begin() const { return const_iterator(this, __head()); }
    const_iterator end() const   { return const_iterator(this); }
};

} // namespace sk

#endif // FIXED_LIST_H

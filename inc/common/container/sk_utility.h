#ifndef UTILITY_H
#define UTILITY_H

namespace sk {
    /*
    * like std::pair, used for map/rbtree to represent value type
    */
    template<typename T1, typename T2>
    struct pair {
        typedef T1 first_type;
        typedef T2 second_type;

        T1 first;
        T2 second;

        pair() : first(), second() {}
        pair(const T1& first, const T2& second) : first(first), second(second) {}
    };

    /*
    * single type selector, used for set/rbtree to extract key from value
    */
    template<typename T>
    struct identity {
        typedef T result_type;

        result_type& operator()(T& t) const { return t; }
        const result_type& operator()(const T& t) const { return t; }
    };

    /*
    * double types selector, select first type, used for map/rbtree to
    * extract key from key-value pair, P should be a sk::pair
    */
    template<typename P>
    struct select1st {
        typedef typename P::first_type result_type;

        result_type& operator()(P& p) const { return p.first; }
        const result_type& operator()(const P& p) const { return p.first; }
    };

    /*
    * double types selector, select second type
    */
    template<typename P>
    struct select2nd {
        typedef typename P::second_type result_type;

        result_type& operator()(P& p) const { return p.second; }
        const result_type& operator()(const P& p) const { return p.second; }
    };

    /*
    * type selector according to a bool value
    */
    template<bool B, typename T, typename F>
    struct if_;

    template<typename T, typename F>
    struct if_<true, T, F> {
        typedef T type;
    };

    template<typename T, typename F>
    struct if_<false, T, F> {
        typedef F type;
    };

    template<typename T, typename K = uint64_t>
    struct equip_key {
        typedef T value_type;
        typedef K key_type;

        key_type _key;

        equip_key(K key):_key(key) {}
        bool operator()(value_type& val) { return val.key() == _key; }
    };

} // namespace sk

#define cast_ptr(type, ptr) static_cast<type*>(static_cast<void*>(ptr))
#define char_ptr(ptr)       cast_ptr(char, ptr)

#define LOG_CHECK_RETNONE(exp)                     \
    do {                                        \
        if (exp) break;                         \
        LOG_CHECK_ERROR(exp);                            \
        return;                                 \
    } while (0)

#define LOG_CHECK_RETVAL(exp, val)                 \
    do {                                        \
        if (exp) break;                         \
        LOG_CHECK_ERROR(exp);                            \
        return (val);                           \
    } while (0)

#endif // UTILITY_H

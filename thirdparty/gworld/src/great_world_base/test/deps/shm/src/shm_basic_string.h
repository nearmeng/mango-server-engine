
// Components for manipulating sequences of characters -*- C++ -*-

// Copyright (C) 1997-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file bits/basic_string.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{string}
 */

//
// ISO C++ 14882: 21 Strings library
//

// codes from gcc 5.2.0 libstdc++ basic_string
// previous gcc cow string cannot support share memory
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=16612

#pragma once

#include "shm_allocator.h"
#include <string>
#include <limits>

template <typename _CharT>
class ShmBasicString {

  // Types:
 public:
  typedef std::char_traits<_CharT> traits_type;
  typedef _CharT value_type;
  typedef ShmAllocator<_CharT> allocator_type;
  typedef typename allocator_type::size_type size_type;
  typedef typename allocator_type::difference_type difference_type;
  typedef typename allocator_type::reference reference;
  typedef typename allocator_type::const_reference const_reference;
  typedef typename allocator_type::pointer pointer;
  typedef typename allocator_type::const_pointer const_pointer;
  typedef pointer iterator;
  typedef const_pointer const_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;

  ///  Value returned by various member functions when they fail.
  static const size_type npos = static_cast<size_type>(-1);

 private:


  // Use empty-base optimization: http://www.cantrip.org/emptyopt.html
  struct _Alloc_hider : allocator_type  // TODO check _is_final
  {
    _Alloc_hider(pointer _dat, const allocator_type& _a = allocator_type())
        : allocator_type(_a), _M_p(_dat) {}

    pointer _M_p;  // The actual data.
  };

  _Alloc_hider _M_dataplus;
  size_type _M_string_length;

  enum { _S_local_capacity = 15 / sizeof(_CharT) };

  union {
    _CharT _M_local_buf[_S_local_capacity + 1];
    size_type _M_allocated_capacity;
  };

  void _M_data(pointer _p) { _M_dataplus._M_p = _p; }

  void _M_length(size_type _length) { _M_string_length = _length; }

  pointer _M_data() const { return _M_dataplus._M_p; }

  pointer _M_local_data() {
    return std::pointer_traits<pointer>::pointer_to(*_M_local_buf);
  }

  const_pointer _M_local_data() const {
    return std::pointer_traits<const_pointer>::pointer_to(*_M_local_buf);
  }

  void _M_capacity(size_type _capacity) { _M_allocated_capacity = _capacity; }

  void _M_set_length(size_type _n) {
    _M_length(_n);
    traits_type::assign(_M_data()[_n], _CharT());
  }

  bool _M_is_local() const { return _M_data() == _M_local_data(); }

  // Create & Destroy
  pointer _M_create(size_type&, size_type);

  void _M_dispose() {
    if (!_M_is_local()) _M_destroy(_M_allocated_capacity);
  }

  void _M_destroy(size_type _size) throw() { _M_dataplus.deallocate(_M_data());
  }

  // _M_construct_aux is used to implement the 21.3.1 para 15 which
  // requires special behaviour if _InIterator is an integral type
  template <typename _InIterator>
  void _M_construct_aux(_InIterator _beg, _InIterator _end, std::false_type) {
    typedef typename std::iterator_traits<_InIterator>::iterator_category _Tag;
    _M_construct(_beg, _end, _Tag());
  }

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 438. Ambiguity in the "do the right thing" clause
  template <typename _Integer>
  void _M_construct_aux(_Integer _beg, _Integer _end, std::true_type) {
    _M_construct_aux_2(static_cast<size_type>(_beg), _end);
  }

  void _M_construct_aux_2(size_type _req, _CharT _c) { _M_construct(_req, _c); }

  template <typename _InIterator>
  void _M_construct(_InIterator _beg, _InIterator _end) {
    typedef typename std::is_integral<_InIterator> _Integral;
    _M_construct_aux(_beg, _end, _Integral());
  }

  // For Input Iterators, used in istreambuf_iterators, etc.
  template <typename _InIterator>
  void _M_construct(_InIterator _beg, _InIterator _end, std::input_iterator_tag);

  // For forward_iterators up to random_access_iterators, used for
  // string::iterator, _CharT*, etc.
  template <typename _FwdIterator>
  void _M_construct(_FwdIterator _beg, _FwdIterator _end, std::forward_iterator_tag);

  void _M_construct(size_type _req, _CharT _c);

  allocator_type& _M_get_allocator() { return _M_dataplus; }

  const allocator_type& _M_get_allocator() const { return _M_dataplus; }

 private:


  size_type _M_check(size_type _pos, const char* _s) const {

    if (_pos >= this->size()) {
        std::string error_info = "ShmBasicString::at: _pos which is " + std::to_string(_pos) +
                                    " >= this->size()  which is " + std::to_string(this->size());
      throw std::out_of_range(error_info.c_str());
    }
    return _pos;
  }

  void _M_check_length(size_type _n1, size_type _n2, const char* _s) const {
    if (this->max_size() - (this->size() - _n1) < _n2) {
      std::string error_info =
          std::string("ShmBasicString::_M_check_length fail at ") + std::string(_s);
      throw std::out_of_range(error_info.c_str());
    }
  }

  // NB: _M_limit doesn't check for a bad _pos value.
  size_type _M_limit(size_type _pos, size_type _off) const noexcept {
    const bool _testoff = _off < this->size() - _pos;
    return _testoff ? _off : this->size() - _pos;
  }

  // True if _Rep and source do not overlap.
  bool _M_disjunct(const _CharT* _s) const noexcept {
    return (std::less<const _CharT*>()(_s, _M_data()) ||
            std::less<const _CharT*>()(_M_data() + this->size(), _s));
  }

  // When _n = 1 way faster than the general multichar
  // traits_type::copy/move/assign.
  static void _S_copy(_CharT* _d, const _CharT* _s, size_type _n) {
    if (_n == 1)
      traits_type::assign(*_d, *_s);
    else
      traits_type::copy(_d, _s, _n);
  }

  static void _S_move(_CharT* _d, const _CharT* _s, size_type _n) {
    if (_n == 1)
      traits_type::assign(*_d, *_s);
    else
      traits_type::move(_d, _s, _n);
  }

  static void _S_assign(_CharT* _d, size_type _n, _CharT _c) {
    if (_n == 1)
      traits_type::assign(*_d, _c);
    else
      traits_type::assign(_d, _n, _c);
  }

  // _S_copy_chars is a separate template to permit specialization
  // to optimize for the common case of pointers as iterators.
  template <class _Iterator>
  static void _S_copy_chars(_CharT* _p, _Iterator _k1, _Iterator _k2) noexcept {
    for (; _k1 != _k2; ++_k1, ++_p) traits_type::assign(*_p, *_k1);  // These types are off.
  }




  static int _S_compare(size_type _n1, size_type _n2) noexcept {
    const difference_type _d = difference_type(_n1 - _n2);

    if (_d > std::numeric_limits<int>::max())
      return std::numeric_limits<int>::max();
    else if (_d < std::numeric_limits<int>::min())
      return std::numeric_limits<int>::min();
    else
      return int(_d);
  }

  void _M_assign(const ShmBasicString& _rcs);

  void _M_mutate(size_type _pos, size_type _len1, const _CharT* _s, size_type _len2);

  void _M_erase(size_type _pos, size_type _n);

 public:
  // Construct/copy/destroy:
  // NB: We overload ctors in some cases instead of using default
  // arguments, per 17.4.4.4 para. 2 item 2.

  /**
   *  @brief  Default constructor creates an empty string.
   */
  ShmBasicString() noexcept : _M_dataplus(_M_local_data()) { _M_set_length(0); }



  /**
   *  @brief  Construct string with copy of value of @a _str.
   *  @param  _str  Source string.
   */
  ShmBasicString(const ShmBasicString& _str)
      : _M_dataplus(_M_local_data(), _str._M_get_allocator())  // TODO A traits
  {
    _M_construct(_str._M_data(), _str._M_data() + _str.length());
  }

  /**
   *  @brief  Construct string as copy of a substring.
   *  @param  _str  Source string.
   *  @param  _pos  Index of first character to copy from.
   *  @param  _n  Number of characters to copy (default remainder).
   */
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 2402. [this constructor] shouldn't use Allocator()
  ShmBasicString(const ShmBasicString& _str, size_type _pos, size_type _n = npos)
      : _M_dataplus(_M_local_data()) {
    const _CharT* _start = _str._M_data() + _str._M_check(_pos, "ShmBasicString::ShmBasicString");
    _M_construct(_start, _start + _str._M_limit(_pos, _n));
  }



        /**
   *  @brief  Construct string initialized by a character %array.
   *  @param  _s  Source character %array.
   *  @param  _n  Number of characters to copy.
   *  @param  _a  Allocator to use (default is default allocator).
   *
   *  NB: @a _s must have at least @a _n characters, &apos;\\0&apos;
   *  has no special meaning.
   */
  ShmBasicString(const _CharT* _s, size_type _n)
      : _M_dataplus(_M_local_data()) {
    _M_construct(_s, _s + _n);
  }

  /**
   *  @brief  Construct string as copy of a C string.
   *  @param  _s  Source C string.
   *  @param  _a  Allocator to use (default is default allocator).
   */
  ShmBasicString(const _CharT* _s)
      : _M_dataplus(_M_local_data()) {
    _M_construct(_s, _s ? _s + traits_type::length(_s) : _s + npos);
  }

  /**
   *  @brief  Construct string as multiple characters.
   *  @param  _n  Number of characters.
   *  @param  _c  Character to use.
   *  @param  _a  Allocator to use (default is default allocator).
   */
  ShmBasicString(size_type _n, _CharT _c)
      : _M_dataplus(_M_local_data()) {
    _M_construct(_n, _c);
  }




  /**
   *  @brief  Move construct string.
   *  @param  _str  Source string.
   *
   *  The newly-created string contains the exact contents of @a _str.
   *  @a _str is a valid, but unspecified string.
   **/
  ShmBasicString(ShmBasicString&& _str) noexcept
      : _M_dataplus(_M_local_data(), std::move(_str._M_get_allocator())) {
    if (_str._M_is_local()) {
      traits_type::copy(_M_local_buf, _str._M_local_buf, _S_local_capacity + 1);
    } else {
      _M_data(_str._M_data());
      _M_capacity(_str._M_allocated_capacity);
    }

    // Must use _M_length() here not _M_set_length() because
    // ShmBasicStringbuf relies on writing into unallocated capacity so
    // we mess up the contents if we put a '\0' in the string.
    _M_length(_str.length());
    _str._M_data(_str._M_local_data());
    _str._M_set_length(0);
  }

  /**
   *  @brief  Construct string from an initializer %list.
   *  @param  _l  std::initializer_list of characters.
   *  @param  _a  Allocator to use (default is default allocator).
   */
  ShmBasicString(std::initializer_list<_CharT> _l)
      : _M_dataplus(_M_local_data()) {
    _M_construct(_l.begin(), _l.end());
  }




  /**
   *  @brief  Construct string as copy of a range.
   *  @param  _beg  Start of range.
   *  @param  _end  End of range.
   *  @param  _a  Allocator to use (default is default allocator).
   */


  template <typename _InputIterator>
  ShmBasicString(_InputIterator _beg, _InputIterator _end)
      : _M_dataplus(_M_local_data()) {
    _M_construct(_beg, _end);
  }

  /**
   *  @brief  Destroy the string instance.
   */
  ~ShmBasicString() { _M_dispose(); }

  /**
   *  @brief  Assign the value of @a str to this string.
   *  @param  _str  Source string.
   */
  ShmBasicString& operator=(const ShmBasicString& _str) { return this->assign(_str); }

  /**
   *  @brief  Copy contents of @a s into this string.
   *  @param  _s  Source null-terminated string.
   */
  ShmBasicString& operator=(const _CharT* _s) { return this->assign(_s); }

  /**
   *  @brief  Set value to string of length 1.
   *  @param  _c  Source character.
   *
   *  Assigning to a character makes this string length 1 and
   *  (*this)[0] == @a c.
   */
  ShmBasicString& operator=(_CharT _c) {
    this->assign(1, _c);
    return *this;
  }


  /**
   *  @brief  Move assign the value of @a str to this string.
   *  @param  _str  Source string.
   *
   *  The contents of @a str are moved into this string (without copying).
   *  @a str is a valid, but unspecified string.
   **/
  // PR 58265, this should be noexcept.
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 2063. Contradictory requirements for string move assignment
  ShmBasicString& operator=(ShmBasicString&& _str) {
    this->swap(_str);
    return *this;
  }

  /**
   *  @brief  Set value to string constructed from initializer %list.
   *  @param  _l  std::initializer_list.
   */
  ShmBasicString& operator=(std::initializer_list<_CharT> _l) {
    this->assign(_l.begin(), _l.size());
    return *this;
  }

  // Iterators:
  /**
   *  Returns a read/write iterator that points to the first character in
   *  the %string.
   */
  iterator begin() noexcept { return iterator(_M_data()); }

  /**
   *  Returns a read-only (constant) iterator that points to the first
   *  character in the %string.
   */
  const_iterator begin() const noexcept { return const_iterator(_M_data()); }

  /**
   *  Returns a read/write iterator that points one past the last
   *  character in the %string.
   */
  iterator end() noexcept { return iterator(_M_data() + this->size()); }

  /**
   *  Returns a read-only (constant) iterator that points one past the
   *  last character in the %string.
   */
  const_iterator end() const noexcept { return const_iterator(_M_data() + this->size()); }

  /**
   *  Returns a read/write reverse iterator that points to the last
   *  character in the %string.  Iteration is done in reverse element
   *  order.
   */
  reverse_iterator rbegin() noexcept { return reverse_iterator(this->end()); }

  /**
   *  Returns a read-only (constant) reverse iterator that points
   *  to the last character in the %string.  Iteration is done in
   *  reverse element order.
   */
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(this->end()); }

  /**
   *  Returns a read/write reverse iterator that points to one before the
   *  first character in the %string.  Iteration is done in reverse
   *  element order.
   */
  reverse_iterator rend() noexcept { return reverse_iterator(this->begin()); }

  /**
   *  Returns a read-only (constant) reverse iterator that points
   *  to one before the first character in the %string.  Iteration
   *  is done in reverse element order.
   */
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(this->begin()); }


  /**
   *  Returns a read-only (constant) iterator that points to the first
   *  character in the %string.
   */
  const_iterator cbegin() const noexcept { return const_iterator(this->_M_data()); }

  /**
   *  Returns a read-only (constant) iterator that points one past the
   *  last character in the %string.
   */
  const_iterator cend() const noexcept { return const_iterator(this->_M_data() + this->size()); }

  /**
   *  Returns a read-only (constant) reverse iterator that points
   *  to the last character in the %string.  Iteration is done in
   *  reverse element order.
   */
  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(this->end()); }

  /**
   *  Returns a read-only (constant) reverse iterator that points
   *  to one before the first character in the %string.  Iteration
   *  is done in reverse element order.
   */
  const_reverse_iterator crend() const noexcept { return const_reverse_iterator(this->begin()); }

 public:
  // Capacity:
  ///  Returns the number of characters in the string, not including any
  ///  null-termination.
  size_type size() const noexcept { return _M_string_length; }

  ///  Returns the number of characters in the string, not including any
  ///  null-termination.
  size_type length() const noexcept { return _M_string_length; }

  ///  Returns the size() of the largest possible %string.
  size_type max_size() const noexcept {
    return (_M_dataplus.max_size() -1) / 2;
  }

  /**
   *  @brief  Resizes the %string to the specified number of characters.
   *  @param  _n  Number of characters the %string should contain.
   *  @param  _c  Character to fill any new elements.
   *
   *  This function will %resize the %string to the specified
   *  number of characters.  If the number is smaller than the
   *  %string's current size the %string is truncated, otherwise
   *  the %string is extended and new elements are %set to @a _c.
   */
  void resize(size_type _n, _CharT _c);

  /**
   *  @brief  Resizes the %string to the specified number of characters.
   *  @param  _n  Number of characters the %string should contain.
   *
   *  This function will resize the %string to the specified length.  If
   *  the new size is smaller than the %string's current size the %string
   *  is truncated, otherwise the %string is extended and new characters
   *  are default-constructed.  For basic types such as char, this means
   *  setting them to 0.
   */
  void resize(size_type _n) { this->resize(_n, _CharT()); }

  ///  A non-binding request to reduce capacity() to size().
  void shrink_to_fit() noexcept {
    if (capacity() > size()) {
      try {
        reserve(0);
      }
      catch(...) {}
    }
  }

  /**
   *  Returns the total number of characters that the %string can hold
   *  before needing to allocate more memory.
   */
  size_type capacity() const noexcept {
    return _M_is_local() ? size_type(_S_local_capacity) : _M_allocated_capacity;
  }

  /**
   *  @brief  Attempt to preallocate enough memory for specified number of
   *          characters.
   *  @param  _res_arg  Number of characters required.
   *  @throw  std::length_error  If @a _res_arg exceeds @c max_size().
   *
   *  This function attempts to reserve enough memory for the
   *  %string to hold the specified number of characters.  If the
   *  number requested is more than max_size(), length_error is
   *  thrown.
   *
   *  The advantage of this function is that if optimal code is a
   *  necessity and the user can determine the string length that will be
   *  required, the user can reserve the memory in %advance, and thus
   *  prevent a possible reallocation of memory and copying of %string
   *  data.
   */
  void reserve(size_type _res_arg = 0);

  /**
   *  Erases the string, making it empty.
   */
  void clear() noexcept { _M_set_length(0); }

  /**
   *  Returns true if the %string is empty.  Equivalent to
   *  <code>*this == ""</code>.
   */
  bool empty() const noexcept { return this->size() == 0; }

  // Element access:
  /**
   *  @brief  Subscript access to the data contained in the %string.
   *  @param  _pos  The index of the character to access.
   *  @return  Read-only (constant) reference to the character.
   *
   *  This operator allows for easy, array-style, data access.
   *  Note that data access with this operator is unchecked and
   *  out_of_range lookups are not defined. (For checked lookups
   *  see at().)
   */
  const_reference operator[](size_type _pos) const noexcept {
    assert(_pos <= size());
    return _M_data()[_pos];
  }

  /**
   *  @brief  Subscript access to the data contained in the %string.
   *  @param  _pos  The index of the character to access.
   *  @return  Read/write reference to the character.
   *
   *  This operator allows for easy, array-style, data access.
   *  Note that data access with this operator is unchecked and
   *  out_of_range lookups are not defined. (For checked lookups
   *  see at().)
   */
  reference operator[](size_type _pos) {
    // Allow pos == size() both in C++98 mode, as v3 extension,
    // and in C++11 mode.
    assert(_pos <= size());
    return _M_data()[_pos];
  }

  /**
   *  @brief  Provides access to the data contained in the %string.
   *  @param _n The index of the character to access.
   *  @return  Read-only (const) reference to the character.
   *  @throw  std::out_of_range  If @a n is an invalid index.
   *
   *  This function provides for safer data access.  The parameter is
   *  first checked that it is in the range of the string.  The function
   *  throws out_of_range if the check fails.
   */
  const_reference at(size_type _n) const {
    if (_n >= this->size()) {
      std::string error_info = "ShmBasicString::at: _n which is " + std::to_string(_n) +
                               " >= this->size()  which is " + std::to_string(this->size());
      throw std::out_of_range(error_info.c_str());
    }
    return _M_data()[_n];
  }

  /**
   *  @brief  Provides access to the data contained in the %string.
   *  @param _n The index of the character to access.
   *  @return  Read/write reference to the character.
   *  @throw  std::out_of_range  If @a n is an invalid index.
   *
   *  This function provides for safer data access.  The parameter is
   *  first checked that it is in the range of the string.  The function
   *  throws out_of_range if the check fails.
   */
  reference at(size_type _n) {
    if (_n >= size()) {
      std::string error_info = "ShmBasicString::at: _n which is " + std::to_string(_n) +
                               " >= this->size()  which is " + std::to_string(this->size());
      throw std::out_of_range(error_info.c_str());
    }

    return _M_data()[_n];
  }


  /**
   *  Returns a read/write reference to the data at the first
   *  element of the %string.
   */
  reference front() noexcept { return operator[](0); }

  /**
   *  Returns a read-only (constant) reference to the data at the first
   *  element of the %string.
   */
  const_reference front() const noexcept { return operator[](0); }

  /**
   *  Returns a read/write reference to the data at the last
   *  element of the %string.
   */
  reference back() noexcept { return operator[](this->size() - 1); }

  /**
   *  Returns a read-only (constant) reference to the data at the
   *  last element of the %string.
   */
  const_reference back() const noexcept { return operator[](this->size() - 1); }

  // Modifiers:
  /**
   *  @brief  Append a string to this string.
   *  @param _str  The string to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& operator+=(const ShmBasicString& _str) { return this->append(_str); }

  /**
   *  @brief  Append a C string.
   *  @param _s  The C string to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& operator+=(const _CharT* _s) { return this->append(_s); }

  /**
   *  @brief  Append a character.
   *  @param _c  The character to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& operator+=(_CharT _c) {
    this->push_back(_c);
    return *this;
  }

  /**
   *  @brief  Append an initializer_list of characters.
   *  @param _l  The initializer_list of characters to be appended.
   *  @return  Reference to this string.
   */
  ShmBasicString& operator+=(std::initializer_list<_CharT> _l) {
    return this->append(_l.begin(), _l.size());
  }

  /**
   *  @brief  Append a string to this string.
   *  @param _str  The string to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& append(const ShmBasicString& _str) {
    return _M_append(_str._M_data(), _str.size());
  }

  /**
   *  @brief  Append a substring.
   *  @param _str  The string to append.
   *  @param _pos  Index of the first character of str to append.
   *  @param _n  The number of characters to append.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range if @a _pos is not a valid index.
   *
   *  This function appends @a _n characters from @a _str
   *  starting at @a _pos to this string.  If @a _n is is larger
   *  than the number of available characters in @a _str, the
   *  remainder of @a _str is appended.
   */
  ShmBasicString& append(const ShmBasicString& _str, size_type _pos, size_type _n) {
    return _M_append(_str._M_data() + _str._M_check(_pos, "ShmBasicString::append"),
                     _str._M_limit(_pos, _n));
  }

  /**
   *  @brief  Append a C substring.
   *  @param _s  The C string to append.
   *  @param _n  The number of characters to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& append(const _CharT* _s, size_type _n) {
    _M_check_length(size_type(0), _n, "ShmBasicString::append");
    return _M_append(_s, _n);
  }

  /**
   *  @brief  Append a C string.
   *  @param _s  The C string to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& append(const _CharT* _s) {
    const size_type _n = traits_type::length(_s);
    _M_check_length(size_type(0), _n, "ShmBasicString::append");
    return _M_append(_s, _n);
  }

  /**
   *  @brief  Append multiple characters.
   *  @param _n  The number of characters to append.
   *  @param _c  The character to use.
   *  @return  Reference to this string.
   *
   *  Appends _n copies of _c to this string.
   */
  ShmBasicString& append(size_type _n, _CharT _c) {
    return _M_replace_aux(this->size(), size_type(0), _n, _c);
  }

  /**
   *  @brief  Append an initializer_list of characters.
   *  @param _l  The initializer_list of characters to append.
   *  @return  Reference to this string.
   */
  ShmBasicString& append(std::initializer_list<_CharT> _l) {
    return this->append(_l.begin(), _l.size());
  }

  /**
   *  @brief  Append a range of characters.
   *  @param _first  Iterator referencing the first character to append.
   *  @param _last  Iterator marking the end of the range.
   *  @return  Reference to this string.
   *
   *  Appends characters in the range [_first,_last) to this string.
   */

  template <class _InputIterator>
  ShmBasicString& append(_InputIterator _first, _InputIterator _last) {
    return this->replace(end(), end(), _first, _last);
  }

  /**
   *  @brief  Append a single character.
   *  @param _c  Character to append.
   */
  void push_back(_CharT _c) {
    const size_type _size = this->size();
    if (_size + 1 > this->capacity()) this->_M_mutate(_size, size_type(0), 0, size_type(1));
    traits_type::assign(this->_M_data()[_size], _c);
    this->_M_set_length(_size + 1);
  }

  /**
   *  @brief  Set value to contents of another string.
   *  @param  _str  Source string to use.
   *  @return  Reference to this string.
   */
  ShmBasicString& assign(const ShmBasicString& _str) {
    this->_M_assign(_str);
    return *this;
  }

  /**
   *  @brief  Set value to contents of another string.
   *  @param  _str  Source string to use.
   *  @return  Reference to this string.
   *
   *  This function sets this string to the exact contents of @a _str.
   *  @a _str is a valid, but unspecified string.
   */
  ShmBasicString& assign(ShmBasicString&& _str) {
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 2063. Contradictory requirements for string move assignment
    return *this = std::move(_str);
  }

  /**
   *  @brief  Set value to a substring of a string.
   *  @param _str  The string to use.
   *  @param _pos  Index of the first character of str.
   *  @param _n  Number of characters to use.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range if @a pos is not a valid index.
   *
   *  This function sets this string to the substring of @a _str
   *  consisting of @a _n characters at @a _pos.  If @a _n is
   *  is larger than the number of available characters in @a
   *  _str, the remainder of @a _str is used.
   */
  ShmBasicString& assign(const ShmBasicString& _str, size_type _pos, size_type _n) {
    return _M_replace(size_type(0), this->size(),
                      _str._M_data() + _str._M_check(_pos, "ShmBasicString::assign"),
                      _str._M_limit(_pos, _n));
  }

  /**
   *  @brief  Set value to a C substring.
   *  @param _s  The C string to use.
   *  @param _n  Number of characters to use.
   *  @return  Reference to this string.
   *
   *  This function sets the value of this string to the first @a _n
   *  characters of @a _s.  If @a _n is is larger than the number of
   *  available characters in @a _s, the remainder of @a _s is used.
   */
  ShmBasicString& assign(const _CharT* _s, size_type _n) {
    return _M_replace(size_type(0), this->size(), _s, _n);
  }

  /**
   *  @brief  Set value to contents of a C string.
   *  @param _s  The C string to use.
   *  @return  Reference to this string.
   *
   *  This function sets the value of this string to the value of @a _s.
   *  The data is copied, so there is no dependence on @a _s once the
   *  function returns.
   */
  ShmBasicString& assign(const _CharT* _s) {
    return _M_replace(size_type(0), this->size(), _s, traits_type::length(_s));
  }

  /**
   *  @brief  Set value to multiple characters.
   *  @param _n  Length of the resulting string.
   *  @param _c  The character to use.
   *  @return  Reference to this string.
   *
   *  This function sets the value of this string to @a _n copies of
   *  character @a _c.
   */
  ShmBasicString& assign(size_type _n, _CharT _c) {
    return _M_replace_aux(size_type(0), this->size(), _n, _c);
  }

  /**
   *  @brief  Set value to a range of characters.
   *  @param _first  Iterator referencing the first character to append.
   *  @param _last  Iterator marking the end of the range.
   *  @return  Reference to this string.
   *
   *  Sets value of string to characters in the range [_first,_last).
   */

  template <class _InputIterator>
  ShmBasicString& assign(_InputIterator _first, _InputIterator _last) {
    return this->replace(begin(), end(), _first, _last);
  }

  /**
   *  @brief  Set value to an initializer_list of characters.
   *  @param _l  The initializer_list of characters to assign.
   *  @return  Reference to this string.
   */
  ShmBasicString& assign(std::initializer_list<_CharT> _l) {
    return this->assign(_l.begin(), _l.size());
  }

  /**
   *  @brief  Insert multiple characters.
   *  @param _p  Const_iterator referencing location in string to
   *              insert at.
   *  @param _n  Number of characters to insert
   *  @param _c  The character to insert.
   *  @return  Iterator referencing the first inserted char.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Inserts @a _n copies of character @a _c starting at the
   *  position referenced by iterator @a _p.  If adding
   *  characters causes the length to exceed max_size(),
   *  length_error is thrown.  The value of the string doesn't
   *  change if an error is thrown.
   */
  iterator insert(const_iterator _p, size_type _n, _CharT _c) {
    assert(_p >= begin() && _p <= end());
    const size_type _pos = _p - begin();
    this->replace(_p, _p, _n, _c);
    return iterator(this->_M_data() + _pos);
  }


  /**
   *  @brief  Insert a range of characters.
   *  @param _p  Const_iterator referencing location in string to
   *              insert at.
   *  @param _beg  Start of range.
   *  @param _end  End of range.
   *  @return  Iterator referencing the first inserted char.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Inserts characters in range [beg,end).  If adding characters
   *  causes the length to exceed max_size(), length_error is
   *  thrown.  The value of the string doesn't change if an error
   *  is thrown.
   */
  template <class _InputIterator>
  iterator insert(const_iterator _p, _InputIterator _beg, _InputIterator _end) {
    assert(_p >= begin() && _p <= end());
    const size_type _pos = _p - begin();
    this->replace(_p, _p, _beg, _end);
    return iterator(this->_M_data() + _pos);
  }



  /**
   *  @brief  Insert an initializer_list of characters.
   *  @param _p  Iterator referencing location in string to insert at.
   *  @param _l  The initializer_list of characters to insert.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   */
  void insert(iterator _p, std::initializer_list<_CharT> _l) {
    assert(_p >= begin() && _p <= end());
    this->insert(_p - begin(), _l.begin(), _l.size());
  }

  /**
   *  @brief  Insert value of a string.
   *  @param _pos1  Iterator referencing location in string to insert at.
   *  @param _str  The string to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Inserts value of @a _str starting at @a _pos1.  If adding
   *  characters causes the length to exceed max_size(),
   *  length_error is thrown.  The value of the string doesn't
   *  change if an error is thrown.
   */
  ShmBasicString& insert(size_type _pos1, const ShmBasicString& _str) {
    return this->replace(_pos1, size_type(0), _str._M_data(), _str.size());
  }

  /**
   *  @brief  Insert a substring.
   *  @param _pos1  Iterator referencing location in string to insert at.
   *  @param _str  The string to insert.
   *  @param _pos2  Start of characters in str to insert.
   *  @param _n  Number of characters to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *  @throw  std::out_of_range  If @a pos1 > size() or
   *  @a _pos2 > @a str.size().
   *
   *  Starting at @a pos1, insert @a _n character of @a _str
   *  beginning with @a _pos2.  If adding characters causes the
   *  length to exceed max_size(), length_error is thrown.  If @a
   *  _pos1 is beyond the end of this string or @a _pos2 is
   *  beyond the end of @a _str, out_of_range is thrown.  The
   *  value of the string doesn't change if an error is thrown.
   */
  ShmBasicString& insert(size_type _pos1, const ShmBasicString& _str, size_type _pos2,
                       size_type _n) {
    return this->replace(_pos1, size_type(0),
                         _str._M_data() + _str._M_check(_pos2, "ShmBasicString::insert"),
                         _str._M_limit(_pos2, _n));
  }

  /**
   *  @brief  Insert a C substring.
   *  @param _pos  Iterator referencing location in string to insert at.
   *  @param _s  The C string to insert.
   *  @param _n  The number of characters to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *  @throw  std::out_of_range  If @a _pos is beyond the end of this
   *  string.
   *
   *  Inserts the first @a _n characters of @a _s starting at @a
   *  _pos.  If adding characters causes the length to exceed
   *  max_size(), length_error is thrown.  If @a _pos is beyond
   *  end(), out_of_range is thrown.  The value of the string
   *  doesn't change if an error is thrown.
   */
  ShmBasicString& insert(size_type _pos, const _CharT* _s, size_type _n) {
    return this->replace(_pos, size_type(0), _s, _n);
  }

  /**
   *  @brief  Insert a C string.
   *  @param _pos  Iterator referencing location in string to insert at.
   *  @param _s  The C string to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *  @throw  std::out_of_range  If @a pos is beyond the end of this
   *  string.
   *
   *  Inserts the first @a n characters of @a _s starting at @a _pos.  If
   *  adding characters causes the length to exceed max_size(),
   *  length_error is thrown.  If @a _pos is beyond end(), out_of_range is
   *  thrown.  The value of the string doesn't change if an error is
   *  thrown.
   */
  ShmBasicString& insert(size_type _pos, const _CharT* _s) {
    return this->replace(_pos, size_type(0), _s, traits_type::length(_s));
  }

  /**
   *  @brief  Insert multiple characters.
   *  @param _pos  Index in string to insert at.
   *  @param _n  Number of characters to insert
   *  @param _c  The character to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *  @throw  std::out_of_range  If @a _pos is beyond the end of this
   *  string.
   *
   *  Inserts @a _n copies of character @a _c starting at index
   *  @a _pos.  If adding characters causes the length to exceed
   *  max_size(), length_error is thrown.  If @a _pos > length(),
   *  out_of_range is thrown.  The value of the string doesn't
   *  change if an error is thrown.
   */
  ShmBasicString& insert(size_type _pos, size_type _n, _CharT _c) {
    return _M_replace_aux(_M_check(_pos, "ShmBasicString::insert"), size_type(0), _n, _c);
  }

  /**
   *  @brief  Insert one character.
   *  @param _p  Iterator referencing position in string to insert at.
   *  @param _c  The character to insert.
   *  @return  Iterator referencing newly inserted char.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Inserts character @a _c at position referenced by @a _p.
   *  If adding character causes the length to exceed max_size(),
   *  length_error is thrown.  If @a _p is beyond end of string,
   *  out_of_range is thrown.  The value of the string doesn't
   *  change if an error is thrown.
   */
  iterator insert(const_iterator _p, _CharT _c) {
    assert(_p >= begin() && _p <= end());
    const size_type _pos = _p - begin();
    _M_replace_aux(_pos, size_type(0), size_type(1), _c);
    return iterator(_M_data() + _pos);
  }

  /**
   *  @brief  Remove characters.
   *  @param _pos  Index of first character to remove (default 0).
   *  @param _n  Number of characters to remove (default remainder).
   *  @return  Reference to this string.
   *  @throw  std::out_of_range  If @a pos is beyond the end of this
   *  string.
   *
   *  Removes @a _n characters from this string starting at @a
   *  _pos.  The length of the string is reduced by @a _n.  If
   *  there are < @a _n characters to remove, the remainder of
   *  the string is truncated.  If @a _p is beyond end of string,
   *  out_of_range is thrown.  The value of the string doesn't
   *  change if an error is thrown.
   */
  ShmBasicString& erase(size_type _pos = 0, size_type _n = npos) {
    this->_M_erase(_M_check(_pos, "ShmBasicString::erase"), _M_limit(_pos, _n));
    return *this;
  }

  /**
   *  @brief  Remove one character.
   *  @param _position  Iterator referencing the character to remove.
   *  @return  iterator referencing same location after removal.
   *
   *  Removes the character at @a _position from this string. The value
   *  of the string doesn't change if an error is thrown.
   */
  iterator erase(const_iterator _position) {
    assert(_position >= begin() && _position < end());
    const size_type _pos = _position - begin();
    this->_M_erase(_pos, size_type(1));
    return iterator(_M_data() + _pos);
  }

  /**
   *  @brief  Remove a range of characters.
   *  @param _first  Iterator referencing the first character to remove.
   *  @param _last  Iterator referencing the end of the range.
   *  @return  Iterator referencing location of first after removal.
   *
   *  Removes the characters in the range [first,last) from this string.
   *  The value of the string doesn't change if an error is thrown.
   */
  iterator erase(const_iterator _first, const_iterator _last) {
    assert(_first >= begin() && _first <= _last && _last <= end());
    const size_type _pos = _first - begin();
    this->_M_erase(_pos, _last - _first);
    return iterator(this->_M_data() + _pos);
  }


  /**
   *  @brief  Remove the last character.
   *
   *  The string must be non-empty.
   */
  void pop_back() noexcept { _M_erase(size() - 1, 1); }

  /**
   *  @brief  Replace characters with value from another string.
   *  @param _pos  Index of first character to replace.
   *  @param _n  Number of characters to be replaced.
   *  @param _str  String to insert.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range  If @a pos is beyond the end of this
   *  string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_pos,_pos+_n) from
   *  this string.  In place, the value of @a _str is inserted.
   *  If @a _pos is beyond end of string, out_of_range is thrown.
   *  If the length of the result exceeds max_size(), length_error
   *  is thrown.  The value of the string doesn't change if an
   *  error is thrown.
   */
  ShmBasicString& replace(size_type _pos, size_type _n, const ShmBasicString& _str) {
    return this->replace(_pos, _n, _str._M_data(), _str.size());
  }

  /**
   *  @brief  Replace characters with value from another string.
   *  @param _pos1  Index of first character to replace.
   *  @param _n1  Number of characters to be replaced.
   *  @param _str  String to insert.
   *  @param _pos2  Index of first character of str to use.
   *  @param _n2  Number of characters from str to use.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range  If @a _pos1 > size() or @a _pos2 >
   *  _str.size().
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_pos1,_pos1 + n) from this
   *  string.  In place, the value of @a _str is inserted.  If @a _pos is
   *  beyond end of string, out_of_range is thrown.  If the length of the
   *  result exceeds max_size(), length_error is thrown.  The value of the
   *  string doesn't change if an error is thrown.
   */
  ShmBasicString& replace(size_type _pos1, size_type _n1, const ShmBasicString& _str,
                        size_type _pos2, size_type _n2) {
    return this->replace(_pos1, _n1,
                         _str._M_data() + _str._M_check(_pos2, "ShmBasicString::replace"),
                         _str._M_limit(_pos2, _n2));
  }

  /**
   *  @brief  Replace characters with value of a C substring.
   *  @param _pos  Index of first character to replace.
   *  @param _n1  Number of characters to be replaced.
   *  @param _s  C string to insert.
   *  @param _n2  Number of characters from @a s to use.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range  If @a pos1 > size().
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_pos,_pos + _n1)
   *  from this string.  In place, the first @a _n2 characters of
   *  @a _s are inserted, or all of @a _s if @a _n2 is too large.  If
   *  @a _pos is beyond end of string, out_of_range is thrown.  If
   *  the length of result exceeds max_size(), length_error is
   *  thrown.  The value of the string doesn't change if an error
   *  is thrown.
   */
  ShmBasicString& replace(size_type _pos, size_type _n1, const _CharT* _s, size_type _n2) {
    return _M_replace(_M_check(_pos, "ShmBasicString::replace"), _M_limit(_pos, _n1), _s, _n2);
  }

  /**
   *  @brief  Replace characters with value of a C string.
   *  @param _pos  Index of first character to replace.
   *  @param _n1  Number of characters to be replaced.
   *  @param _s  C string to insert.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range  If @a pos > size().
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_pos,_pos + _n1)
   *  from this string.  In place, the characters of @a _s are
   *  inserted.  If @a _pos is beyond end of string, out_of_range
   *  is thrown.  If the length of result exceeds max_size(),
   *  length_error is thrown.  The value of the string doesn't
   *  change if an error is thrown.
   */
  ShmBasicString& replace(size_type _pos, size_type _n1, const _CharT* _s) {
    return this->replace(_pos, _n1, _s, traits_type::length(_s));
  }

  /**
   *  @brief  Replace characters with multiple characters.
   *  @param _pos  Index of first character to replace.
   *  @param _n1  Number of characters to be replaced.
   *  @param _n2  Number of characters to insert.
   *  @param _c  Character to insert.
   *  @return  Reference to this string.
   *  @throw  std::out_of_range  If @a _pos > size().
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [pos,pos + n1) from this
   *  string.  In place, @a _n2 copies of @a _c are inserted.
   *  If @a _pos is beyond end of string, out_of_range is thrown.
   *  If the length of result exceeds max_size(), length_error is
   *  thrown.  The value of the string doesn't change if an error
   *  is thrown.
   */
  ShmBasicString& replace(size_type _pos, size_type _n1, size_type _n2, _CharT _c) {
    return _M_replace_aux(_M_check(_pos, "ShmBasicString::replace"), _M_limit(_pos, _n1), _n2,
                          _c);
  }

  /**
   *  @brief  Replace range of characters with string.
   *  @param _i1  Iterator referencing start of range to replace.
   *  @param _i2  Iterator referencing end of range to replace.
   *  @param _str  String value to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_i1,_i2).  In place,
   *  the value of @a _str is inserted.  If the length of result
   *  exceeds max_size(), length_error is thrown.  The value of
   *  the string doesn't change if an error is thrown.
   */
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, const ShmBasicString& _str) {
    return this->replace(_i1, _i2, _str._M_data(), _str.size());
  }

  /**
   *  @brief  Replace range of characters with C substring.
   *  @param _i1  Iterator referencing start of range to replace.
   *  @param _i2  Iterator referencing end of range to replace.
   *  @param _s  C string value to insert.
   *  @param _n  Number of characters from s to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_i1,_i2).  In place,
   *  the first @a _n characters of @a _s are inserted.  If the
   *  length of result exceeds max_size(), length_error is thrown.
   *  The value of the string doesn't change if an error is
   *  thrown.
   */
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, const _CharT* _s,
                        size_type _n) {
    assert(begin() <= _i1 && _i1 <= _i2 && _i2 <= end());
    return this->replace(_i1 - begin(), _i2 - _i1, _s, _n);
  }

  /**
   *  @brief  Replace range of characters with C string.
   *  @param _i1  Iterator referencing start of range to replace.
   *  @param _i2  Iterator referencing end of range to replace.
   *  @param _s  C string value to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_i1,_i2).  In place,
   *  the characters of @a _s are inserted.  If the length of
   *  result exceeds max_size(), length_error is thrown.  The
   *  value of the string doesn't change if an error is thrown.
   */
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, const _CharT* _s) {
    return this->replace(_i1, _i2, _s, traits_type::length(_s));
  }

  /**
   *  @brief  Replace range of characters with multiple characters
   *  @param _i1  Iterator referencing start of range to replace.
   *  @param _i2  Iterator referencing end of range to replace.
   *  @param _n  Number of characters to insert.
   *  @param _c  Character to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_i1,_i2).  In place,
   *  @a _n copies of @a _c are inserted.  If the length of
   *  result exceeds max_size(), length_error is thrown.  The
   *  value of the string doesn't change if an error is thrown.
   */
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, size_type _n, _CharT _c) {
    assert(begin() <= _i1 && _i1 <= _i2 && _i2 <= end());
    return _M_replace_aux(_i1 - begin(), _i2 - _i1, _n, _c);
  }

  /**
   *  @brief  Replace range of characters with range.
   *  @param _i1  Iterator referencing start of range to replace.
   *  @param _i2  Iterator referencing end of range to replace.
   *  @param _k1  Iterator referencing start of range to insert.
   *  @param _k2  Iterator referencing end of range to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_i1,_i2).  In place,
   *  characters in the range [_k1,_k2) are inserted.  If the
   *  length of result exceeds max_size(), length_error is thrown.
   *  The value of the string doesn't change if an error is
   *  thrown.
   */
  template <class _InputIterator>
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, _InputIterator _k1,
                        _InputIterator _k2) {
    assert(begin() <= _i1 && _i1 <= _i2 && _i2 <= end());
    return this->_M_replace_dispatch(_i1, _i2, _k1, _k2, std::false_type());
  }


  // Specializations for the common case of pointer and iterator:
  // useful to avoid the overhead of temporary buffering in _M_replace.
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, _CharT* _k1, _CharT* _k2) {
    assert(begin() <= _i1 && _i1 <= _i2 && _i2 <= end());
    return this->replace(_i1 - begin(), _i2 - _i1, _k1, _k2 - _k1);
  }

  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, const _CharT* _k1,
                        const _CharT* _k2) {
    assert(begin() <= _i1 && _i1 <= _i2 && _i2 <= end());
    return this->replace(_i1 - begin(), _i2 - _i1, _k1, _k2 - _k1);
  }


  /**
   *  @brief  Replace range of characters with initializer_list.
   *  @param _i1  Iterator referencing start of range to replace.
   *  @param _i2  Iterator referencing end of range to replace.
   *  @param _l  The initializer_list of characters to insert.
   *  @return  Reference to this string.
   *  @throw  std::length_error  If new length exceeds @c max_size().
   *
   *  Removes the characters in the range [_i1,_i2).  In place,
   *  characters in the range [_k1,_k2) are inserted.  If the
   *  length of result exceeds max_size(), length_error is thrown.
   *  The value of the string doesn't change if an error is
   *  thrown.
   */
  ShmBasicString& replace(const_iterator _i1, const_iterator _i2, std::initializer_list<_CharT> _l) {
    return this->replace(_i1, _i2, _l.begin(), _l.end());
  }

 private:
  template <class _Integer>
  ShmBasicString& _M_replace_dispatch(const_iterator _i1, const_iterator _i2, _Integer _n,
                                    _Integer _val, std::true_type ) {
    return _M_replace_aux(_i1 - begin(), _i2 - _i1, _n, _val);
  }

  template <class _InputIterator>
  ShmBasicString& _M_replace_dispatch(const_iterator _i1, const_iterator _i2, _InputIterator _k1,
                                    _InputIterator _k2, std::false_type);

  ShmBasicString& _M_replace_aux(size_type _pos1, size_type _n1, size_type _n2, _CharT _c);

  ShmBasicString& _M_replace(size_type _pos, size_type _len1, const _CharT* _s,
                           const size_type _len2);

  ShmBasicString& _M_append(const _CharT* _s, size_type _n);

 public:
  /**
   *  @brief  Copy substring into C string.
   *  @param _s  C string to copy value into.
   *  @param _n  Number of characters to copy.
   *  @param _pos  Index of first character to copy.
   *  @return  Number of characters actually copied
   *  @throw  std::out_of_range  If _pos > size().
   *
   *  Copies up to @a _n characters starting at @a _pos into the
   *  C string @a _s.  If @a _pos is %greater than size(),
   *  out_of_range is thrown.
   */
  size_type copy(_CharT* _s, size_type _n, size_type _pos = 0) const;

  /**
   *  @brief  Swap contents with another string.
   *  @param _s  String to swap with.
   *
   *  Exchanges the contents of this string with that of @a _s in constant
   *  time.
   */
  void swap(ShmBasicString& _s) noexcept;

  // String operations:
  /**
   *  @brief  Return const pointer to null-terminated contents.
   *
   *  This is a handle to internal data.  Do not modify or dire things may
   *  happen.
   */
  const _CharT* c_str() const noexcept { return _M_data(); }

  /**
   *  @brief  Return const pointer to contents.
   *
   *  This is a handle to internal data.  Do not modify or dire things may
   *  happen.
   */
  const _CharT* data() const noexcept { return _M_data(); }

  /**
   *  @brief  Return copy of allocator used to construct this string.
   */
  allocator_type get_allocator() const noexcept { return _M_get_allocator(); }

  /**
   *  @brief  Find position of a C substring.
   *  @param _s  C string to locate.
   *  @param _pos  Index of character to search from.
   *  @param _n  Number of characters from @a s to search for.
   *  @return  Index of start of first occurrence.
   *
   *  Starting from @a _pos, searches forward for the first @a
   *  _n characters in @a _s within this string.  If found,
   *  returns the index where it begins.  If not found, returns
   *  npos.
   */
  size_type find(const _CharT* _s, size_type _pos, size_type _n) const;

  /**
   *  @brief  Find position of a string.
   *  @param _str  String to locate.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of start of first occurrence.
   *
   *  Starting from @a _pos, searches forward for value of @a _str within
   *  this string.  If found, returns the index where it begins.  If not
   *  found, returns npos.
   */
  size_type find(const ShmBasicString& _str, size_type _pos = 0) const noexcept {
    return this->find(_str.data(), _pos, _str.size());
  }

  /**
   *  @brief  Find position of a C string.
   *  @param _s  C string to locate.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of start of first occurrence.
   *
   *  Starting from @a _pos, searches forward for the value of @a
   *  _s within this string.  If found, returns the index where
   *  it begins.  If not found, returns npos.
   */
  size_type find(const _CharT* _s, size_type _pos = 0) const {
    return this->find(_s, _pos, traits_type::length(_s));
  }

  /**
   *  @brief  Find position of a character.
   *  @param _c  Character to locate.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for @a _c within
   *  this string.  If found, returns the index where it was
   *  found.  If not found, returns npos.
   */
  size_type find(_CharT _c, size_type _pos = 0) const noexcept;

  /**
   *  @brief  Find last position of a string.
   *  @param _str  String to locate.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of start of last occurrence.
   *
   *  Starting from @a _pos, searches backward for value of @a
   *  _str within this string.  If found, returns the index where
   *  it begins.  If not found, returns npos.
   */
  size_type rfind(const ShmBasicString& _str, size_type _pos = npos) const noexcept {
    return this->rfind(_str.data(), _pos, _str.size());
  }

  /**
   *  @brief  Find last position of a C substring.
   *  @param _s  C string to locate.
   *  @param _pos  Index of character to search back from.
   *  @param _n  Number of characters from s to search for.
   *  @return  Index of start of last occurrence.
   *
   *  Starting from @a _pos, searches backward for the first @a
   *  _n characters in @a _s within this string.  If found,
   *  returns the index where it begins.  If not found, returns
   *  npos.
   */
  size_type rfind(const _CharT* _s, size_type _pos, size_type _n) const;

  /**
   *  @brief  Find last position of a C string.
   *  @param _s  C string to locate.
   *  @param _pos  Index of character to start search at (default end).
   *  @return  Index of start of  last occurrence.
   *
   *  Starting from @a _pos, searches backward for the value of
   *  @a _s within this string.  If found, returns the index
   *  where it begins.  If not found, returns npos.
   */
  size_type rfind(const _CharT* _s, size_type _pos = npos) const {
    return this->rfind(_s, _pos, traits_type::length(_s));
  }

  /**
   *  @brief  Find last position of a character.
   *  @param _c  Character to locate.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for @a _c within
   *  this string.  If found, returns the index where it was
   *  found.  If not found, returns npos.
   */
  size_type rfind(_CharT _c, size_type _pos = npos) const noexcept;

  /**
   *  @brief  Find position of a character of string.
   *  @param _str  String containing characters to locate.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for one of the
   *  characters of @a _str within this string.  If found,
   *  returns the index where it was found.  If not found, returns
   *  npos.
   */
  size_type find_first_of(const ShmBasicString& _str, size_type _pos = 0) const noexcept {
    return this->find_first_of(_str.data(), _pos, _str.size());
  }

  /**
   *  @brief  Find position of a character of C substring.
   *  @param _s  String containing characters to locate.
   *  @param _pos  Index of character to search from.
   *  @param _n  Number of characters from s to search for.
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for one of the
   *  first @a _n characters of @a _s within this string.  If
   *  found, returns the index where it was found.  If not found,
   *  returns npos.
   */
  size_type find_first_of(const _CharT* _s, size_type _pos, size_type _n) const;

  /**
   *  @brief  Find position of a character of C string.
   *  @param _s  String containing characters to locate.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for one of the
   *  characters of @a _s within this string.  If found, returns
   *  the index where it was found.  If not found, returns npos.
   */
  size_type find_first_of(const _CharT* _s, size_type _pos = 0) const {
    return this->find_first_of(_s, _pos, traits_type::length(_s));
  }

  /**
   *  @brief  Find position of a character.
   *  @param _c  Character to locate.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for the character
   *  @a _c within this string.  If found, returns the index
   *  where it was found.  If not found, returns npos.
   *
   *  Note: equivalent to find(_c, _pos).
   */
  size_type find_first_of(_CharT _c, size_type _pos = 0) const noexcept {
    return this->find(_c, _pos);
  }

  /**
   *  @brief  Find last position of a character of string.
   *  @param _str  String containing characters to locate.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for one of the
   *  characters of @a _str within this string.  If found,
   *  returns the index where it was found.  If not found, returns
   *  npos.
   */
  size_type find_last_of(const ShmBasicString& _str, size_type _pos = npos) const noexcept {
    return this->find_last_of(_str.data(), _pos, _str.size());
  }

  /**
   *  @brief  Find last position of a character of C substring.
   *  @param _s  C string containing characters to locate.
   *  @param _pos  Index of character to search back from.
   *  @param _n  Number of characters from s to search for.
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for one of the
   *  first @a _n characters of @a _s within this string.  If
   *  found, returns the index where it was found.  If not found,
   *  returns npos.
   */
  size_type find_last_of(const _CharT* _s, size_type _pos, size_type _n) const;

  /**
   *  @brief  Find last position of a character of C string.
   *  @param _s  C string containing characters to locate.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for one of the
   *  characters of @a _s within this string.  If found, returns
   *  the index where it was found.  If not found, returns npos.
   */
  size_type find_last_of(const _CharT* _s, size_type _pos = npos) const {
    return this->find_last_of(_s, _pos, traits_type::length(_s));
  }

  /**
   *  @brief  Find last position of a character.
   *  @param _c  Character to locate.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for @a _c within
   *  this string.  If found, returns the index where it was
   *  found.  If not found, returns npos.
   *
   *  Note: equivalent to rfind(_c, _pos).
   */
  size_type find_last_of(_CharT _c, size_type _pos = npos) const noexcept {
    return this->rfind(_c, _pos);
  }

  /**
   *  @brief  Find position of a character not in string.
   *  @param _str  String containing characters to avoid.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for a character not contained
   *  in @a _str within this string.  If found, returns the index where it
   *  was found.  If not found, returns npos.
   */
  size_type find_first_not_of(const ShmBasicString& _str, size_type _pos = 0) const noexcept {
    return this->find_first_not_of(_str.data(), _pos, _str.size());
  }

  /**
   *  @brief  Find position of a character not in C substring.
   *  @param _s  C string containing characters to avoid.
   *  @param _pos  Index of character to search from.
   *  @param _n  Number of characters from _s to consider.
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for a character not
   *  contained in the first @a _n characters of @a _s within
   *  this string.  If found, returns the index where it was
   *  found.  If not found, returns npos.
   */
  size_type find_first_not_of(const _CharT* _s, size_type _pos, size_type _n) const;

  /**
   *  @brief  Find position of a character not in C string.
   *  @param _s  C string containing characters to avoid.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for a character not
   *  contained in @a _s within this string.  If found, returns
   *  the index where it was found.  If not found, returns npos.
   */
  size_type find_first_not_of(const _CharT* _s, size_type _pos = 0) const {
    return this->find_first_not_of(_s, _pos, traits_type::length(_s));
  }

  /**
   *  @brief  Find position of a different character.
   *  @param _c  Character to avoid.
   *  @param _pos  Index of character to search from (default 0).
   *  @return  Index of first occurrence.
   *
   *  Starting from @a _pos, searches forward for a character
   *  other than @a _c within this string.  If found, returns the
   *  index where it was found.  If not found, returns npos.
   */
  size_type find_first_not_of(_CharT _c, size_type _pos = 0) const noexcept;

  /**
   *  @brief  Find last position of a character not in string.
   *  @param _str  String containing characters to avoid.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for a character
   *  not contained in @a _str within this string.  If found,
   *  returns the index where it was found.  If not found, returns
   *  npos.
   */
  size_type find_last_not_of(const ShmBasicString& _str, size_type _pos = npos) const noexcept {
    return this->find_last_not_of(_str.data(), _pos, _str.size());
  }

  /**
   *  @brief  Find last position of a character not in C substring.
   *  @param _s  C string containing characters to avoid.
   *  @param _pos  Index of character to search back from.
   *  @param _n  Number of characters from s to consider.
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for a character not
   *  contained in the first @a _n characters of @a _s within this string.
   *  If found, returns the index where it was found.  If not found,
   *  returns npos.
   */
  size_type find_last_not_of(const _CharT* _s, size_type _pos, size_type _n) const;
  /**
   *  @brief  Find last position of a character not in C string.
   *  @param _s  C string containing characters to avoid.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for a character
   *  not contained in @a _s within this string.  If found,
   *  returns the index where it was found.  If not found, returns
   *  npos.
   */
  size_type find_last_not_of(const _CharT* _s, size_type _pos = npos) const {
    return this->find_last_not_of(_s, _pos, traits_type::length(_s));
  }

  /**
   *  @brief  Find last position of a different character.
   *  @param _c  Character to avoid.
   *  @param _pos  Index of character to search back from (default end).
   *  @return  Index of last occurrence.
   *
   *  Starting from @a _pos, searches backward for a character other than
   *  @a _c within this string.  If found, returns the index where it was
   *  found.  If not found, returns npos.
   */
  size_type find_last_not_of(_CharT _c, size_type _pos = npos) const noexcept;

  /**
   *  @brief  Get a substring.
   *  @param _pos  Index of first character (default 0).
   *  @param _n  Number of characters in substring (default remainder).
   *  @return  The new string.
   *  @throw  std::out_of_range  If _pos > size().
   *
   *  Construct and return a new string using the @a _n
   *  characters starting at @a _pos.  If the string is too
   *  short, use the remainder of the characters.  If @a _pos is
   *  beyond the end of the string, out_of_range is thrown.
   */
  ShmBasicString substr(size_type _pos = 0, size_type _n = npos) const {
    return ShmBasicString(*this, _M_check(_pos, "ShmBasicString::substr"), _n);
  }

  /**
   *  @brief  Compare to a string.
   *  @param _str  String to compare against.
   *  @return  Integer < 0, 0, or > 0.
   *
   *  Returns an integer < 0 if this string is ordered before @a
   *  _str, 0 if their values are equivalent, or > 0 if this
   *  string is ordered after @a _str.  Determines the effective
   *  length rlen of the strings to compare as the smallest of
   *  size() and str.size().  The function then compares the two
   *  strings by calling traits::compare(data(), str.data(),rlen).
   *  If the result of the comparison is nonzero returns it,
   *  otherwise the shorter one is ordered first.
   */
  int compare(const ShmBasicString& _str) const {
    const size_type _size = this->size();
    const size_type _osize = _str.size();
    const size_type _len = std::min(_size, _osize);

    int _r = traits_type::compare(_M_data(), _str.data(), _len);
    if (!_r) _r = _S_compare(_size, _osize);
    return _r;
  }

  /**
   *  @brief  Compare substring to a string.
   *  @param _pos  Index of first character of substring.
   *  @param _n  Number of characters in substring.
   *  @param _str  String to compare against.
   *  @return  Integer < 0, 0, or > 0.
   *
   *  Form the substring of this string from the @a _n characters
   *  starting at @a _pos.  Returns an integer < 0 if the
   *  substring is ordered before @a _str, 0 if their values are
   *  equivalent, or > 0 if the substring is ordered after @a
   *  _str.  Determines the effective length rlen of the strings
   *  to compare as the smallest of the length of the substring
   *  and @a _str.size().  The function then compares the two
   *  strings by calling
   *  traits::compare(substring.data(),str.data(),rlen).  If the
   *  result of the comparison is nonzero returns it, otherwise
   *  the shorter one is ordered first.
   */
  int compare(size_type _pos, size_type _n, const ShmBasicString& _str) const;

  /**
   *  @brief  Compare substring to a substring.
   *  @param _pos1  Index of first character of substring.
   *  @param _n1  Number of characters in substring.
   *  @param _str  String to compare against.
   *  @param _pos2  Index of first character of substring of str.
   *  @param _n2  Number of characters in substring of str.
   *  @return  Integer < 0, 0, or > 0.
   *
   *  Form the substring of this string from the @a _n1
   *  characters starting at @a _pos1.  Form the substring of @a
   *  _str from the @a _n2 characters starting at @a _pos2.
   *  Returns an integer < 0 if this substring is ordered before
   *  the substring of @a _str, 0 if their values are equivalent,
   *  or > 0 if this substring is ordered after the substring of
   *  @a _str.  Determines the effective length rlen of the
   *  strings to compare as the smallest of the lengths of the
   *  substrings.  The function then compares the two strings by
   *  calling
   *  traits::compare(substring.data(),str.substr(pos2,n2).data(),rlen).
   *  If the result of the comparison is nonzero returns it,
   *  otherwise the shorter one is ordered first.
   */
  int compare(size_type _pos1, size_type _n1, const ShmBasicString& _str, size_type _pos2,
              size_type _n2) const;

  /**
   *  @brief  Compare to a C string.
   *  @param _s  C string to compare against.
   *  @return  Integer < 0, 0, or > 0.
   *
   *  Returns an integer < 0 if this string is ordered before @a _s, 0 if
   *  their values are equivalent, or > 0 if this string is ordered after
   *  @a _s.  Determines the effective length rlen of the strings to
   *  compare as the smallest of size() and the length of a string
   *  constructed from @a _s.  The function then compares the two strings
   *  by calling traits::compare(data(),s,rlen).  If the result of the
   *  comparison is nonzero returns it, otherwise the shorter one is
   *  ordered first.
   */
  int compare(const _CharT* _s) const;

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 5 String::compare specification questionable
  /**
   *  @brief  Compare substring to a C string.
   *  @param _pos  Index of first character of substring.
   *  @param _n1  Number of characters in substring.
   *  @param _s  C string to compare against.
   *  @return  Integer < 0, 0, or > 0.
   *
   *  Form the substring of this string from the @a _n1
   *  characters starting at @a pos.  Returns an integer < 0 if
   *  the substring is ordered before @a _s, 0 if their values
   *  are equivalent, or > 0 if the substring is ordered after @a
   *  _s.  Determines the effective length rlen of the strings to
   *  compare as the smallest of the length of the substring and
   *  the length of a string constructed from @a _s.  The
   *  function then compares the two string by calling
   *  traits::compare(substring.data(),_s,rlen).  If the result of
   *  the comparison is nonzero returns it, otherwise the shorter
   *  one is ordered first.
   */
  int compare(size_type _pos, size_type _n1, const _CharT* _s) const;

  /**
   *  @brief  Compare substring against a character %array.
   *  @param _pos  Index of first character of substring.
   *  @param _n1  Number of characters in substring.
   *  @param _s  character %array to compare against.
   *  @param _n2  Number of characters of s.
   *  @return  Integer < 0, 0, or > 0.
   *
   *  Form the substring of this string from the @a _n1
   *  characters starting at @a _pos.  Form a string from the
   *  first @a _n2 characters of @a _s.  Returns an integer < 0
   *  if this substring is ordered before the string from @a _s,
   *  0 if their values are equivalent, or > 0 if this substring
   *  is ordered after the string from @a _s.  Determines the
   *  effective length rlen of the strings to compare as the
   *  smallest of the length of the substring and @a _n2.  The
   *  function then compares the two strings by calling
   *  traits::compare(substring.data(),s,rlen).  If the result of
   *  the comparison is nonzero returns it, otherwise the shorter
   *  one is ordered first.
   *
   *  NB: s must have at least n2 characters, &apos;\\0&apos; has
   *  no special meaning.
   */
  int compare(size_type _pos, size_type _n1, const _CharT* _s, size_type _n2) const;
};


// operator+
/**
 *  @brief  Concatenate two strings.
 *  @param _lhs  First string.
 *  @param _rhs  Last string.
 *  @return  New string with value of @a _lhs followed by @a _rhs.
 */
template <typename _CharT>
ShmBasicString<_CharT> operator+(
    const ShmBasicString<_CharT>& _lhs,
    const ShmBasicString<_CharT>& _rhs) {
  ShmBasicString<_CharT> _str(_lhs);
  _str.append(_rhs);
  return _str;
}

/**
 *  @brief  Concatenate C string and string.
 *  @param _lhs  First string.
 *  @param _rhs  Last string.
 *  @return  New string with value of @a _lhs followed by @a _rhs.
 */
template <typename _CharT>
ShmBasicString<_CharT> operator+(const _CharT* _lhs,
                                                const ShmBasicString<_CharT>& _rhs);

/**
 *  @brief  Concatenate character and string.
 *  @param _lhs  First string.
 *  @param _rhs  Last string.
 *  @return  New string with @a _lhs followed by @a _rhs.
 */
template <typename _CharT>
ShmBasicString<_CharT> operator+(_CharT _lhs,
                                                const ShmBasicString<_CharT>& _rhs);

/**
 *  @brief  Concatenate string and C string.
 *  @param _lhs  First string.
 *  @param _rhs  Last string.
 *  @return  New string with @a _lhs followed by @a _rhs.
 */
template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  ShmBasicString<_CharT> _str(_lhs);
  _str.append(_rhs);
  return _str;
}

/**
 *  @brief  Concatenate string and character.
 *  @param _lhs  First string.
 *  @param _rhs  Last string.
 *  @return  New string with @a _lhs followed by @a _rhs.
 */
template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    const ShmBasicString<_CharT>& _lhs, _CharT _rhs) {
  typedef ShmBasicString<_CharT> _string_type;
  typedef typename _string_type::size_type size_type;
  _string_type _str(_lhs);
  _str.append(size_type(1), _rhs);
  return _str;
}


template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    ShmBasicString<_CharT>&& _lhs,
    const ShmBasicString<_CharT>& _rhs) {
  return std::move(_lhs.append(_rhs));
}

template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    const ShmBasicString<_CharT>& _lhs,
    ShmBasicString<_CharT>&& _rhs) {
  return std::move(_rhs.insert(0, _lhs));
}

template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    ShmBasicString<_CharT>&& _lhs, ShmBasicString<_CharT>&& _rhs) {
  const auto _size = _lhs.size() + _rhs.size();
  const bool _cond = (_size > _lhs.capacity() && _size <= _rhs.capacity());
  return _cond ? std::move(_rhs.insert(0, _lhs)) : std::move(_lhs.append(_rhs));
}

template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    const _CharT* _lhs, ShmBasicString<_CharT>&& _rhs) {
  return std::move(_rhs.insert(0, _lhs));
}

template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    _CharT _lhs, ShmBasicString<_CharT>&& _rhs) {
  return std::move(_rhs.insert(0, 1, _lhs));
}

template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    ShmBasicString<_CharT>&& _lhs, const _CharT* _rhs) {
  return std::move(_lhs.append(_rhs));
}

template <typename _CharT>
inline ShmBasicString<_CharT> operator+(
    ShmBasicString<_CharT>&& _lhs, _CharT _rhs) {
  return std::move(_lhs.append(1, _rhs));
}

// operator ==
/**
 *  @brief  Test equivalence of two strings.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *  @return  True if @a _lhs.compare(@a _rhs) == 0.  False otherwise.
 */
template <typename _CharT>
inline bool operator==(const ShmBasicString<_CharT>& _lhs,
                       const ShmBasicString<_CharT>& _rhs) {
  return _lhs.compare(_rhs) == 0;
}

/**
 *  @brief  Test equivalence of C string and string.
 *  @param _lhs  C string.
 *  @param _rhs  String.
 *  @return  True if @a _rhs.compare(@a _lhs) == 0.  False otherwise.
 */
template <typename _CharT>
inline bool operator==(const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
  return _rhs.compare(_lhs) == 0;
}

/**
 *  @brief  Test equivalence of string and C string.
 *  @param _lhs  String.
 *  @param _rhs  C string.
 *  @return  True if @a _lhs.compare(@a _rhs) == 0.  False otherwise.
 */
template <typename _CharT>
inline bool operator==(const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  return _lhs.compare(_rhs) == 0;
}

// operator !=
/**
 *  @brief  Test difference of two strings.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *  @return  True if @a _lhs.compare(@a _rhs) != 0.  False otherwise.
 */
template <typename _CharT>
inline bool operator!=(const ShmBasicString<_CharT>& _lhs,
                       const ShmBasicString<_CharT>& _rhs) {
  return !(_lhs == _rhs);
}

/**
 *  @brief  Test difference of C string and string.
 *  @param _lhs  C string.
 *  @param _rhs  String.
 *  @return  True if @a _rhs.compare(@a _lhs) != 0.  False otherwise.
 */
template <typename _CharT>
inline bool operator!=(const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
  return !(_lhs == _rhs);
}

/**
 *  @brief  Test difference of string and C string.
 *  @param _lhs  String.
 *  @param _rhs  C string.
 *  @return  True if @a _lhs.compare(@a _rhs) != 0.  False otherwise.
 */
template <typename _CharT>
inline bool operator!=(const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  return !(_lhs == _rhs);
}

// operator <
/**
 *  @brief  Test if string precedes string.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *  @return  True if @a _lhs precedes @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator<(const ShmBasicString<_CharT>& _lhs,
                      const ShmBasicString<_CharT>& _rhs) {
  return _lhs.compare(_rhs) < 0;
}

/**
 *  @brief  Test if string precedes C string.
 *  @param _lhs  String.
 *  @param _rhs  C string.
 *  @return  True if @a _lhs precedes @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator<(const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  return _lhs.compare(_rhs) < 0;
}

/**
 *  @brief  Test if C string precedes string.
 *  @param _lhs  C string.
 *  @param _rhs  String.
 *  @return  True if @a _lhs precedes @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator<(const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
  return _rhs.compare(_lhs) > 0;
}

// operator >
/**
 *  @brief  Test if string follows string.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *  @return  True if @a _lhs follows @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator>(const ShmBasicString<_CharT>& _lhs,
                      const ShmBasicString<_CharT>& _rhs) {
  return _lhs.compare(_rhs) > 0;
}

/**
 *  @brief  Test if string follows C string.
 *  @param _lhs  String.
 *  @param _rhs  C string.
 *  @return  True if @a _lhs follows @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator>(const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  return _lhs.compare(_rhs) > 0;
}

/**
 *  @brief  Test if C string follows string.
 *  @param _lhs  C string.
 *  @param _rhs  String.
 *  @return  True if @a _lhs follows @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator>(const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
  return _rhs.compare(_lhs) < 0;
}

// operator <=
/**
 *  @brief  Test if string doesn't follow string.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *  @return  True if @a _lhs doesn't follow @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator<=(const ShmBasicString<_CharT>& _lhs,
                       const ShmBasicString<_CharT>& _rhs) {
  return _lhs.compare(_rhs) <= 0;
}

/**
 *  @brief  Test if string doesn't follow C string.
 *  @param _lhs  String.
 *  @param _rhs  C string.
 *  @return  True if @a _lhs doesn't follow @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator<=(const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  return _lhs.compare(_rhs) <= 0;
}

/**
 *  @brief  Test if C string doesn't follow string.
 *  @param _lhs  C string.
 *  @param _rhs  String.
 *  @return  True if @a _lhs doesn't follow @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator<=(const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
  return _rhs.compare(_lhs) >= 0;
}

// operator >=
/**
 *  @brief  Test if string doesn't precede string.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *  @return  True if @a _lhs doesn't precede @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator>=(const ShmBasicString<_CharT>& _lhs,
                       const ShmBasicString<_CharT>& _rhs) {
  return _lhs.compare(_rhs) >= 0;
}

/**
 *  @brief  Test if string doesn't precede C string.
 *  @param _lhs  String.
 *  @param _rhs  C string.
 *  @return  True if @a _lhs doesn't precede @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator>=(const ShmBasicString<_CharT>& _lhs, const _CharT* _rhs) {
  return _lhs.compare(_rhs) >= 0;
}

/**
 *  @brief  Test if C string doesn't precede string.
 *  @param _lhs  C string.
 *  @param _rhs  String.
 *  @return  True if @a _lhs doesn't precede @a _rhs.  False otherwise.
 */
template <typename _CharT>
inline bool operator>=(const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
  return _rhs.compare(_lhs) <= 0;
}

/**
 *  @brief  Swap contents of two strings.
 *  @param _lhs  First string.
 *  @param _rhs  Second string.
 *
 *  Exchanges the contents of @a _lhs and @a _rhs in constant time.
 */
template <typename _CharT>
inline void swap(ShmBasicString<_CharT>& _lhs,
                 ShmBasicString<_CharT>& _rhs) {
  _lhs.swap(_rhs);
}



/**
 *  @brief  Write string to a stream.
 *  @param _os  Output stream.
 *  @param _str  String to write out.
 *  @return  Reference to the output stream.
 *
 *  Output characters of @a _str into os following the same rules as for
 *  writing a C string.
 */
template <typename _CharT>
inline std::basic_ostream<_CharT>& operator<<(
    std::basic_ostream<_CharT>& _os, const ShmBasicString<_CharT>& _str) {
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 586. string inserter not a formatted function
#ifdef _MSC_VER
  return std::_Insert_string(_os, _str.data(), _str.size());
#else
  return __ostream_insert(_os, _str.data(), _str.size());
#endif
}







  template <typename _CharT>
  const typename ShmBasicString<_CharT>::size_type
      ShmBasicString<_CharT>::npos;

  template <typename _CharT>
  void ShmBasicString<_CharT>::swap(ShmBasicString & _s) noexcept {
    if (this == &_s) return;


    if (_M_is_local())
      if (_s._M_is_local()) {
        if (length() && _s.length()) {
          _CharT _tmp_data[_S_local_capacity + 1];
          traits_type::copy(_tmp_data, _s._M_local_buf, _S_local_capacity + 1);
          traits_type::copy(_s._M_local_buf, _M_local_buf, _S_local_capacity + 1);
          traits_type::copy(_M_local_buf, _tmp_data, _S_local_capacity + 1);
        } else if (_s.length()) {
          traits_type::copy(_M_local_buf, _s._M_local_buf, _S_local_capacity + 1);
          _M_length(_s.length());
          _s._M_set_length(0);
          return;
        } else if (length()) {
          traits_type::copy(_s._M_local_buf, _M_local_buf, _S_local_capacity + 1);
          _s._M_length(length());
          _M_set_length(0);
          return;
        }
      } else {
        const size_type _tmp_capacity = _s._M_allocated_capacity;
        traits_type::copy(_s._M_local_buf, _M_local_buf, _S_local_capacity + 1);
        _M_data(_s._M_data());
        _s._M_data(_s._M_local_buf);
        _M_capacity(_tmp_capacity);
      }
    else {
      const size_type _tmp_capacity = _M_allocated_capacity;
      if (_s._M_is_local()) {
        traits_type::copy(_M_local_buf, _s._M_local_buf, _S_local_capacity + 1);
        _s._M_data(_M_data());
        _M_data(_M_local_buf);
      } else {
        pointer _tmp_ptr = _M_data();
        _M_data(_s._M_data());
        _s._M_data(_tmp_ptr);
        _M_capacity(_s._M_allocated_capacity);
      }
      _s._M_capacity(_tmp_capacity);
    }

    const size_type _tmp_length = length();
    _M_length(_s.length());
    _s._M_length(_tmp_length);
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::pointer
  ShmBasicString<_CharT>::_M_create(size_type & _capacity,
                                                   size_type _old_capacity) {
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 83.  String::npos vs. string::max_size()
    if (_capacity > max_size()) {
      throw std::out_of_range("ShmBasicString::_M_create max size fail");
    }

    // The below implements an exponential growth policy, necessary to
    // meet amortized linear time requirements of the library: see
    // http://gcc.gnu.org/ml/libstdc++/2001-07/msg00085.html.
    if (_capacity > _old_capacity && _capacity < 2 * _old_capacity) {
      _capacity = 2 * _old_capacity;
      // Never allocate a string bigger than max_size.
      if (_capacity > max_size()) _capacity = max_size();
    }

    // NB: Need an array of char_type[_capacity], plus a terminating
    // null char_type() element.
    return _M_dataplus.allocate(_capacity + 1);
  }

  // NB: This is the special case for Input Iterators, used in
  // istreambuf_iterators, etc.
  // Input Iterators have a cost structure very different from
  // pointers, calling for a different coding style.
  template <typename _CharT>
  template <typename _InIterator>
  void ShmBasicString<_CharT>::_M_construct(_InIterator _beg, _InIterator _end,
                                                           std::input_iterator_tag) {
    size_type _len = 0;
    size_type _capacity = size_type(_S_local_capacity);

    while (_beg != _end && _len < _capacity) {
      _M_data()[_len++] = *_beg;
      ++_beg;
    }

    try {
      while (_beg != _end) {
        if (_len == _capacity) {
          // Allocate more space.
          _capacity = _len + 1;
          pointer _another = _M_create(_capacity, _len);
          this->_S_copy(_another, _M_data(), _len);
          _M_dispose();
          _M_data(_another);
          _M_capacity(_capacity);
        }
        _M_data()[_len++] = *_beg;
        ++_beg;
      }
    }
    catch(std::exception& e) {
      _M_dispose();
      throw e;
    }

    _M_set_length(_len);
  }

  template <typename _CharT>
  template <typename _InIterator>
  void ShmBasicString<_CharT>::_M_construct(_InIterator _beg, _InIterator _end,
                                                           std::forward_iterator_tag) {
    // NB: Not required, but considered best practice.
    if (_beg == nullptr && _beg != _end)
      throw std::invalid_argument("ShmBasicString:: _M_construct null not valid");

    size_type _dnew = static_cast<size_type>(std::distance(_beg, _end));

    if (_dnew > size_type(_S_local_capacity)) {
      _M_data(_M_create(_dnew, size_type(0)));
      _M_capacity(_dnew);
    }

    // Check for out_of_range and length_error exceptions.
    try {
      this->_S_copy_chars(_M_data(), _beg, _end);
    }
    catch(std::exception& e) {
      _M_dispose();
      throw e;
    }

    _M_set_length(_dnew);
  }

  template <typename _CharT>
  void ShmBasicString<_CharT>::_M_construct(size_type _n, _CharT _c) {
    if (_n > size_type(_S_local_capacity)) {
      _M_data(_M_create(_n, size_type(0)));
      _M_capacity(_n);
    }

    if (_n) this->_S_assign(_M_data(), _n, _c);

    _M_set_length(_n);
  }

  template <typename _CharT>
  void ShmBasicString<_CharT>::_M_assign(const ShmBasicString& _str) {
    if (this != &_str) {
      const size_type _rsize = _str.length();
      const size_type _capacity = capacity();

      if (_rsize > _capacity) {
        size_type _new_capacity = _rsize;
        pointer _tmp = _M_create(_new_capacity, _capacity);
        _M_dispose();
        _M_data(_tmp);
        _M_capacity(_new_capacity);
      }

      if (_rsize) this->_S_copy(_M_data(), _str._M_data(), _rsize);

      _M_set_length(_rsize);
    }
  }

  template <typename _CharT>
  void ShmBasicString<_CharT>::reserve(size_type _res) {
    // Make sure we don't shrink below the current size.
    if (_res < length()) _res = length();

    const size_type _capacity = capacity();
    if (_res != _capacity) {
      if (_res > _capacity || _res > size_type(_S_local_capacity)) {
        pointer _tmp = _M_create(_res, _capacity);
        this->_S_copy(_tmp, _M_data(), length() + 1);
        _M_dispose();
        _M_data(_tmp);
        _M_capacity(_res);
      } else if (!_M_is_local()) {
        this->_S_copy(_M_local_data(), _M_data(), length() + 1);
        _M_destroy(_capacity);
        _M_data(_M_local_data());
      }
    }
  }

  template <typename _CharT>
  void ShmBasicString<_CharT>::_M_mutate(size_type _pos, size_type _len1,
                                                        const _CharT* _s, size_type _len2) {
    const size_type _how_much = length() - _pos - _len1;

    size_type _new_capacity = length() + _len2 - _len1;
    pointer _r = _M_create(_new_capacity, capacity());

    if (_pos) this->_S_copy(_r, _M_data(), _pos);
    if (_s && _len2) this->_S_copy(_r + _pos, _s, _len2);
    if (_how_much) this->_S_copy(_r + _pos + _len2, _M_data() + _pos + _len1, _how_much);

    _M_dispose();
    _M_data(_r);
    _M_capacity(_new_capacity);
  }

  template <typename _CharT>
  void ShmBasicString<_CharT>::_M_erase(size_type _pos, size_type _n) {
    const size_type _how_much = length() - _pos - _n;

    if (_how_much && _n) this->_S_move(_M_data() + _pos, _M_data() + _pos + _n, _how_much);

    _M_set_length(length() - _n);
  }

  template <typename _CharT>
  void ShmBasicString<_CharT>::resize(size_type _n, _CharT _c) {
    const size_type _size = this->size();
    if (_size < _n)
      this->append(_n - _size, _c);
    else if (_n < _size)
      this->_M_erase(_n, _size - _n);
  }

  template <typename _CharT>
  ShmBasicString<_CharT>& ShmBasicString<_CharT>::_M_append(
      const _CharT* _s, size_type _n) {
    const size_type _len = _n + this->size();

    if (_len <= this->capacity()) {
      if (_n) this->_S_copy(this->_M_data() + this->size(), _s, _n);
    } else
      this->_M_mutate(this->size(), size_type(0), _s, _n);

    this->_M_set_length(_len);
    return *this;
  }

  template <typename _CharT>
  template <typename _InputIterator>
  ShmBasicString<_CharT>& ShmBasicString<_CharT>::_M_replace_dispatch(
      const_iterator _i1, const_iterator _i2, _InputIterator _k1, _InputIterator _k2,
      std::false_type) {
    const ShmBasicString _s(_k1, _k2);
    const size_type _n1 = _i2 - _i1;
    return _M_replace(_i1 - begin(), _n1, _s._M_data(), _s.size());
  }

  template <typename _CharT>
  ShmBasicString<_CharT>& ShmBasicString<_CharT>::_M_replace_aux(
      size_type _pos1, size_type _n1, size_type _n2, _CharT _c) {
    _M_check_length(_n1, _n2, "ShmBasicString::_M_replace_aux");

    const size_type _old_size = this->size();
    const size_type _new_size = _old_size + _n2 - _n1;

    if (_new_size <= this->capacity()) {
      _CharT* _p = this->_M_data() + _pos1;

      const size_type _how_much = _old_size - _pos1 - _n1;
      if (_how_much && _n1 != _n2) this->_S_move(_p + _n2, _p + _n1, _how_much);
    } else
      this->_M_mutate(_pos1, _n1, 0, _n2);

    if (_n2) this->_S_assign(this->_M_data() + _pos1, _n2, _c);

    this->_M_set_length(_new_size);
    return *this;
  }

  template <typename _CharT>
  ShmBasicString<_CharT>& ShmBasicString<_CharT>::_M_replace(
      size_type _pos, size_type _len1, const _CharT* _s, const size_type _len2) {
    _M_check_length(_len1, _len2, "ShmBasicString::_M_replace");

    const size_type _old_size = this->size();
    const size_type _new_size = _old_size + _len2 - _len1;

    if (_new_size <= this->capacity()) {
      _CharT* _p = this->_M_data() + _pos;

      const size_type _how_much = _old_size - _pos - _len1;
      if (_M_disjunct(_s)) {
        if (_how_much && _len1 != _len2) this->_S_move(_p + _len2, _p + _len1, _how_much);
        if (_len2) this->_S_copy(_p, _s, _len2);
      } else {
        // Work in-place.
        if (_len2 && _len2 <= _len1) this->_S_move(_p, _s, _len2);
        if (_how_much && _len1 != _len2) this->_S_move(_p + _len2, _p + _len1, _how_much);
        if (_len2 > _len1) {
          if (_s + _len2 <= _p + _len1)
            this->_S_move(_p, _s, _len2);
          else if (_s >= _p + _len1)
            this->_S_copy(_p, _s + _len2 - _len1, _len2);
          else {
            const size_type _nleft = (_p + _len1) - _s;
            this->_S_move(_p, _s, _nleft);
            this->_S_copy(_p + _nleft, _p + _len2, _len2 - _nleft);
          }
        }
      }
    } else
      this->_M_mutate(_pos, _len1, _s, _len2);

    this->_M_set_length(_new_size);
    return *this;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::copy(_CharT * _s, size_type _n, size_type _pos) const {
    _M_check(_pos, "ShmBasicString::copy");
    _n = _M_limit(_pos, _n);
    if (_n) _S_copy(_s, _M_data() + _pos, _n);
    // 21.3.5.7 par 3: do not append null.  (good.)
    return _n;
  }



  template <typename _CharT>
  ShmBasicString<_CharT> operator+(
      const _CharT* _lhs, const ShmBasicString<_CharT>& _rhs) {
    typedef ShmBasicString<_CharT> _string_type;
    typedef typename _string_type::size_type size_type;
    const size_type _len = _string_type::traits_type::length(_lhs);
    _string_type _str;
    _str.reserve(_len + _rhs.size());
    _str.append(_lhs, _len);
    _str.append(_rhs);
    return _str;
  }

  template <typename _CharT>
  ShmBasicString<_CharT> operator+(
      _CharT _lhs, const ShmBasicString<_CharT>& _rhs) {
    typedef ShmBasicString<_CharT> _string_type;
    typedef typename _string_type::size_type size_type;
    _string_type _str;
    const size_type _len = _rhs.size();
    _str.reserve(_len + 1);
    _str.append(size_type(1), _lhs);
    _str.append(_rhs);
    return _str;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find(const _CharT* _s, size_type _pos, size_type _n)
      const {
    const size_type _size = this->size();
    const _CharT* _data = _M_data();

    if (_n == 0) return _pos <= _size ? _pos : npos;

    if (_n <= _size) {
      for (; _pos <= _size - _n; ++_pos)
        if (traits_type::eq(_data[_pos], _s[0]) &&
            traits_type::compare(_data + _pos + 1, _s + 1, _n - 1) == 0)
          return _pos;
    }
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find(_CharT _c, size_type _pos) const noexcept {
    size_type _ret = npos;
    const size_type _size = this->size();
    if (_pos < _size) {
      const _CharT* _data = _M_data();
      const size_type _n = _size - _pos;
      const _CharT* _p = traits_type::find(_data + _pos, _n, _c);
      if (_p) _ret = _p - _data;
    }
    return _ret;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::rfind(const _CharT* _s, size_type _pos, size_type _n)
      const {
    const size_type _size = this->size();
    if (_n <= _size) {
      _pos = std::min(size_type(_size - _n), _pos);
      const _CharT* _data = _M_data();
      do {
        if (traits_type::compare(_data + _pos, _s, _n) == 0) return _pos;
      } while (_pos-- > 0);
    }
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::rfind(_CharT _c, size_type _pos)
      const noexcept {
    size_type _size = this->size();
    if (_size) {
      if (--_size > _pos) _size = _pos;
      for (++_size; _size-- > 0;)
        if (traits_type::eq(_M_data()[_size], _c)) return _size;
    }
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find_first_of(const _CharT* _s, size_type _pos,
                                                       size_type _n) const {
    for (; _n && _pos < this->size(); ++_pos) {
      const _CharT* _p = traits_type::find(_s, _n, _M_data()[_pos]);
      if (_p) return _pos;
    }
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find_last_of(const _CharT* _s, size_type _pos,
                                                      size_type _n) const {
    size_type _size = this->size();
    if (_size && _n) {
      if (--_size > _pos) _size = _pos;
      do {
        if (traits_type::find(_s, _n, _M_data()[_size])) return _size;
      } while (_size-- != 0);
    }
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find_first_not_of(const _CharT* _s, size_type _pos,
                                                           size_type _n) const {
    for (; _pos < this->size(); ++_pos)
      if (!traits_type::find(_s, _n, _M_data()[_pos])) return _pos;
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find_first_not_of(_CharT _c, size_type _pos)
      const noexcept {
    for (; _pos < this->size(); ++_pos)
      if (!traits_type::eq(_M_data()[_pos], _c)) return _pos;
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find_last_not_of(const _CharT* _s, size_type _pos,
                                                          size_type _n) const {
    size_type _size = this->size();
    if (_size) {
      if (--_size > _pos) _size = _pos;
      do {
        if (!traits_type::find(_s, _n, _M_data()[_size])) return _size;
      } while (_size--);
    }
    return npos;
  }

  template <typename _CharT>
  typename ShmBasicString<_CharT>::size_type
  ShmBasicString<_CharT>::find_last_not_of(_CharT _c, size_type _pos)
      const noexcept {
    size_type _size = this->size();
    if (_size) {
      if (--_size > _pos) _size = _pos;
      do {
        if (!traits_type::eq(_M_data()[_size], _c)) return _size;
      } while (_size--);
    }
    return npos;
  }

  template <typename _CharT>
  int ShmBasicString<_CharT>::compare(size_type _pos, size_type _n,
                                                     const ShmBasicString& _str) const {
    _M_check(_pos, "ShmBasicString::compare");
    _n = _M_limit(_pos, _n);
    const size_type _osize = _str.size();
    const size_type _len = std::min(_n, _osize);
    int _r = traits_type::compare(_M_data() + _pos, _str.data(), _len);
    if (!_r) _r = _S_compare(_n, _osize);
    return _r;
  }

  template <typename _CharT>
  int ShmBasicString<_CharT>::compare(
      size_type _pos1, size_type _n1, const ShmBasicString& _str, size_type _pos2, size_type _n2)
      const {
    _M_check(_pos1, "ShmBasicString::compare");
    _str._M_check(_pos2, "ShmBasicString::compare");
    _n1 = _M_limit(_pos1, _n1);
    _n2 = _str._M_limit(_pos2, _n2);
    const size_type _len = std::min(_n1, _n2);
    int _r = traits_type::compare(_M_data() + _pos1, _str.data() + _pos2, _len);
    if (!_r) _r = _S_compare(_n1, _n2);
    return _r;
  }

  template <typename _CharT>
  int ShmBasicString<_CharT>::compare(const _CharT* _s) const {
    const size_type _size = this->size();
    const size_type _osize = traits_type::length(_s);
    const size_type _len = std::min(_size, _osize);
    int _r = traits_type::compare(_M_data(), _s, _len);
    if (!_r) _r = _S_compare(_size, _osize);
    return _r;
  }

  template <typename _CharT>
  int ShmBasicString<_CharT>::compare(size_type _pos, size_type _n1,
                                                     const _CharT* _s) const {
    
    _M_check(_pos, "ShmBasicString::compare");
    _n1 = _M_limit(_pos, _n1);
    const size_type _osize = traits_type::length(_s);
    const size_type _len = std::min(_n1, _osize);
    int _r = traits_type::compare(_M_data() + _pos, _s, _len);
    if (!_r) _r = _S_compare(_n1, _osize);
    return _r;
  }

  template <typename _CharT>
  int ShmBasicString<_CharT>::compare(size_type _pos, size_type _n1,
                                                     const _CharT* _s, size_type _n2) const {
    _M_check(_pos, "ShmBasicString::compare");
    _n1 = _M_limit(_pos, _n1);
    const size_type _len = std::min(_n1, _n2);
    int _r = traits_type::compare(_M_data() + _pos, _s, _len);
    if (!_r) _r = _S_compare(_n1, _n2);
    return _r;
  }

  

  




#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

template<class T>
struct Iter;

template<typename T>
struct Slice;

using str = Slice<const char>;

template<typename T>
struct Slice
{
    using type_t    = T;
    using const_t   = ustd::const_t<type_t>;
    using ref_t     = ustd::ref_t<type_t>;
    using cref_t    = ustd::const_t<ref_t>;

    using size_t    = u32;
    using offset_t  = i32;

    type_t* _data       = nullptr;
    size_t  _size       = 0;
    size_t  _capacity   = 0;

#pragma region ctor
    // ctor:
    constexpr Slice() noexcept = default;

    // ctor:
    constexpr Slice(T* ptr, size_t length, size_t capacity = 0)
        : _data{ ptr }, _size{ length }, _capacity{ capacity }
    {}

    // ctor:
    template<size_t N>
    constexpr Slice(const T(&s)[N])
        : Slice{ s, $is_same<const_t, const char> ? N - 1 : N }
    {}

    // convert: const
    constexpr operator Slice<const_t>() const noexcept {
        return { _data, _size, _capacity };
    }

#pragma endregion

#pragma region property
    // property[r]: data
    __declspec(property(get = get_data)) type_t* data;
    fn get_data() const noexcept -> type_t* {
        return _data;
    }

    // property[r]: len
    __declspec(property(get = get_len)) size_t len;
    fn get_len() const noexcept -> size_t {
        return _size;
    }

    // property[r]: capacity
    __declspec(property(get = get_capacity)) size_t capacity;
    fn get_capacity() const noexcept -> size_t {
        return _capacity;
    }

    // property[r]: is_empty 
    fn is_empty() const noexcept -> bool {
        return _size == 0;
    }

    // property[r]: is_full
    fn is_full() const noexcept -> bool {
        return _size == _capacity;
    }
#pragma endregion

#pragma region access
    // operator: []
    fn operator[](size_t i) const noexcept -> cref_t {
        return _data[i];
    }

    // operator: []
    fn operator[](size_t i) noexcept -> ref_t {
        return _data[i];
    }
#pragma endregion

#pragma region method

    // method: slice
    fn slice(size_t beg, size_t end) const noexcept-> Slice {
        return { _data + beg, end - beg + 1 };
    }

    fn slice(offset_t beg, offset_t end) const noexcept -> Slice {
        let beg_pos = size_t(beg + offset_t(_size)) % _size;
        let end_pos = size_t(end + offset_t(_size)) % _size;
        return { _data + beg_pos, end_pos - beg_pos + 1 };
    }

    // method: clear
    fn clear() noexcept -> void {
        let size = _size;

        if (!trivial<T>::$dtor) {
            for (mut i = 0u; i < size; ++i) {
                _data[i].~T();
            }
        }

        _size = 0;
    }

    // operator: eq
    template<class U>
    fn operator==(Slice<U> other) const noexcept -> bool {
        if (_size != other._size) return false;
        if (_data == other._data) return true;

        for (u32 i = 0; i < _size; ++i) {
            if (_data[i] != other._data[i]) return false;
        }
        return true;
    }

    // operator: neq
    template<class U>
    fn operator!=(Slice<U> other) const noexcept -> bool {
        let is_eq = (*this == other);
        return !is_eq;
    }

    // method: starts_with
    template<class U>
    fn starts_with(Slice<U> prefix) const noexcept -> bool {
        if (prefix._size > _size) {
            return false;
        }

        let tmp = this->slice(0u, prefix._size - 1);
        let res = tmp == prefix;
        return res;
    }

    // method: ends_with
    template<class U>
    fn ends_with(Slice<U> suffix) const noexcept -> bool {
        static_assert($is<ustd::const_t<U>, const_t>);

        if (suffix._size > _size) {
            return false;
        }

        let tmp = this->slice(_size - 1 - suffix._size, _size - 1);
        let res = tmp == suffix;
        return res;
    }

    // method: contains
    template<class U>
    fn contains(U&& item) const noexcept -> bool {
        for (mut i = 0u; i < _size; ++i) {
            if (_data[i] == item) {
                return true;
            }
        }
        return false;
    }

    // method: replace
    template<class ...U>
    fn replace(const T& from, U&& ...u) noexcept -> void {
        let cnt = _size;
        for (mut i = 0u; i < cnt; ++i) {
            if (_data[i] == from) {
                new(&_data[i])T(as_fwd<U>(u)...);
            }
        }
    }

    // method: find
    template<class ...U>
    fn find(U&& ...u) const noexcept -> Option<size_t> {
        let cnt = _size;
        for (mut i = 0u; i < cnt; ++i) {
            if (_data[i] == T(as_fwd<U>(u)...) ) {
                return Option<size_t>::Some(i);
            }
        }
        return Option<size_t>::None();
    }

    // method: find
    template<class ...U>
    fn rfind(U&& ...u) const noexcept -> Option<size_t> {
        let cnt = _size;
        for (mut i = cnt; i != 0; --i) {
            if (_data[i] == T(as_fwd<U>(u)...)) {
                return Option<size_t>::Some(i);
            }
        }
        return Option<size_t>::None();
    }

#pragma endregion

#pragma region iter
    // iter:
    fn into_iter() const noexcept -> Iter<const_t> {
        return { _data, _size };
    }

    fn into_iter() noexcept -> Iter<type_t> {
        return { _data, _size };
    }
#pragma endregion

#pragma region push/pop
public:
    // method: push
    template<class ...U>
    fn push(U&& ...u) noexcept -> Option<Slice&> {
        if (_size + 1 > _capacity) {
            return Option<Slice&>::None();
        }
        _push(as_fwd<U>(u)...);
        return Option<Slice&>::Some(*this);
    }

    // method: push
    template<class ...U>
    fn pushn(u32 n, U&& ...u) noexcept -> Option<Slice&> {
        if (_size + n > _capacity) {
            return Option<Slice&>::None();
        }

        _pushn(n, as_fwd<U>(u)...);
        return Option<Slice&>::Some(*this);
    }

    // method: push_slice
    Option<Slice&> push_slice(Slice<const T> v) noexcept _if(trivial<T>::$copy) {
        if (_size + v._size > _capacity) {
            return Option<Slice&>::None();
        }
        _push_slice(v);
        return Option<Slice&>::Some(*this);
    }

    // method: pop
    fn pop() noexcept -> Option<T> {
        return _pop();
    }

protected:
    // method: push
    template<class ...U>
    fn _push(U&& ...u) noexcept -> void {
        ustd::ctor(&_data[_size++], as_fwd<U>(u)...);
    }

    // method: push
    template<class ...U>
    fn _pushn(u32 n, U&& ...u) noexcept -> void {
        for (mut i = 0u; i < n; ++i) {
            ustd::ctor(&_data[_size++], as_fwd<U>(u)...);
        }
    }

    // method: push_slice
    void _push_slice(Slice<const T> v) noexcept _if(trivial<T>::$copy) {
        let dst = _data + _size;
        let src = v._data;
        ustd::mcpy(dst, src, v._size);

        _size += v._size;
    }

    // method: pop
    fn _pop() noexcept -> Option<T> {
        if (_size == 0) {
            return Option<T>::None();
        }
        mut& res = _data[(_size--) - 1];
        return Option<T>::Some(as_mov(res));
    }
#pragma endregion

};

}


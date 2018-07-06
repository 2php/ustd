#pragma once

#include "ustd/core/builtin.h"
#include "ustd/core/option.h"

namespace ustd
{

template<class T>
struct Iter;

template<typename T>
struct Slice;

using str = Slice<const char>;

template<class T>
fn mcpy(T* dst, const T* src, u64 size) -> void;

template<typename T>
struct Slice
{
    T* _data       = nullptr;
    u32 _size       = 0;
    u32 _capacity   = 0;

#pragma region ctor
    // ctor:
    constexpr Slice() noexcept = default;

    // ctor:
    constexpr Slice(T* ptr, u32 length, u32 capacity = 0)
        : _data{ ptr }, _size{ length }, _capacity{ capacity }
    {}

    // ctor:
    template<u32 N>
    constexpr Slice(const T(&s)[N])
        : Slice{ s, $is_same<T, const char> ? N - 1 : N }
    {}

    // convert: const
    constexpr operator Slice<const T>() const noexcept {
        return { _data, _size, _capacity };
    }

#pragma endregion

#pragma region property
    fn data() const noexcept -> T* {
        return _data;
    }

    fn len() const noexcept -> u32 {
        return _size;
    }

    fn size() const noexcept -> u32 {
        return _size;
    }

    fn count() const noexcept -> u32 {
        return _size;
    }

    fn capacity() const noexcept -> u32 {
        return _capacity;
    }

    fn is_empty() const noexcept -> bool {
        return _size == 0;
    }

    fn is_full() const noexcept -> bool {
        return _size == _capacity;
    }
#pragma endregion

#pragma region access
    // operator: []
    fn operator[](u32 i) const noexcept -> const T& {
        return _data[i];
    }

    // operator: []
    fn operator[](u32 i) noexcept -> T& {
        return _data[i];
    }
#pragma endregion

#pragma region method

    // method: slice
    fn slice(u32 beg, u32 end) const noexcept-> Slice {
        return { _data + beg, end - beg + 1 };
    }

    fn slice(i32 beg, i32 end) const noexcept -> Slice {
        let beg_pos = u32(beg + i32(_size)) % _size;
        let end_pos = u32(end + i32(_size)) % _size;
        return { _data + beg_pos, end_pos - beg_pos + 1 };
    }

    // method: clear
    fn clear() noexcept -> void {
        let cnt = _size;
        _size   = 0;

        if (!trivial<T>::$dtor) {
            for (mut i = 0u; i < cnt; ++i) {
                ustd::dtor(&_data[i]);
            }
        }
    }

    // operator: eq
    fn operator==(Slice<const T> other) const noexcept -> bool {
        if (_size != other._size) return false;
        if (_data == other._data) return true;

        for (mut i = 0u; i < _size; ++i) {
            if (_data[i] != other._data[i]) {
                return false;
            }
        }
        return true;
    }

    // operator: neq
    fn operator!=(Slice<const T> other) const noexcept -> bool {
        let is_eq = (*this == other);
        return !is_eq;
    }

    // method: starts_with
    fn starts_with(Slice<const T> prefix) const noexcept -> bool {
        if (prefix._size > _size) {
            return false;
        }

        let tmp = this->slice(0u, prefix._size - 1);
        let res = tmp == prefix;
        return res;
    }

    // method: ends_with
    fn ends_with(Slice<const T> suffix) const noexcept -> bool {
        if (suffix._size > _size) {
            return false;
        }

        let tmp = this->slice(_size - 1 - suffix._size, _size - 1);
        let res = tmp == suffix;
        return res;
    }

    // method: contains
    fn contains(const T& val) const noexcept -> bool {
        for (mut i = 0u; i < _size; ++i) {
            if (_data[i] == val) {
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
                ustd::ctor(&_data[i], as_fwd<U>(u)...);
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
    fn into_iter() const noexcept -> Iter<const_t<T>> {
        return { _data, _size };
    }

    fn into_iter() noexcept -> Iter<T> {
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


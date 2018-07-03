#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

template<typename T>
class Option;

template<typename T>
struct Iter
{
    using type_t = T;
    using size_t = u32;

    type_t* _data;
    size_t  _size;
    size_t  _index;

    Iter(type_t* data, size_t size) : _data{ data }, _size{ size }, _index{ 0 }
    {}

    // get value
    fn next() noexcept -> Option<T&> {
        if (_index >= _size) return Option<T&>::None();
        return Option<T&>::Some(_data[_index++]);
    }
};

template<class I, class T = typename I::type_t>
struct _Foreach
{
    using O = decltype(declval<I>().next());

    I   _itr;
    O   _opt;

    _Foreach(I itr): _itr(itr)
    {}

    fn operator++() noexcept -> _Foreach& {
        mut res = _itr.next();
        ustd::swap(_opt, res);
        return *this;
    }

    fn operator*() noexcept -> ref_t<decltype(_opt._val)> {
        return _opt._val;
    }

    fn operator!=(const _Foreach& other) const noexcept {
         let res = _opt._valid != other._opt._valid;
         return res;
    }
};

template<class I>
fn begin(I itr) noexcept -> _Foreach<I, typename I::type_t> {
    mut res = _Foreach<I>(itr);
    ++res;
    return res;
}

template<class I>
fn end(I itr) noexcept -> _Foreach<I, typename I::type_t> {
    mut res = _Foreach<I>(itr);
    return res;
}

}

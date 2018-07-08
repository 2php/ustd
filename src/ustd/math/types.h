#pragma once

#include "ustd/core/builtin.h"
#include "ustd/core/vec.h"

namespace ustd
{
class Formatter;
}

namespace ustd::math
{

/* predef */
struct _ndvec_t
{};

template<class T, u32 N>
struct ndvec_t;

template<class T, u32 N>
struct NDSlice;

template<class T, u32 N>
struct NDArray;

/* ndvec */
template<class T, u32 N>
struct ndvec_t: public _ndvec_t
{
    using val_t = T;
    constexpr static let $rank = N;
};

template<typename T>
constexpr let $is_ndvec = $is_base<_ndvec_t, T>;

template<class F, class ...T>
using res_t = decltype(declval<F>()(declval<typename T::val_t>()...));

/* scalar */
template<class T>
struct scalar_t: ndvec_t<T, 0>
{
    using vec_t = scalar_t;

    T   _val;

    explicit scalar_t(T val): _val(val)
    {}

    template<class ...U>
    fn operator()(const U& ...u) const noexcept -> const T& {
        return _val;
    }

    template<class ...U>
    fn operator()(const U& ...u) noexcept -> T& {
        return _val;
    }
};

template<class T, class F>
fn operator<<=(scalar_t<T>& dst, const F& src) noexcept -> scalar_t<T>& {
    let val = src();
    dst._val = val;
    return dst;
}

/* to: ndvec */
template<class T>
fn to_ndvec(const T& t) {
    if constexpr(trait<T>::$num) {
        return scalar_t<T>(t);
    }
    if constexpr($is_ndvec<T>) {
        return typename T::vec_t(t);
    }
}

}

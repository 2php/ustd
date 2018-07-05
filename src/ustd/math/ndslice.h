#pragma once

#include "ustd/core.h"

namespace ustd::math
{

template<typename T, u32 N = 1>
struct NDSlice;

#pragma region detail
template<class ...U>
constexpr static fn _slice_rank(u32 s, U ...u) -> u32 {
    if constexpr(sizeof...(U)==0) {
        return s == 1u ? 0u : 1u;
    }
    else {
        return (s == 1u ? 0u : 1u) + _slice_rank(u...);
    }
}

template<u32 I0, u32 ...Is>
constexpr static fn _make_step(immut_t<u32, I0, Is...>, const vec<u32, 1+sizeof...(Is)>& dims) -> vec<i32, 1+sizeof...(Is)> {
    return { 1, i32(idx_reduce<ops::Mul>(seq_t<Is>{}, dims))... };
}
#pragma endregion

template<typename T, u32 N>
struct NDSlice
{
    using type_t  = T;
    using size_t  = u32;
    using idxs_t  = vec<i32, N>;
    using dims_t  = vec<u32, N>;

    static constexpr u32 $rank = N;

    type_t* _data;
    dims_t  _dims;
    idxs_t  _step;

#pragma region ctor/dtor
    // constructor
    constexpr NDSlice() noexcept
        : _data(nullptr), _dims(), _step()
    {}

    // constructor
    constexpr NDSlice(T* data, dims_t dims, idxs_t step) noexcept
        : _data(data), _dims(dims), _step(step)
    {}

    // constructor
    constexpr NDSlice(T* data, dims_t dims) noexcept
        : _data(data), _dims(dims), _step(_make_step(seq_t<N>{}, dims))
    {}

    // constructor
    constexpr NDSlice(T* data, const u32(&dims)[$rank], const i32(&step)[$rank]) noexcept
        : NDSlice(data, dims_t(dims), idxs_t(step))
    {}

    // constructor
    constexpr NDSlice(T* data, const u32(&dims)[$rank]) noexcept
        : NDSlice(data, dims_t(dims))
    {}
#pragma endregion

#pragma region property
    // property[r]: data
    fn data() noexcept -> T* {
        return _data;
    }

    // property[r]: rank
    fn rank() noexcept -> size_t {
        return N;
    }

    // property[r]: dims
    fn dims() const noexcept -> const dims_t&{
        return _dims;
    }

    // property[r]: dims
    fn dims(u32 idx) const noexcept -> size_t {
        return idx < N ? _dims[idx] : 0;
    }

    // property[r]: step
    fn step() const noexcept -> const idxs_t& {
        return _step;
    }

    // property[r]: count
    fn count() const noexcept -> size_t {
        return idx_reduce<ops::Mul>(seq_t<N>{}, _dims);
    }

    // method: is_empty
    fn is_empty() const noexcept -> bool {
        return count() == 0;
    }
#pragma endregion

#pragma region access
    // operator: ()
    template<typename ...R, class=when<sizeof...(R)==N> >
    fn operator()(R ...idxs) noexcept -> T& {
        let offset = _get_offset(seq_t<N>{}, idxs...);
        return _data[offset];
    }

    // operator: ()
    template<typename ...R, class = when<sizeof...(R) == N> >
    fn operator()(R ...idxs) const noexcept -> const T&{
        let offset = _get_offset(seq_t<N>{}, idxs...);
        return _data[offset];
    }

    // operator: []
    fn operator[](dims_t idx) noexcept -> T& {
        let offset = _get_offset(seq_t<N>{}, idx);
        return _data[offset];
    }

    // operator: []
    fn operator[](dims_t idx) const noexcept -> const T&{
        let offset = _get_offset(seq_t<N>{}, idx);
        return _data[offset];
    }

    // operator: []
    fn operator[](idxs_t idx) noexcept -> T& {
        let offset = _get_offset(seq_t<N>{}, idx);
        return _data[offset];
    }

    // operator: []
    fn operator[](idxs_t idx) const noexcept -> const T&{
        let offset = _get_offset(seq_t<N>{}, idx);
        return _data[offset];
    }
#pragma endregion

#pragma region method
    // method: slice
    template<u32 ...M>
    fn slice(const i32(&...s)[M]) const noexcept -> NDSlice<T, _slice_rank(M...)> {
        static_assert(sizeof...(s) == N, "ustd::math::NdSlice::slice_rank: invalid arguments");
        return slice1(seq_t<N>{}, s...).select_dims(idx_t<(M != 1)...>{});
    }

    // method: slice
    template<u32 ...M>
    fn slice(const u32(&...s)[M]) const noexcept ->  NDSlice<T, _slice_rank(M...)> {
        static_assert(sizeof...(s) == N, "ustd::math::NdSlice::slice_rank: invalid arguments");
        return slice1(seq_t<N>{}, s...).select_dims(idx_t<(M != 1)...>{});
    }

#pragma endregion

private:
    /* method: access */
    template<u32 ...I, typename ...U>
    fn _get_offset(immut_t<u32, I...>, U ...idxs) const -> i32 {
        return sum((i32(_step[I]) * i32(idxs))...);
    }

    /* method: access */
    template<u32 ...I, typename U>
    fn _get_offset(immut_t<u32, I...>, vec<U, N> idx) const -> i32 {
        return sum((i32(_step[I]) * i32(idx[I]))...);
    }

    /* method: NView */
    template<u32 ...I, typename ...R, u32 ...M>
    fn slice1(immut_t<u32, I...>, const R(&...s)[M]) const noexcept -> NDSlice<T, sizeof...(I)> {
        return { _data + get_offset_by_dims(immut_t<u32, I...>{}, s[0]...),{ get_size_by_dim<I>(s)... }, _step._arr };
    }

    /* method: NView */
    template<u32 ...I>
    fn select_dims(immut_t<u32, I...>) const noexcept -> NDSlice<T, sizeof...(I)> {
        return { _data,{ _dims[I]... },{ _step[I]... } };
    }

    /* method: NView */
    template<u32 I, typename R>
    fn get_offset_by_dim(R idx) const noexcept -> u32 {
        return idx >= 0 ? u32(idx) : _dims[I] - u32(0 - idx);
    }

    /* method: NView */
    template<u32 ...I, typename ...R>
    fn get_offset_by_dims(immut_t<u32, I...>, R ...idx) const noexcept -> u32 {
        return sum((get_offset_by_dim<I>(idx)*_step[I])...);
    }

    /* method: NView */
    template<u32 I, typename R, u32 M>
    fn get_size_by_dim(const R(&s)[M]) const noexcept -> u32 {
        return get_offset_by_dim<I>(s[M - 1]) - get_offset_by_dim<I>(s[0]) + 1;
    }
};

// scalar
template<typename T>
struct NDSlice<T, 0>
{
    using type_t = T;
    constexpr static const u32 $rank = 0;

    type_t _val;

    constexpr NDSlice(const T& val) noexcept : _val(val)
    {}

    // property[r]: dims
    fn dims(u32) const noexcept -> u32 {
        return 0;
    }

    // operator: ()
    template<typename ...I>
    fn operator()(I ...) const noexcept -> const type_t& {
        return _val;
    }

    // operator: ()
    template<typename ...I>
    fn operator()(I ...) noexcept  -> type_t& {
        return _val;
    }
};

template<class T>
using Scalar = NDSlice<T,0>;

template<class T, u32 N>
void sfmt(Formatter& fmt, const NDSlice<T, N>& v) {
    if constexpr(N == 0) {
        fmt.push_str("[]");
    }
    if constexpr(N == 1) {
        let  nx = v.dims[0];

        fmt.push_str("\n");
        for (mut ix = 0u; ix < nx; ++ix) {
            let& val = v(ix);

            fmt.push_str("    ");
            fmt(val);
            fmt.push_str("\n");
        }
    }
    if constexpr(N == 2) {
        let nx = v.dims[0];
        let ny = v.dims[1];

        fmt.push_str("\n");
        for (mut ix = 0u; ix < nx; ++ix) {
            fmt.push_str("    ");
            for (mut iy = 0u; iy < ny; ++iy) {
                let val = v(ix, iy);
                fmt(val);
                fmt.push_str(", ");
            }
            fmt.push_str("\n");
        }
    }
}

template<typename F, class Y, class X>
void foreach(Y& y, const X& x) {
    let f = F();

    if constexpr(Y::$rank == 0) {
        f(y(), x());
    }

    if constexpr(Y::$rank == 1) {
        let nx = y._dims[0];

        for (mut ix = 0u; ix < nx; ++ix) {
            f(y(ix), x(ix));
        }
    }
    if constexpr(Y::$rank == 2) {
        let nx = y._dims[0];
        let ny = y._dims[1];

        for (mut iy = 0u; iy < ny; ++iy) {
            for (mut ix = 0u; ix < nx; ++ix) {
                f(y(ix, iy), x(ix, iy));
            }
        }
    }
    if constexpr(Y::$rank == 3) {
        let nx = y._dims[0];
        let ny = y._dims[1];
        let nz = y._dims[2];

        for (mut iz = 0u; iz < nz; ++iz) {
            for (mut iy = 0u; iy < ny; ++iy) {
                for (mut ix = 0u; ix < nx; ++ix) {
                    f(y(ix, iy, iz), x(ix, iy, iz));
                }
            }
        }
    }
}


}

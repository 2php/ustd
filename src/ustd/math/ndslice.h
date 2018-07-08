#pragma once

#include "ustd/math/types.h"
#include "ustd/core/fmt.h"

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

template<u32 I0, u32 ...Is, typename V>
constexpr static fn _make_step(immut_t<u32, I0, Is...>, const V& dims) -> vec<i32, 1+sizeof...(Is)> {
    return { 1, i32(idx_reduce<ops::Mul>(seq_t<Is>(), dims))... };
}

#pragma endregion

template<typename T, u32 N>
struct NDSlice: public ndvec_t<T,N>
{
    using u32xN = vec<u32,N>;
    using i32xN = vec<i32,N>;
    using vec_t = NDSlice;

    T*     _data;
    u32xN  _dims;
    i32xN  _step;

#pragma region ctor/dtor
    constexpr NDSlice() noexcept
        : _data(nullptr), _dims(), _step()
    {}

    constexpr NDSlice(T* data, u32xN dims, i32xN step) noexcept
        : _data(data), _dims(dims), _step(step)
    {}

    constexpr NDSlice(T* data, u32xN dims) noexcept
        : _data(data), _dims(dims), _step(_make_step(seq_t<N>(), dims))
    {}

    constexpr NDSlice(T* data, const u32(&dims)[N], const i32(&step)[N]) noexcept
        : NDSlice(data, u32xN(dims), i32xN(step))
    {}

    constexpr NDSlice(T* data, const u32(&dims)[N]) noexcept
        : NDSlice(data, u32xN(dims))
    {}
#pragma endregion

#pragma region property
    fn data() noexcept -> T* {
        return _data;
    }

    // property[r]: dims
    fn dims() const noexcept -> const u32xN&{
        return _dims;
    }

    // property[r]: dims
    fn dims(u32 idx) const noexcept -> u32 {
        return idx < N ? _dims[idx] : 0;
    }

    // property[r]: step
    fn step() const noexcept -> const i32xN& {
        return _step;
    }

    // property[r]: count
    fn count() const noexcept -> u32 {
        return idx_reduce<ops::Mul>(seq_t<N>(), _dims);
    }

    // method: is_empty
    fn is_empty() const noexcept -> bool {
        let cnt = count();
        return cnt == 0;
    }
#pragma endregion

#pragma region method
    // operator: ()
    template<typename ...R, class=when<sizeof...(R)==N> >
    fn operator()(R ...idxs) noexcept -> T& {
        let offset = _get_offset(seq_t<N>{}, idxs...);
        return _data[offset];
    }

    // operator: ()
    template<typename ...R, class=when<sizeof...(R)==N> >
    fn operator()(R ...idxs) const noexcept -> const T&{
        let offset = _get_offset(seq_t<N>{}, idxs...);
        return _data[offset];
    }

    // method: slice
    template<class ...I, u32 ...K, class=when<sizeof...(I) == N> >
    fn slice(const I(&...s)[K]) const noexcept -> NDSlice<T, _slice_rank(K...)> {
        return slice1(seq_t<N>{}, s...).select_dims(idx_t<(K != 1)...>{});
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

template<class T>
fn trait_sfmt(Formatter& fmt, const NDSlice<T, 1>& v) -> void {
    let  dims = v.dims();

    fmt.push_str("[\n");
    for (mut ix = 0u; ix < dims.x; ++ix) {
        let val = v(ix);

        fmt.push_str("    ");
        fmt(val);
        (ix+1 != dims.x) ? fmt.push_str(",\n") : fmt.push_str("\n");
    }
    fmt.push_str("]");
}

template<class T>
fn trait_sfmt(Formatter& fmt, const NDSlice<T,2>& v) -> void {
    let dims = v.dims();

    fmt.push_str("[\n");
    for (mut ix = 0u; ix < dims.x; ++ix) {
        fmt.push_str("    [");
        for (mut iy = 0u; iy < dims.y; ++iy) {
            let val = v(ix, iy);
            fmt(val);
            (iy+1 != dims.y) ? fmt.push_str(", ") : fmt.push_str("]");
        }
        (ix+1 != dims.x) ? fmt.push_str(",\n") : fmt.push_str("\n");                
    }
    fmt.push_str("]");
}

template<class O, class T, class F>
fn foreach(NDSlice<T,1>& dst, const F& src) -> void {
    let dims = dst.dims();
    for(mut x = 0u; x < dims.x; ++x) {
        dst(x) = src(x);
    }
}

template<class O, class T, class F>
fn foreach(NDSlice<T,2>& dst, const F& src) -> void {
    let dims = dst.dims();
    
    for(mut x = 0u; x < dims.x; ++x) {
        for(mut y = 0u; y < dims.y; ++y) {
            dst(x, y) = src(x, y);
        }
    }
}

template<class O, class T, class F>
fn foreach(NDSlice<T,3>& dst, const F& src) -> void {
    let dims = dst.dims();
    
    for(mut x = 0u; x < dims.x; ++x) {
        for(mut y = 0u; y < dims.y; ++y) {
            for(mut z = 0u; z < dims.z; ++z) {
                dst(x, y, z) = src(x, y, z);
            }
        }
    }
}

template<class O, class T, class F>
fn foreach(NDSlice<T,4>& dst, const F& src) -> void {
    let dims = dst.dims();
    
    for(mut x = 0u; x < dims.x; ++x) {
        for(mut y = 0u; y < dims.y; ++y) {
            for(mut z = 0u; z < dims.z; ++z) {
                for(mut w = 0u; w < dims.w; ++w) {
                    dst(x, y, z, w) = src(x, y, z, w);
                }
            }
        }
    }
}

template<class T, u32 N, class U>
fn operator<<=(NDSlice<T, N>& dst, const U& src) noexcept -> NDSlice<T, N>& {
    foreach<ops::SetTo>(dst, to_ndvec(src));
    return dst;
}

template<class T, u32 N, class U>
fn operator+=(NDSlice<T, N>& dst, const U& src) noexcept -> NDSlice<T, N>& {
    foreach<ops::AddTo>(dst, to_ndvec(src));
    return dst;
}

template<class T, u32 N, class U>
fn operator-=(NDSlice<T, N>& dst, const U& src) noexcept -> NDSlice<T, N>& {
    foreach<ops::SubTo>(dst, to_ndvec(src));
    return dst;
}

template<class T, u32 N, class U>
fn operator*=(NDSlice<T, N>& dst, const U& src) noexcept -> NDSlice<T, N>& {
    foreach<ops::MulTo>(dst, _to_ndslice(src));
    return dst;
}

template<class T, u32 N, class U>
fn operator/=(NDSlice<T, N>& dst, const U& src) noexcept -> NDSlice<T, N>& {
    foreach<ops::DivTo>(dst, to_ndvec(src));
    return dst;
}

}

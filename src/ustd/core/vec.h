#pragma once

#include "ustd/core/builtin.h"
#include "ustd/core/ops.h"

namespace ustd
{

template<typename T, u32 N>
struct vec
{
    T _arr[N];

    vec() noexcept = default;

    template<u32 ...I, class V>
    vec(immut_t<u32, I...>, const V& v) noexcept
        : _arr{ T(v[I])... }
    {}

    vec(const T(&arr)[N]) noexcept : vec(seq_t<N>{}, arr)
    {}

    template<class ...Us, class=when<1 + sizeof...(Us) == N> >
    vec(const T &t, const Us& ...s) noexcept 
        : _arr{ t, s... }
    {}

    explicit operator bool() const noexcept {
        return ustd::idx_reduce<ops::And>(seq_t<N>{}, _arr);
    }

    fn operator[](u32 idx)       noexcept ->       T& { return _arr[idx]; }
    fn operator[](u32 idx) const noexcept -> const T& { return _arr[idx]; }
};

template<typename T>
struct vec<T,1>
{
    static constexpr let $rank = 1;
    union {
        T _arr[$rank];
        struct { T x;  };
    };

    vec() noexcept = default;

    template<u32 ...I, class V>
    vec(immut_t<u32, I...>, const V& v) noexcept
        : _arr{ T(v[I])... }
    {}

    vec(const T(&arr)[$rank]) noexcept : vec(seq_t<$rank>{}, arr)
    {}

    template<class ...Us, class = when<1 + sizeof...(Us) == $rank> >
    vec(const T& t) noexcept
        : _arr{ t}
    {}

    explicit operator bool() const noexcept {
        return ustd::idx_reduce<ops::And>(seq_t<$rank>{}, _arr);
    }

    fn operator[](u32 idx)       noexcept ->       T& { return _arr[idx]; }
    fn operator[](u32 idx) const noexcept -> const T& { return _arr[idx]; }
};

template<typename T>
struct vec<T,2>
{
    static constexpr let $rank = 2;
    union {
        T _arr[$rank];
        struct { T x, y; };
    };

    vec() noexcept = default;

    template<u32 ...I, class V>
    vec(immut_t<u32, I...>, const V& v) noexcept
        : _arr{ T(v[I])... }
    {}

    vec(const T(&arr)[$rank]) noexcept : vec(seq_t<$rank>{}, arr)
    {}

    template<class ...Us, class = when<1 + sizeof...(Us) == $rank> >
    vec(const T& t, const Us& ...s) noexcept
        : _arr{ t, s... }
    {}

    explicit operator bool() const noexcept {
        return ustd::idx_reduce<ops::And>(seq_t<$rank>{}, _arr);
    }

    fn operator[](u32 idx)       noexcept ->       T& { return _arr[idx]; }
    fn operator[](u32 idx) const noexcept -> const T& { return _arr[idx]; }
};

template<typename T>
struct vec<T, 3>
{
    static constexpr let $rank = 3;
    union {
        T _arr[$rank];
        struct { T x, y, z; };
    };

    vec() noexcept = default;

    template<u32 ...I, class V>
    vec(immut_t<u32, I...>, const V& v) noexcept
        : _arr{ T(v[I])... }
    {}

    vec(const T(&arr)[$rank]) noexcept : vec(seq_t<$rank>{}, arr)
    {}

    template<class ...Us, class = when<1 + sizeof...(Us) == $rank> >
    vec(const T& t, const Us& ...s) noexcept
        : _arr{ t, s... }
    {}

    explicit operator bool() const noexcept {
        return ustd::idx_reduce<ops::And>(seq_t<$rank>{}, _arr);
    }

    fn operator[](u32 idx)       noexcept ->       T& { return _arr[idx]; }
    fn operator[](u32 idx) const noexcept -> const T& { return _arr[idx]; }
};

template<typename T>
struct vec<T, 4>
{
    static constexpr let $rank = 4;
    union {
        T _arr[$rank];
        struct { T x, y, z, w; };
    };

    vec() noexcept = default;

    template<u32 ...I, class V>
    vec(immut_t<u32, I...>, const V& v) noexcept
        : _arr{ T(v[I])... }
    {}

    vec(const T(&arr)[$rank]) noexcept : vec(seq_t<$rank>(), arr)
    {}

    template<class ...Us, class = when<1 + sizeof...(Us) == $rank> >
    vec(const T& t, const Us& ...s) noexcept
        : _arr{ t, s... }
    {}

    explicit operator bool() const noexcept {
        return ustd::idx_reduce<ops::And>(seq_t<$rank>{}, _arr);
    }

    fn operator[](u32 idx)       noexcept ->       T& { return _arr[idx]; }
    fn operator[](u32 idx) const noexcept -> const T& { return _arr[idx]; }
};

template<class T, class F, u32 N>
fn vec_cast(const vec<F,N>& src) noexcept -> vec<T,N> {
    return vec<T, N>(seq_t<N>(), src._arr);
}

#define def_vec_bop(op, fun)                                                        \
template<u32 ...I, class T, u32 N>                                                  \
fn _v##fun(immut_t<u32, I...>, const vec<T, N>& a, const vec<T, N>& b) noexcept {   \
    using R = decltype((a._arr[0] op b._arr[0]));                                   \
    return vec<R, N>((a._arr[I] op b._arr[I])...);                                  \
}                                                                                   \
template<typename T, u32 N>                                                         \
fn operator op(const vec<T,N>& a, const vec<T,N>& b) noexcept {                     \
    return _v##fun(seq_t<N>{}, a, b);                                               \
}

def_vec_bop(+ , add);
def_vec_bop(- , sub);
def_vec_bop(* , mul);
def_vec_bop(/ , div);
def_vec_bop(% , mod);

def_vec_bop(||, or);
def_vec_bop(&&, and);

def_vec_bop(> , gt);
def_vec_bop(< , lt);
def_vec_bop(>=, ge);
def_vec_bop(<=, le);
def_vec_bop(==, eq);
def_vec_bop(!=, ne);

#undef def_vec_bop

#pragma region scal
template<u32 ...I, class T, u32 N>
fn _vscal(immut_t<u32, I...>, const T& k, const vec<T, N>& x) noexcept -> vec<T, N>{
    return vec<T, N>((k * x._arr[I])...);
}

template<typename T, u32 N>
fn operator*(const T& k, const vec<T, N>& x) noexcept -> vec<T, N> {
    return _vscal(seq_t<N>{}, k, x);
}
#pragma endregion

#pragma region norm
template<u32 ...I, class T, u32 N>
fn _vnorm2(immut_t<u32, I...>, const vec<T, N>& x) noexcept -> vec<T, N>{
    return (... + (x._arr[I]*x._arr[I]));
}

template<typename T, u32 N>
fn vnorm(const vec<T,N>& x) noexcept -> T {
    return sqrt(_vnorm2(seq_t<N>{}, x));
}
#pragma endregion

#pragma region round
template<u32 ...I, u32 N>
fn _vround(immut_t<u32, I...>, const vec<f32, N>& x) noexcept -> vec<i32, N> {
    return vec<f32, N>(i32(__builtin_roundf(x._arr[I]))...);
}

template<u32 ...I, u32 N>
fn _vround(immut_t<u32, I...>, const vec<f64, N>& x) noexcept -> vec<i32, N> {
    return vec<f64, N>(i32(__builtin_round(x._arr[I]))...);
}

template<typename T, u32 N>
fn vround(const vec<T, N>& x) noexcept -> vec<i32, N> {
    return _vround(seq_t<N>{}, x);
}
#pragma endregion


#define def_vec(t)              \
    using t##x1 = vec<t, 1>;    \
    using t##x2 = vec<t, 2>;    \
    using t##x3 = vec<t, 3>;    \
    using t##x4 = vec<t, 4>

def_vec(i8);    def_vec(u8);
def_vec(i16);   def_vec(u16);
def_vec(i32);   def_vec(u32);
def_vec(i64);   def_vec(u64);
def_vec(f32);   def_vec(f64);
#undef def_vec

}

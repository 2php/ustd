#pragma once

#include "ustd/core.h"
#include "ustd/math/ndslice.h"

namespace ustd::math
{

template<typename T, u32 N>
struct Linspace
{
    constexpr static const u32 $rank = N;

    T _step[N];

    // property[r]: dims
    __declspec(property(get = get_dims)) u32 dims[];
    fn get_dims(u32) const noexcept -> u32 {
        return u32(0);
    }

    template<typename ...Tidx>
    fn operator()(Tidx ...idxs) const -> T {
        static_assert(sizeof...(Tidx) == $rank, "ustd::math::Linespace: unexpect arguments count, should be `$rank`");
        return this->at(seq_t<N>{}, idxs...);
    }

    template<u32 ...I, typename ...R>
    fn at(immut_t<u32, I...>, R ...idxs) const -> T {
        return ustd::sum((T(_step[I])*idxs)...);
    }
};

template<typename T, typename ...U>
constexpr fn vline(const T& t, const U& ...u) -> Linspace<T, u32(1 + sizeof...(U))> {
    return { { t, u... } };
}

template<typename F, typename ...T>
struct Parallel;

template<typename F, typename T>
struct Parallel<F, T>
{
    constexpr static const u32 $rank = T::$rank;

    T   _t;

    Parallel(const T& t) noexcept : _t(t) 
    {}

    // property[r]: dims
    __declspec(property(get = get_dims)) u32 dims[];
    fn get_dims(u32 i) const noexcept -> u32 {
        return _t.dims(i);
    }

    template<typename ...I>
    __forceinline fn operator()(I ...idx) const noexcept {
        return F::op(_t(idx...));
    }
};

template<typename F, typename A, typename B>
struct Parallel<F, A, B>
{
    constexpr static const u32 $rank = A::$rank | B::$rank;

    A  _a;
    B  _b;

    Parallel(const A& a, const B& b) noexcept : _a(a), _b(b) {
        static_assert(A::$rank == B::$rank || A::$rank == 0 || B::$rank == 0, "ustd::math::Parallel<A, B>: $rank not match");
    }

    __declspec(property(get = get_dims)) u32 dims[];
    fn get_dims(u32 i) const noexcept -> u32 {
        return A::$rank == 0 ? _a.dims[i] : _b.dims[i];
    }

    template<typename ...I>
    fn operator()(I ...idx) const noexcept {
        return F()(_a(idx...), _b(idx...));
    }
};

template<typename F, typename ...V>
struct Reduce;

template<typename F, typename V>
struct Reduce<F, V>
{
    constexpr static const u32 $rank = V::$rank - 1;

    V _t;

    __declspec(property(get = get_dims)) u32 dims[];
    fn get_dims(u32 i) const noexcept -> u32 {
        return _t.dims[i + 1];
    }

    template<typename ...I>
    fn operator()(I ...i) const noexcept {
        let cnt = _t.dims[0];
        let f   = F();

        mut ret = f(_t(0, i...), _t(1, i...));

        for (u32 k = 2u; k < cnt; ++k) {
            ret = f(ret, _t(k, i...));
        }
        return ret;
    }
};

#pragma region detail
template<typename T, u32 N>
constexpr fn _is_ndslice(const NDSlice<T, N>*) noexcept -> bool {
    return true;
}

constexpr fn _is_ndslice(const void*) noexcept -> bool {
    return false;
}

template<typename T>
constexpr fn _to_ndview(const T& val) noexcept {
    if constexpr(_is_ndslice(declptr<T>())) {
        return NDSlice<typename T::type_t, T::$rank>(val);
    }
    else if constexpr(trait<T>::$num) {
        return Scalar<T>(val);
    }
    else {
        return val;
    }
    
}

template<typename T, typename=decltype(T::$rank)>
constexpr fn _get_rank(const T*) noexcept -> u32 {
    return T::$rank;
}

constexpr fn _get_rank(...) noexcept -> u32 {
    return 0u;
}

template<typename A, typename B>
constexpr fn _get_rank_bop() -> u32 {
    return _get_rank(declptr<A>()) + _get_rank(declptr<B>());
}

template<typename F, typename A>
fn _make_parallel(const A& a) noexcept -> Parallel<F, decltype(_to_ndview(a))> {
    return { _to_ndview(a) };
}

template<typename F, typename A, typename B>
fn _make_parallel(const A& a, const B& b) noexcept -> Parallel<F, decltype(_to_ndview(a)), decltype(_to_ndview(b)) > {
    return { _to_ndview(a), _to_ndview(b) };
}

template<typename F, typename T>
fn _make_reduce(const T& t) noexcept -> Reduce<F, decltype(_to_ndview(t)) > {
    return { _to_ndview(t) };
}
#pragma endregion

template<class T, class = decltype(T::$rank) > fn operator+(const T& t) noexcept { return _make_parallel<ops::Pos>(t); }
template<class T, class = decltype(T::$rank) > fn operator-(const T& t) noexcept { return _make_parallel<ops::Neg>(t); }
                                                                 
template<class A, class B, class=when<_get_rank_bop<A,B>()!=0> > fn operator+(const A& a, const B& b) noexcept { return _make_parallel<ops::Add>(a, b); }
template<class A, class B, class=when<_get_rank_bop<A,B>()!=0> > fn operator-(const A& a, const B& b) noexcept { return _make_parallel<ops::Sub>(a, b); }
template<class A, class B, class=when<_get_rank_bop<A,B>()!=0> > fn operator*(const A& a, const B& b) noexcept { return _make_parallel<ops::Mul>(a, b); }
template<class A, class B, class=when<_get_rank_bop<A,B>()!=0> > fn operator/(const A& a, const B& b) noexcept { return _make_parallel<ops::Div>(a, b); }
template<class A, class B, class=when<_get_rank_bop<A,B>()!=0> > fn operator%(const A& a, const B& b) noexcept { return _make_parallel<ops::Mod>(a, b); }

template<class T> fn vabs  (const T& t) noexcept { return _make_parallel<ops::Abs>  (t); }
template<class T> fn vsqrt (const T& t) noexcept { return _make_parallel<ops::Sqrt> (t); }
template<class T> fn vpow2 (const T& t) noexcept { return _make_parallel<ops::Pow2> (t); }
template<class T> fn vexp  (const T& t) noexcept { return _make_parallel<ops::Exp>  (t); }
template<class T> fn vln   (const T& t) noexcept { return _make_parallel<ops::Ln>   (t); }
template<class T> fn vlog10(const T& t) noexcept { return _make_parallel<ops::Log10>(t); }

template<class T> fn vsin(const T& t) noexcept { return _make_parallel<ops::Sin>(t); }
template<class T> fn vcos(const T& t) noexcept { return _make_parallel<ops::Cos>(t); }
template<class T> fn vtan(const T& t) noexcept { return _make_parallel<ops::Tan>(t); }

template<class T> fn vasin(const T& t) noexcept { return _make_parallel<ops::ASin>(t); }
template<class T> fn vacos(const T& t) noexcept { return _make_parallel<ops::ACos>(t); }
template<class T> fn vatan(const T& t) noexcept { return _make_parallel<ops::ATan>(t); }

template<class T> fn vmax(const T& t) noexcept { return _make_reduce<ops::Max>(t); }
template<class T> fn vmin(const T& t) noexcept { return _make_reduce<ops::Min>(t); }
template<class T> fn vsum(const T& t) noexcept { return _make_reduce<ops::Add>(t); }

}

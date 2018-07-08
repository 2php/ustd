#pragma once

#include "ustd/core.h"
#include "ustd/math/types.h"
#include "ustd/math/funcs.h"

namespace ustd::math
{

template<typename F, class ...V>
struct parallel_t;

template<typename F, class ...V>
struct reduce_t;

/* parallel: f(a) */
template<typename F, class A>
struct parallel_t<F, A>: ndvec_t<res_t<F, A>, A::$rank>
{
    using vec_t = parallel_t;

    A   _a;

    explicit parallel_t(const A& a) noexcept : _a(a) 
    {}

    // property[r]: dims
    fn dims(u32 i) const noexcept -> u32 {
        return _a.dims(i);
    }

    template<typename ...I>
    fn operator()(I ...idx) const noexcept {
        let a = _a(idx...);
        let r = F()(a);
        return r;
    }
};

/* parallel: f(a, b) */
template<typename F, class A, class B>
struct parallel_t<F, A, B>: ndvec_t<res_t<F, A, B>, A::$rank|B::$rank >
{
    using vec_t = parallel_t;

    A  _a;
    B  _b;

    explicit parallel_t(const A& a, const B& b) noexcept : _a(a), _b(b) {
        static_assert(A::$rank == B::$rank || A::$rank == 0 || B::$rank == 0, "ustd::math::Parallel<A, B>: $rank not match");
    }

    fn dims(u32 i) const noexcept -> u32 {
        return A::$rank == 0 ? _a.dims(i) : _b.dims(i);
    }

    template<typename ...I>
    fn operator()(I ...idx) const noexcept {
        let a = _a(idx...);
        let b = _b(idx...);
        let r = F()(a, b);
        return r;
    }
};

template<typename F, typename ...T>
fn _make_parallel(const T& ...t) -> parallel_t<F, T...> {
    return parallel_t<F, T...>(t...);
}

template<typename F, typename ...T>
fn make_parallel(const T& ...t) noexcept {
    return _make_parallel<F>(to_ndvec(t)...);
}

/* reduce: f(a) */
template<typename F, typename A>
struct reduce_t<F, A>: ndvec_t<res_t<F, A, A>, A::$rank-1>
{
    using vec_t = reduce_t;
    A  _a;

    template<typename TA>
    reduce_t(TA&& a) noexcept : _a(a)
    {}

    fn dims(u32 i) const noexcept -> u32 {
        return _a.dims(i + 1);
    }

    template<typename ...I>
    fn operator()(I ...i) const noexcept {
        let nx  = _a.dims(0);
        let a0  = _a(0, i...);
        let a1  = _a(1, i...);
        mut r   = F()(a0, a1);

        for (mut k = 2u; k < nx; ++k) {
            let a = _a(k, i...);
            r     = F()(r, a);
        }
        return r;
    }
};

template<typename F, typename ...T>
fn _make_reduce(const T& ...t) -> reduce_t<F, T...> {
    return reduce_t<F, T...>(t...);
}

template<typename F, typename ...T>
fn make_reduce(const T& ...t) noexcept {
    return _make_reduce<F>(to_ndvec(t)...);
}

/* parallel: @a */
template<class A, class =  when<$is_ndvec<A>>> fn operator+(const A& a) noexcept { return make_parallel<ops::Pos>(a); }
template<class A, class =  when<$is_ndvec<A>>> fn operator-(const A& a) noexcept { return make_parallel<ops::Neg>(a); }

/* parallel: a @ b */                                                                 
template<class A, class B, class = when<$is_ndvec<A>||$is_ndvec<B>>> fn operator+(const A& a, const B& b) noexcept { return make_parallel<ops::Add>(a, b); }
template<class A, class B, class = when<$is_ndvec<A>||$is_ndvec<B>>> fn operator-(const A& a, const B& b) noexcept { return make_parallel<ops::Sub>(a, b); }
template<class A, class B, class = when<$is_ndvec<A>||$is_ndvec<B>>> fn operator*(const A& a, const B& b) noexcept { return make_parallel<ops::Mul>(a, b); }
template<class A, class B, class = when<$is_ndvec<A>||$is_ndvec<B>>> fn operator/(const A& a, const B& b) noexcept { return make_parallel<ops::Div>(a, b); }
template<class A, class B, class = when<$is_ndvec<A>||$is_ndvec<B>>> fn operator%(const A& a, const B& b) noexcept { return make_parallel<ops::Mod>(a, b); }

/* parallel: fun(a) */
template<class A> fn vabs  (const A& a) noexcept { return make_parallel<ops::Abs>  (a); }
template<class A> fn vsqrt (const A& a) noexcept { return make_parallel<ops::Sqrt> (a); }
template<class A> fn vpow2 (const A& a) noexcept { return make_parallel<ops::Pow2> (a); }
template<class A> fn vexp  (const A& a) noexcept { return make_parallel<ops::Exp>  (a); }
template<class A> fn vln   (const A& a) noexcept { return make_parallel<ops::Ln>   (a); }
template<class A> fn vlog10(const A& a) noexcept { return make_parallel<ops::Log10>(a); }
template<class A> fn vsin  (const A& a) noexcept { return make_parallel<ops::Sin>  (a); }
template<class A> fn vcos  (const A& a) noexcept { return make_parallel<ops::Cos>  (a); }
template<class A> fn vtan  (const A& a) noexcept { return make_parallel<ops::Tan>  (a); }
template<class A> fn vasin (const A& a) noexcept { return make_parallel<ops::ASin> (a); }
template<class A> fn vacos (const A& a) noexcept { return make_parallel<ops::ACos> (a); }
template<class A> fn vatan (const A& a) noexcept { return make_parallel<ops::ATan> (a); }

/* reduce: fun(a) */
template<class A> fn vmax  (const A& a) noexcept { return make_reduce<ops::Max>  (a); }
template<class A> fn vmin  (const A& a) noexcept { return make_reduce<ops::Min>  (a); }
template<class A> fn vsum  (const A& a) noexcept { return make_reduce<ops::Add>  (a); }

}

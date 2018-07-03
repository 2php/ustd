#pragma once

#include "ustd/core/builtin.h"

namespace ustd::ops
{

struct Pos
{
    template<class T>
    constexpr fn operator()(const T& t) const noexcept {
        return +t;
    }
};

struct Neg
{
    template<class T>
    constexpr fn operator()(const T& t) const noexcept {
        return -t;
    }
};

struct And
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... && u);
    }

    template<class F, class ...Us>
    constexpr fn map(F&& f, const Us& ...u) const noexcept {
        return (... && f(u));
    }
};

struct Or
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... || u);
    }

    template<class F, class ...Us>
    constexpr fn map(F&& f, const Us& ...u) const noexcept {
        return (... && f(u));
    }
};

struct Add
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... + u);
    }
};

struct Sub
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... - u);
    }
};

struct Mul
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... * u);
    }
};

struct Div
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... / u);
    }
};

struct Mod
{
    template<class ...Us>
    constexpr fn operator()(const Us& ...u) const noexcept {
        return (... / u);
    }
};


struct Eq
{
    template<class A, class B>
    constexpr fn operator()(A a, B b) const noexcept -> bool {
        return a == b;
    }
};

struct Ne
{
    template<class A, class B>
    constexpr fn operator()(A a, B b) const noexcept -> bool {
        return a != b;
    }
};

struct Lt
{
    template<class A, class B>
    constexpr fn operator()(A a, B b) const noexcept -> bool {
        return a < b;
    }
};

struct Gt
{
    template<class A, class B>
    constexpr fn operator()(A a, B b) const noexcept -> bool {
        return a > b;
    }
};

struct Le
{
    template<class A, class B>
    constexpr fn operator()(A a, B b) const noexcept -> bool {
        return a <= b;
    }
};

struct Ge
{
    template<class A, class B>
    constexpr fn operator()(A a, B b) const noexcept -> bool {
        return a >= b;
    }
};

struct Pow2 {
    template<class A>
    constexpr fn operator()(A a) const noexcept {
        return a * a;
    }
};

struct Pow3 {
    template<class A>
    constexpr fn operator()(A a) const noexcept {
        return a * a * a;
    }
};

struct Abs  {
    template<class A>
    constexpr fn operator()(A a) const noexcept {
        return a < 0 ? -a : +a;
    }
};

struct SetTo { template<class A, class B> fn operator()(A& a, B b) const noexcept -> A& { return a  = b; } };
struct AddTo { template<class A, class B> fn operator()(A& a, B b) const noexcept -> A& { return a += b; } };
struct SubTo { template<class A, class B> fn operator()(A& a, B b) const noexcept -> A& { return a -= b; } };
struct MulTo { template<class A, class B> fn operator()(A& a, B b) const noexcept -> A& { return a *= b; } };
struct DivTo { template<class A, class B> fn operator()(A& a, B b) const noexcept -> A& { return a /= b; } };
struct ModTo { template<class A, class B> fn operator()(A& a, B b) const noexcept -> A& { return a %= b; } };


struct Max
{
    template<typename T, typename ...Us>
    constexpr fn operator()(const T& t, const Us& ...u) const noexcept {
        if constexpr(sizeof...(Us) == 0) { return t; }
        if constexpr(sizeof...(Us) == 1) { return t > as_const(u...) ? t : as_const(u...); }
        if constexpr(sizeof...(Us) >= 2) { return op(t, op(u...)); }
    }
};

struct Min
{
    template<typename T, typename ...Us>
    constexpr fn operator()(const T& t, const Us& ...u) const noexcept {
        if constexpr(sizeof...(Us) == 0) { return t; }
        if constexpr(sizeof...(Us) == 1) { return t < as_const(u...) ? t : as_const(u...); }
        if constexpr(sizeof...(Us) >= 2) { return op(t, op(u...)); }
    }
};


}

namespace ustd
{

template<class F, u32 ...I, typename V>
constexpr fn idx_reduce(immut_t<u32, I...>, const V& v) noexcept {
    return F()(v[I]...);
}

template<class F, u32 ...I, typename V, typename M>
constexpr fn idx_reduce(immut_t<u32, I...>, const V& v, M&& m) noexcept {
    return F().map(m, v[I]...);
}

template<class T>
constexpr fn(max)(T a, T b) noexcept -> T {
    return a > b ? a : b;
}

template<class T>
constexpr fn(min)(T a, T b) noexcept -> T {
    return a < b ? a : b;
}

template<class T, class=when<trait<T>::$num> >
constexpr fn(abs)(T x) noexcept -> T {
    return x < T(0) ? -x : x;
}

template<typename ...Us>
constexpr fn(all)(const Us& ...us) noexcept -> bool {
    return ops::And()(us...);
}

template<typename ...Us>
constexpr fn(any)(const Us& ...us) noexcept -> bool {
    return ops::Or()(us...);
}

template<typename ...Us>
constexpr fn(min)(const Us& ...us) noexcept {
    return ops::Min()(us...);
}

template<typename ...Us>
constexpr fn(max)(const Us& ...us) noexcept {
    return ops::Max()(us...);
}

template<typename ...Us>
constexpr fn sum(const Us& ...us) noexcept {
    return ops::Add()(us...);
}

template<typename ...Us>
constexpr fn prod(const Us& ...us) noexcept {
    return ops::Mul()(us...);
}

}

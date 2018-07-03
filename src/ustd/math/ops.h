#pragma once

#include "ustd/core.h"

namespace ustd::math
{

inline f32 exp(f32 x)   noexcept  { return ustd_builtin(expf)(x); }
inline f64 exp(f64 x)   noexcept  { return ustd_builtin(exp)(x);  }

inline f32 ln(f32 x)    noexcept  { return ustd_builtin(logf)(x); }
inline f64 ln(f64 x)    noexcept  { return ustd_builtin(log)(x);  }

inline f32 log10(f32 x) noexcept  { return ustd_builtin(log10f)(x); }
inline f64 log10(f64 x) noexcept  { return ustd_builtin(log10)(x);  }

inline f32 sin(f32 x)   noexcept  { return ustd_builtin(sinf)(x); }
inline f64 sin(f64 x)   noexcept  { return ustd_builtin(sin)(x);  }

inline f32 cos(f32 x)   noexcept  { return ustd_builtin(cosf)(x); }
inline f64 cos(f64 x)   noexcept  { return ustd_builtin(cos)(x);  }

inline f32 tan(f32 x)   noexcept  { return ustd_builtin(tanf)(x); }
inline f64 tan(f64 x)   noexcept  { return ustd_builtin(tan)(x);  }

inline f32 asin(f32 x)  noexcept  { return ustd_builtin(asinf)(x); }
inline f64 asin(f64 x)  noexcept  { return ustd_builtin(asin)(x);  }

inline f32 acos(f32 x)  noexcept  { return ustd_builtin(acosf)(x); }
inline f64 acos(f64 x)  noexcept  { return ustd_builtin(acos)(x);  }

inline f32 atan(f32 x)  noexcept  { return ustd_builtin(atanf)(x); }
inline f64 atan(f64 x)  noexcept  { return ustd_builtin(atan)(x);  }

inline f32 sqrt(f32 x)  noexcept { return ustd_builtin(sqrtf)(x); }
inline f64 sqrt(f64 x)  noexcept { return ustd_builtin(sqrt)(x);  }

inline f32 erf(f32 x) noexcept { return ustd_builtin(erff)(x); }
inline f64 erf(f64 x) noexcept { return ustd_builtin(erf )(x); }

inline f32 pow(f32 x, f32 y) noexcept { return ustd_builtin(powf)(x, y); }
inline f64 pow(f64 x, f64 y) noexcept { return ustd_builtin(pow)(x, y);  }

inline f32 atan2(f32 x, f32 y) noexcept { return ustd_builtin(atan2f)(x, y); }
inline f64 atan2(f64 x, f64 y) noexcept { return ustd_builtin(atan2)(x, y);  }

namespace ops
{
using namespace ustd::ops;

struct Sin   { template<class A> fn operator()(A a) noexcept { return math::sin(a); } };
struct Cos   { template<class A> fn operator()(A a) noexcept { return math::cos(a); } };
struct Tan   { template<class A> fn operator()(A a) noexcept { return math::tan(a); } };

struct ASin  { template<class A> fn operator()(A a) noexcept { return math::asin(a); } };
struct ACos  { template<class A> fn operator()(A a) noexcept { return math::acos(a); } };
struct ATan  { template<class A> fn operator()(A a) noexcept { return math::atan(a); } };

struct Ln    { template<class A> fn operator()(A a) noexcept { return math::ln(a);    } };
struct Log10 { template<class A> fn operator()(A a) noexcept { return math::log10(a); } };
struct Exp   { template<class A> fn operator()(A a) noexcept { return math::exp(a);   } };
struct Sqrt  { template<class A> fn operator()(A a) noexcept { return math::sqrt(a);  } };

struct Pow   { template<class A, class B> fn operator()(A a, B b) noexcept { return math::pow(a, b); } };

}

template<u32 ...I, class T, u32 N>
fn idx_norm(immut_t<u32, I...>, vec<T, N> v) noexcept -> T {
    return math::sqrt(... +(v[I]*v[I]));
}

template<class T, u32 N>
fn vnorm(vec<T, N> v) noexcept -> T {
    return idx_norm(seq_t<N>{}, v);
}

}

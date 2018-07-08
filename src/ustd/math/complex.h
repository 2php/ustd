#pragma once

#include "ustd/core/builtin.h"

namespace ustd::math
{

#ifndef USTD_MSVC_INTELLISENSE
constexpr _Complex float  _cf32 = 0;
constexpr _Complex double _cf64 = 0;
using cf32 = decltype(_cf32);
using cf64 = decltype(_cf64);
#endif

#ifdef USTD_MSVC_INTELLISENSE
using cf32 = _Fcomplex;
using cf64 = _Dcomplex;
#endif

inline cf32 exp(cf32 x)   noexcept { return ustd_builtin(cexpf)(x); }
inline cf64 exp(cf64 x)   noexcept { return ustd_builtin(cexp)(x); }

inline cf32 ln(cf32 x)    noexcept { return ustd_builtin(clogf)(x); }
inline cf64 ln(cf64 x)    noexcept { return ustd_builtin(clog)(x); }

inline cf32 sin(cf32 x)   noexcept { return ustd_builtin(csinf)(x); }
inline cf64 sin(cf64 x)   noexcept { return ustd_builtin(csin)(x); }

inline cf32 cos(cf32 x)   noexcept { return ustd_builtin(ccosf)(x); }
inline cf64 cos(cf64 x)   noexcept { return ustd_builtin(ccos)(x); }

inline cf32 tan(cf32 x)   noexcept { return ustd_builtin(ctanf)(x); }
inline cf64 tan(cf64 x)   noexcept { return ustd_builtin(ctan)(x); }

inline cf32 asin(cf32 x)  noexcept { return ustd_builtin(casinf)(x); }
inline cf64 asin(cf64 x)  noexcept { return ustd_builtin(casin)(x); }

inline cf32 acos(cf32 x)  noexcept { return ustd_builtin(cacosf)(x); }
inline cf64 acos(cf64 x)  noexcept { return ustd_builtin(cacos)(x); }

inline cf32 atan(cf32 x)  noexcept { return ustd_builtin(catanf)(x); }
inline cf64 atan(cf64 x)  noexcept { return ustd_builtin(catan)(x); }

inline cf32 sqrt(cf32 x)  noexcept { return ustd_builtin(csqrtf)(x); }
inline cf64 sqrt(cf64 x)  noexcept { return ustd_builtin(csqrt)(x); }

inline cf32 pow(cf32 x, cf32 y) noexcept { return ustd_builtin(cpowf)(x, y); }
inline cf64 pow(cf64 x, cf64 y) noexcept { return ustd_builtin(cpow)(x, y); }

}

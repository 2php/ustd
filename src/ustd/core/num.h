#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

enum FP
{
    $nan,
    $inf,
    $normal,
    $subnormal,
    $zero,
};


inline fn (fpclassify)(f32 x) noexcept -> FP   { return FP(ustd_builtin(fpclassify)(FP::$nan, FP::$inf, FP::$normal, FP::$subnormal, FP::$zero, x)); }
inline fn (fpclassify)(f64 x) noexcept -> FP   { return FP(ustd_builtin(fpclassify)(FP::$nan, FP::$inf, FP::$normal, FP::$subnormal, FP::$zero, x)); }
inline fn (isnan)(f32 x)      noexcept -> bool { return ustd_builtin(isnan)(x); }
inline fn (isnan)(f64 x)      noexcept -> bool { return ustd_builtin(isnan)(x); }
inline fn (isinf)(f32 x)      noexcept -> bool { return ustd_builtin(isinf)(x); }
inline fn (isinf)(f64 x)      noexcept -> bool { return ustd_builtin(isinf)(x); }
inline fn (isnormal)(f32 x)   noexcept -> bool { return ustd_builtin(isnormal)(x); }
inline fn (isnormal)(f64 x)   noexcept -> bool { return ustd_builtin(isnormal)(x); }

pub fn float_eq(f32 a, f32 b, f32 err = 1e-6f) noexcept -> bool;
pub fn float_eq(f64 a, f64 b, f64 err = 1e-14) noexcept -> bool;

}

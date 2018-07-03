#include "config.inl"

namespace ustd
{

pub fn fpclassify(f32 x) noexcept -> FP { return FP(__builtin_fpclassify(FP::$nan, FP::$inf, FP::$normal, FP::$subnormal, FP::$zero, x)); }
pub fn fpclassify(f64 x) noexcept -> FP { return FP(__builtin_fpclassify(FP::$nan, FP::$inf, FP::$normal, FP::$subnormal, FP::$zero, x)); }

pub fn isnan(f32 x)     noexcept -> bool { return __builtin_isnan(x); }
pub fn isnan(f64 x)     noexcept -> bool { return __builtin_isnan(x); }
pub fn isinf(f32 x)     noexcept -> bool { return __builtin_isinf(x); }
pub fn isinf(f64 x)     noexcept -> bool { return __builtin_isinf(x); }
pub fn isnormal(f32 x)  noexcept -> bool { return __builtin_isnormal(x); }
pub fn isnormal(f64 x)  noexcept -> bool { return __builtin_isnormal(x); }

pub fn float_eq(f32 a, f32 b, f32 err) noexcept -> bool {
    let base = 0.5f*(ustd::abs(a) + ustd::abs(b));
    if (!isnormal(base)) return false;

    let abs_err = ustd::abs(a - b);
    let rel_err = abs_err / base;
    let res_val = rel_err < err;
    return res_val;
}

pub fn float_eq(f64 a, f64 b, f64 err) noexcept -> bool {
    let base = 0.5*(ustd::abs(a) + ustd::abs(b));
    if (!isnormal(base)) return false;

    let abs_err = ustd::abs(a - b);
    let rel_err = abs_err / base;
    let res_val = rel_err < err;
    return res_val;
}

}

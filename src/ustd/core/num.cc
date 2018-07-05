#include "config.inl"

namespace ustd
{



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

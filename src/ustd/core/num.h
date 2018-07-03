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

pub fn fpclassify(f32 x) noexcept -> FP  ;
pub fn fpclassify(f64 x) noexcept -> FP  ;

pub fn isnan     (f32 x) noexcept -> bool;
pub fn isnan     (f64 x) noexcept -> bool;
pub fn isinf     (f32 x) noexcept -> bool;
pub fn isinf     (f64 x) noexcept -> bool;
pub fn isnormal  (f32 x) noexcept -> bool;
pub fn isnormal  (f64 x) noexcept -> bool;

pub fn float_eq(f32 a, f32 b, f32 err = 1e-6f) noexcept -> bool;
pub fn float_eq(f64 a, f64 b, f64 err = 1e-14) noexcept -> bool;

}

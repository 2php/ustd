#pragma once

#include "ustd/core.h"
#include "ustd/math/ndslice.h"

namespace ustd::math
{

using vf32 = NDSlice<f32, 1>;
using vf64 = NDSlice<f64, 1>;
using mf32 = NDSlice<f32, 2>;
using mf64 = NDSlice<f64, 2>;
using vu32 = NDSlice<u32, 1>;

inline namespace blas
{
// blas: 1
pub fn amax(vf32 x) noexcept->u32;
pub fn amax(vf64 x) noexcept->u32;

pub fn amin(vf32 x) noexcept->u32;
pub fn amin(vf64 x) noexcept->u32;

pub fn asum(vf32 x) noexcept->f32;
pub fn asum(vf64 x) noexcept->f64;

pub fn axpy(f32 a, vf32 x, vf32 y) -> void;
pub fn axpy(f64 a, vf64 x, vf64 y) -> void;

pub fn copy(vf32 x, vf32 y) -> void;
pub fn copy(vf64 x, vf64 y) -> void;

pub fn dot(vf32 x, vf32 y) noexcept->f32;
pub fn dot(vf64 x, vf64 y) noexcept->f64;

pub fn nrm2(vf32 x) noexcept->f32;
pub fn nrm2(vf64 x) noexcept->f64;

pub fn rot(vf32 x, vf32 y, f32x4 h) -> void;
pub fn rot(vf64 x, vf64 y, f64x4 h) -> void;

pub fn scal(f32 alpha, vf32 x) -> void;
pub fn scal(f64 alpha, vf64 x) -> void;

pub fn swap(vf32 x, vf32 y) -> void;
pub fn swap(vf64 x, vf64 y) -> void;

// y := alpha*A*x + beta*y
pub fn gemv(f32 alpha, mf32 ma, vf32 vx, f32 beta, vf32 vy) -> void;

// y := alpha*A*x + beta*y
pub fn gemv(f64 alpha, mf64 ma, vf64 vx, f64 beta, vf64 vy) -> void;

// A := alpha*x*y' + A
pub fn ger(f32 alpha, vf32 x, vf32 y, mf32 A) -> void;

// A := alpha*x*y' + A
pub fn ger(f64 alpha, vf64 vx, vf64 vy, mf64 A) -> void;

// C := alpha*A*B + beta*C
pub fn gemm(f32 alpha, mf32 A, mf32 B, f32 beta, mf32 C) -> void;

// C := alpha*A*B + beta*C
pub fn gemm(f64 alpha, mf64 A, mf64 B, f64 beta, mf64 C) -> void;

}

inline namespace lapack
{

// A = P*L*U
pub fn getrf(mf32 A, vu32 vp) -> void;

// A = P*L*U
pub fn getrf(mf64 A, vu32 vp) -> void;

// A = QR
pub fn geqrf(mf32 A, vf32 tau) -> void;

// A = QR
pub fn geqrf(mf64 A, vf64 tau) -> void;

// A = SVD
pub fn gesvd(mf32 A, vf32 s, mf32 U, mf32 VT) -> void;

// A -> A'
pub fn getri(mf32 A, vu32 vp) -> void;

// A -> A'
pub fn getri(mf64 A, vu32 vp) -> void;

// A*x = b
pub fn getrs(mf32 A, vu32 vp, vf32 b) -> void;

// A*X = B
pub fn getrs(mf64 A, vu32 vp, vf64 b) -> void;

}

inline namespace fft
{}

}

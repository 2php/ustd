#include "config.inl"

namespace ustd::math
{

inline namespace mkl
{

enum class Layout: u32
{
    None        = 0,
    Row         = 101,
    Col         = 102
};

enum class Trans: u32
{
    None        = 0,
    NoTrans     = 111,
    Trans       = 112,
    ConjTrans   = 113
};

template<class T>
static fn get_trans(NDSlice<T, 2> A) -> Trans {
    if (A._step[0] == 1) return Trans::NoTrans;
    if (A._step[1] == 1) return Trans::Trans;
    log::error("ustd::math::mkl::get_trans(): invalid valud");

    return Trans::None;
}

static fn _get_mkl() -> Option<ffi::Mod> {
    mut res = ffi::Mod::load("mkl_rt");
    if (res.is_none()) {
        log::error("ustd::math::mkl::get_mkl(): cannot load library.");
    }
    return as_mov(res);
}

static fn get_mkl() -> const Option<ffi::Mod>& {
    static let mkl_opt = _get_mkl();
    return mkl_opt;
}

template<class F>
static fn get_fun(str name) -> Option<ffi::Fn<F>> {
    let& mod_opt = get_mkl();
    if (mod_opt.is_none()) {
        log::error("ustd::math::mkl::get_fun({name={}}): cannot load mod `mkl`", name);
        return Option<ffi::Fn<F>>::None();
    }

    let  fid_opt = mod_opt._val[name];
    if (fid_opt.is_none()) {
        log::error("ustd::math::mkl::get_fun({name={}}): cannot find func", name);
        return Option<ffi::Fn<F>>::None();
    }

    let fun = ffi::Fn<F>(fid_opt._val);
    return Option<ffi::Fn<F>>::Some(fun);
}

}

// blas: 1
inline namespace blas
{
pub fn amax(vf32 x) noexcept-> u32 {
    static let f = get_fun<u32(u32 cnt, f32* vx, u32 dx)>("cblas_isamax");
    if(f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

pub fn amax(vf64 x) noexcept-> u32 {
    static let f = get_fun<u32(u32 cnt, f64* vx, u32 dx)>("cblas_idamax");
    if(f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

unittest(amax)
{
    mut x = NDArray<f32, 1>::with_dims({ 101 });

    x <<= vline(1.0f);
    let i0 = amax(x);
    assert_eq(i0, 100u);

    x -= 100;
    let i1 = amax(x);
    assert_eq(i1, 0u);
}

pub fn amin(vf32 x) noexcept -> u32 {
    static let f = get_fun<u32(u32 cnt, f32* vx, u32 dx)>("cblas_isamin");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

pub fn amin(vf64 x) noexcept -> u32 {
    static let f = get_fun<u32(u32 cnt, f64* vx, u32 dx)>("cblas_idamin");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

unittest(samin)
{
    mut x = NDArray<f32, 1>::with_dims({ 101 });

    x <<= vline(1.0f);
    let i0 = amin(x);
    assert_eq(i0, 0u);

    x -= 100;
    let i1 = amin(x);
    assert_eq(i1, 100u);

    x += 50;
    let i2 = amin(x);
    assert_eq(i2, 50u);
}

pub fn asum(vf32 x) noexcept->f32 {
    static let f = get_fun<f32(u32 cnt, f32* vx, u32 dx)>("cblas_sasum");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

pub fn asum(vf64 x) noexcept->f64 {
    static let f = get_fun<f64(u32 cnt, f64* vx, u32 dx)>("cblas_dasum");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

pub fn axpy(f32 alpha, vf32 x, vf32 y) -> void {
    static let f = get_fun<void(u32 cnt, f32 alpha, f32* vx, u32 dx, f32* vy, u32 dy)>("cblas_saxpy");
    if (f.is_none()) {
        return;
    }

    let cnt = y.count();
    f._val(cnt, alpha, x._data, x._step[0], y._data, y._step[0]);
}

pub fn axpy(f64 alpha, vf64 x, vf64 y) -> void {
    static let f = get_fun<void(u32 cnt, f64 alpha, f64* vx, u32 dx, f64* vy, u32 dy)>("cblas_daxpy");
    if (f.is_none()) {
        return;
    }

    let cnt = x.count();
    f._val(cnt, alpha, x._data, x._step[0], y._data, y._step[0]);
}

pub fn copy(vf32 x, vf32 y) -> void {
    static let f = get_fun<void(u32 cnt, f32* vx, u32 dx, f32* vy, u32 dy)>("cblas_scopy");
    if (f.is_none()) {
        return;
    }

    let cnt = x.count();
    f._val(cnt, x._data, x._step[0], y._data, y._step[0]);
}

pub fn copy(vf64 x, vf64 y) -> void {
    static let f = get_fun<void(u32 cnt, f64* vx, u32 dx, f64* vy, u32 dy)>("cblas_dcopy");
    if (f.is_none()) {
        return;
    }

    let cnt = x.count();
    f._val(cnt, x._data, x._step[0], y._data, y._step[0]);
}

pub fn dot(vf32 x, vf32 y) noexcept->f32 {
    static let f = get_fun<f32(u32 cnt, f32* vx, u32 dx, f32* vy, u32 dy)>("cblas_sdot");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0], y._data, y._step[0]);;
    return res;
}

pub fn dot(vf64 x, vf64 y) noexcept->f64 {
    static let f = get_fun<f64(u32 cnt, f64* vx, u32 dx, f64* vy, u32 dy)>("cblas_ddot");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0], y._data, y._step[0]);;
    return res;
}

pub fn nrm2(vf32 x) noexcept->f32 {
    static let f = get_fun<f32(u32 cnt, f32* vx, u32 dx)>("cblas_snrm2");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

pub fn nrm2(vf64 x) noexcept->f64 {
    static let f = get_fun<f64(u32 cnt, f64* vx, u32 dx)>("cblas_dnrm2");
    if (f.is_none()) {
        return 0;
    }

    let cnt = x.count();
    let res = f._val(cnt, x._data, x._step[0]);
    return res;
}

pub fn rot(vf32 x, vf32 y, f32x4 h) -> void {
    static let f1 = get_fun<void(u32 cnt, f32* vx, u32 dx, f32* vy, u32 dy, f32 c, f32 s)>("cblas_srot");
    static let f2 = get_fun<void(u32 cnt, f32* vx, u32 dx, f32* vy, u32 dy, f32* m)>("cblas_srotm");
    if (f1.is_none() || f2.is_none()) {
        return;
    }

    let cnt = y.count();
    if (bool(h[0]==h[3]) && bool(h[1]==-h[2])) {
        // +c +s
        // -s +c
        let c = +h[0];
        let s = -h[1];
        f1._val(cnt, x._data, x._step[0], y._data, y._step[0], c, s);
    }
    else {
        f2._val(cnt, x._data, x._step[0], y._data, y._step[0], h._arr);
    }
}

pub fn rot(vf64 x, vf64 y, f64x4 h) -> void {
    static let f1 = get_fun<void(u32 cnt, f64* vx, u32 dx, f64* vy, u32 dy, f64 c, f64 s)>("cblas_srot");
    static let f2 = get_fun<void(u32 cnt, f64* vx, u32 dx, f64* vy, u32 dy, f64* m)>("cblas_srotm");
    if (f1.is_none() || f2.is_none()) {
        return;
    }

    let cnt = y.count();
    if (bool(h[0]==h[3]) && bool(h[1]==-h[2])) {
        // +c +s
        // -s +c
        let c = +h[0];
        let s = -h[1];
        f1._val(cnt, x._data, x._step[0], y._data, y._step[0], c, s);
    }
    else {
        f2._val(cnt, x._data, x._step[0], y._data, y._step[0], h._arr);
    }
}

unittest(rot)
{
    mut x = NDArray<f32, 1>::with_dims({ 64 });
    mut y = NDArray<f32, 1>::with_dims({ 64 });
    x <<= vline(1.0f);
    y <<= vline(0.1f);

    assert_eq(x(1), 1.0f);
    assert_eq(y(1), 0.1f);

    rot(x, y, { 0.0f, -1.0f, +1.0f, 0.0f });

    assert_eq(x(1), +0.1f);
    assert_eq(y(1), -1.0f);
}

pub fn scal(f32 alpha, vf32 x) -> void {
    static let f = get_fun<void(u32 cnt, f32 alpha, f32* vx, u32 dx)>("cblas_sscal");
    if (f.is_none()) {
        return;
    }

    let cnt = x.count();
    f._val(cnt, alpha, x._data, x._step[0]);
}

pub fn scal(f64 alpha, vf64 x) -> void {
    static let f = get_fun<void(u32 cnt, f64 alpha, f64* vx, u32 dx)>("cblas_dscal");
    if (f.is_none()) {
        return;
    }
    
    let cnt = x.count();
    f._val(cnt, alpha, x._data, x._step[0]);
}

pub fn swap(vf32 x, vf32 y) -> void {
    static let f = get_fun<void(u32 cnt, f32* vx, u32 dx, f32* vy, u32 dy)>("cblas_sswap");
    if (f.is_none()) {
        return;
    }

    let cnt = y.count();
    f._val(cnt, x._data, x._step[0], y._data, y._step[0]);
}

pub fn swap(vf64 x, vf64 y) -> void {
    static let f = get_fun<void(u32 cnt, f64* vx, u32 dx, f64* vy, u32 dy)>("cblas_dswap");
    let cnt = y.count();
    f._val(cnt, x._data, x._step[0], y._data, y._step[0]);
}

// y := alpha*A*x + beta*y
pub fn gemv(f32 alpha, mf32 A, vf32 x, f32 beta, vf32 y) -> void {
    static let f = get_fun<void(Layout layout, Trans trans, u32 m, u32 n, f32 alpha, f32* ma, u32 lda, f32* vx, u32 incx, f32 beta, f32* vy, u32 incy)>("cblas_sgemv");
    let trans = get_trans(A);
    f._val(Layout::Col, trans, A._dims[0], A._dims[1], alpha, A._data, A._step[1], x._data, x._step[0], beta, y._data, y._step[1]);
}

// y := alpha*A*x + beta*y
pub fn gemv(f64 alpha, mf64 ma, vf64 vx, f64 beta, vf64 vy) -> void {
    static let f = get_fun<void(Layout layout, Trans trans, u32 m, u32 n, f64 alpha, f64* ma, u32 lda, f64* vx, u32 incx, f64 beta, f64* vy, u32 incy)>("cblas_dgemv");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, get_trans(ma), ma._dims[0], ma._dims[1], alpha, ma._data, ma._step[1], vx._data, vx._step[0], beta, vy._data, vy._step[1]);
}

// A := alpha*x*y' + A
pub fn ger(f32 alpha, vf32 x, vf32 y, mf32 A) -> void {
    static let f = get_fun<void(Layout layout, Trans trans, u32 m, u32 n, f32 alpha, f32* vx, u32 incx, f32* vy, u32 incy, f32* ma, u32 lda)>("cblas_sger");
    let trans = get_trans(A);
    f._val(Layout::Col, trans, A._dims[0], A._dims[1], alpha, x._data, x._step[0], y._data, y._step[0], A._data, A._step[1]);
}

// A := alpha*x*y' + A
pub fn ger(f64 alpha, vf64 vx, vf64 vy, mf64 A) -> void {
    static let f = get_fun<void(Layout layout, Trans trans, u32 m, u32 n, f64 alpha, f64* vx, u32 incx, f64* vy, u32 incy, f64* ma, u32 lda)>("cblas_dger");
    let trans = get_trans(A);
    f._val(Layout::Col, trans, A._dims[0], A._dims[1], alpha, vx._data, vx._step[0], vy._data, vy._step[0], A._data, A._step[1]);
}

// C := alpha*A*B + beta*C
pub fn gemm(f32 alpha, mf32 A, mf32 B, f32 beta, mf32 C) -> void {
    static let f = get_fun<void(Layout layout, Trans transa, Trans transb, u32 m, u32 n, u32 k, f32 alpha, f32* a, u32 lda, f32* b, u32 ldb, f32 beta, f32* c, u32 ldc)>("cblas_sgemm");
    if (f.is_none()) {
        return;
    }
    
    let trans_a = get_trans(A);
    let trans_b = get_trans(B);
    f._val(Layout::Col, trans_a, trans_b, C._dims[0], C._dims[1], A._dims[1], alpha, A._data, A._step[1], B._data, B._step[1], beta, C._data, C._step[1]);
}

// C := alpha*A*B + beta*C
pub fn gemm(f64 alpha, mf64 A, mf64 B, f64 beta, mf64 C) -> void {
    static let f = get_fun<void(Layout layout, Trans transa, Trans transb, u32 m, u32 n, u32 k, f64 alpha, f64* a, u32 lda, f64* b, u32 ldb, f64 beta, f64* c, u32 ldc)>("cblas_dgemm");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, Trans::NoTrans, Trans::NoTrans, C._dims[0], C._dims[1], A._dims[1], alpha, A._data, A._step[1], B._data, B._step[1], beta, C._data, C._step[1]);
}

}


// lapack
inline namespace lapack
{

// A = P*L*U
pub fn getrf(mf32 A, vu32 vp) -> void {
    static let f = get_fun<void(Layout layout, u32 m, u32 n, f32* ma, u32 lda, u32* vp)>("cblas_sgetrf");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._dims[1], A._data, A._step[1], vp._data);
}

// A = P*L*U
pub fn getrf(mf64 A, vu32 vp) -> void {
    static let f = get_fun<void(Layout layout, u32 m, u32 n, f64* ma, u32 lda, u32* vp)>("cblas_dgetrf");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._dims[1], A._data, A._step[1], vp._data);
}

// A = QR
pub fn geqrf(mf32 A, vf32 tau) -> void {
    static let f = get_fun<void(Layout layout, u32 m, u32 n, f32* a, u32 lda, f32* tau)>("cblas_sgeqrf");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._dims[1], A._data, A._step[1], tau._data);
}

// A = QR
pub fn geqrf(mf64 A, vf64 tau) -> void {
    static let f = get_fun<void(Layout layout, u32 m, u32 n, f64* a, u32 lda, f64* tau)>("cblas_dgeqrf");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._dims[1], A._data, A._step[1], tau._data);
}

// SVD
// A = U*E*Vt
pub fn gesvd(mf32 A, vf32 s, mf32 U, mf32 VT) -> void {
    static let f = get_fun<i32(Layout layout, char jobu, char jobvt, u32 m, u32 n, f32* a, u32 lda, f32* s, f32* u, u32 ldu, f32* vt, u32 ldvt, f32* superb)>("LAPACKE_gesvd");
    if (f.is_none()) {
        return;
    }
    
    let jobu  =  U.count() == 0 ? 'N' : 'A';
    let jobvt = VT.count() == 0 ? 'N' : 'A';
    let info  = f._val(Layout::Col, jobu, jobvt, A._dims[0], A._dims[1], A._data, A._step[1], s._data, U._data, U._step[1], VT._data, VT._step[1], nullptr);
    (void)info;
}

// A -> A'
pub fn getri(mf32 A, vu32 vp) -> void {
    static let f = get_fun<void(Layout layout, u32 n, f32* ma, u32 lda, u32* vp)>("cblas_sgetri");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._data, A._dims[0], vp._data);
}

// A -> A'
pub fn getri(mf64 A, vu32 vp) -> void {
    static let f = get_fun<void(Layout layout, u32 n, f64* ma, u32 lda, u32* vp)>("cblas_dgetri");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._data, A._step[1], vp._data);
}

// A*x = b
pub fn getrs(mf32 A, vu32 vp, vf32 b) -> void {
    static let f = get_fun<void(Layout layout, u32 n, f32* ma, u32 lda, u32* vp, f32* b, u32 ldb)>("cblas_sgetrs");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._data, A._step[1], vp._data, b._data, b._step[1]);
}

// A*X = B
pub fn getrs(mf64 A, vu32 vp, vf64 b) -> void {
    static let f = get_fun<void(Layout layout, u32 n, f64* ma, u32 lda, u32* vp, f64* b, u32 ldb)>("cblas_sgetrs");
    if (f.is_none()) {
        return;
    }
    
    f._val(Layout::Col, A._dims[0], A._data, A._step[1], vp._data, b._data, b._step[1]);
}

}

// fft
inline namespace fft
{

enum fft_t
{};

class Contex
{

};

}

}

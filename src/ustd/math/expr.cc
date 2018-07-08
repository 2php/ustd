#include "config.inl"

namespace ustd::math
{

unittest(vline)
{
    mut a = NDArray<f32>::with_dims({ 8 });
    assert_eq(a.dims(), { 8 });
    a <<= vline(1.0f);
    for (u32 i = 0; i < a._dims[0]; ++i) {
        assert_eq(a(i), f32(i));
    }
    log::info("a = {}", a);
    a <<= 2.f * a + 10;
    for (u32 i = 0; i < a._dims[0]; ++i) {
        assert_eq(a(i), f32(i)*2.f + 10);
    }
    log::info("a <<= 2.f * a + 10");
    log::info("a = {}", a);
}

unittest(vsum)
{
    mut a1 = NDArray<f32>::with_dims({8});
    log::info("a1 <<= 1");
    a1 <<= 1;
    log::info("a1 = {}", a1);

    mut sum_a1 = scalar_t<f32>(0.f);
    sum_a1 <<= vsum(a1);
    log::info("vsum(a1) = {}", sum_a1());
    assert_eq(sum_a1(), 8.f);

    mut a2 = NDArray<f32, 2>::with_dims({ 8, 8 });
    log::info("a2 <<= 1");
    a2 <<= 1;
    log::info("a2 = {}", a2);
    mut sum_a2 = scalar_t<f32>(0.f);
    sum_a2 <<= vsum(vsum(a2));
    log::info("vsun(vsun(a2)) = {}", sum_a2());
    assert_eq(sum_a2(), 64.f);
}

unittest(axpy)
{

    let cnt     = 1024 * 1024;
    mut x       = NDArray<f32>::with_dims({ cnt });
    mut y       = NDArray<f32>::with_dims({ cnt });
    let a       = 0.1f;
    let loop    = 10;

    x <<= vline(1.f);

    {
        y <<= 1.f;
        let t0 = time::Instant::now();
        let ni = y._dims[0];
        for (mut k = 0u; k < loop; ++k) {
            for (u32 i = 0; i < ni; ++i) {
                y(i) += a * x(i);
            }
        }
        let t1 = time::Instant::now();
        let dur = t1 - t0;
        log::info("user.code: {}", dur);
    }

    {
        blas::axpy(a, x, y);
        y <<= 1.f;
        let t0 = time::Instant::now();
        for (u32 k = 0; k < loop; ++k) {
            blas::axpy(a, x, y);
        }
        let t1 = time::Instant::now();
        let dur = t1 - t0;
        log::info("blas.axpy: {}", dur);
    }

    y <<= 1.f;
    {
        let t0 = time::Instant::now();
        for (u32 k = 0; k < loop; ++k) {
            y += a * x;
        }
        let t1 = time::Instant::now();
        let dur = t1 - t0;
        log::info("ustd.expr: {}", dur);
    }
}

}

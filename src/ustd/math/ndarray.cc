#include "config.inl"

namespace ustd::math
{

unittest(ndarray_v1)
{
    mut a = NDArray<f32>({8});
    mut b = a.slice({ 1u, 5u });
    assert_eq(a.dims(), u32x1{8});
    assert_eq(b.dims(), u32x1{5});
}

unittest(ndarray_v2)
{
    mut a = NDArray<f32, 2>({8, 8});
    mut b = a.slice({ 1, 2 }, { 3, 4 });
    assert_eq(a.dims(), u32x2{ 8u, 8u });
    assert_eq(b.dims(), u32x2{ 2u, 2u });
}

unittest(ndarray_v3)
{
    mut a = NDArray<f32, 3>({8u, 8u, 8u});
    assert_eq(a.dims(), u32x3{ 8u, 8u, 8u });
}

}

#include "config.inl"

namespace ustd::cuda
{

unittest(tex2d) {
    let dims = vec<u32, 2>{256, 256};
    mut arr  = math::NDArray<f32, 2>::with_dims(dims);
    mut tex  = Tex<f32, 2>(dims, TexAddress::Clamp);

    arr <<= math::vline(0.1f, 1.0f);
    tex.set(arr);
    mut out = math::NDArray<f32, 2>::with_dims(dims);
    tex.get(out);

    assert_eq(out(1, 0), 0.1f);
    assert_eq(out(0, 1), 1.0f);
    assert_eq(out(1, 1), 1.1f);
}

unittest(tex3d) {
    let dims = vec<u32, 3>{ 256, 256, 64 };
    mut arr = math::NDArray<f32, 3>::with_dims(dims);
    mut tex = Tex<f32, 3>(dims, TexAddress::Clamp);

    arr <<= math::vline(0.01f, 0.10f, 1.00f);
    tex.set(arr);

    mut out = math::NDArray<f32, 3>::with_dims(dims);
    tex.get(out);

    assert_eq(out(1, 0, 0), 0.01f);
    assert_eq(out(1, 1, 0), 0.11f);
    assert_eq(out(1, 1, 1), 1.11f);
}

}
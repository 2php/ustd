#include "config.inl"

namespace ustd
{

unittest(vec) {
    // f32x3
    let a = f32x3{ 1, 2, 3 };
    let b = 0.1f*a;
    let c = a + b;
    assert_eq(c, f32x3{1.1f, 2.2f, 3.3f});
}

}
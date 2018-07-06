#include "config.inl"

namespace ustd
{

ustd_test(iter) {
    let s = str("hello world");

    mut i = 0;
    for(let c: s.into_iter()) {
        assert_eq(c, s[i++]);
    }
}

}

#include "config.inl"

namespace ustd
{

ustd_test(iter) {
    let s = str("hello");

    for(let c: s.into_iter()) {
        log::info("c = {c}", c);
    }
}

}

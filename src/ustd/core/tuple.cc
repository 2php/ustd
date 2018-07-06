#include "config.inl"

namespace ustd
{

unittest(Tuple) {
    mut tp = Tuple<int, float>{ 0, 1 };
    log::info("tuple = {}", tp);
}

}

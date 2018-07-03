#include "config.inl"

namespace ustd
{

unittest(Tuple) {
    Tuple<int, float> tp;
    tp.$0 = 0;
    tp.$1 = 1;

    log::info("tuple = {}", tp);
}

}

#include "config.inl"

namespace ustd::test
{

fn compare(f64 a, f64 b) -> bool {
    let base    = f64((ustd::abs(a) + ustd::abs(b)) / 2);
    let diff    = f64(ustd::abs(a - b));
    let fpc     = fpclassify(base);

    if (fpc == FP::$zero)   return true;
    if (base<1e-30)         return true;

    let error   = diff / base;
    return error<1e-6f;
}

}

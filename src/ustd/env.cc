#include "config.inl"

namespace ustd::env
{

using namespace ffi;
using namespace io;

pub fn var(str key) -> str {
    if (key.is_empty()) {
        return {};
    }

    let ckey = FixedCStr<1024>(key);

    let val = ::getenv(ckey);
    let res = cstr(val);

    return res;
}

pub fn set_var(str key, str val) -> void {
    if (key.is_empty()) {
        return;
    }

    let ckey = FixedCStr<128*01>(key);
    let cval = FixedCStr<128*31>(val);

#ifdef _UCRT
    ::_putenv_s(ckey, cval);
#else
    ::setenv(ckey, cval, 1);
#endif

}

unittest(test) {
    let os = var("OS");
    log::info("ustd::env::var(key=`{}`) => `{}`", "OS", os);
}

}

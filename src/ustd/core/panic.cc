#include "config.inl"

namespace ustd
{

pub fn panic(const str& msg) -> void {
    log::error(msg);
    throw PanicError();
}

pub fn panic() -> void {
    log::error("ustd::panic");
    throw PanicError();
}

}

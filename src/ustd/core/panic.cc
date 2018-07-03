#include "config.inl"

namespace ustd
{

pub fn panic(str msg) -> void {
    log::error(msg);
    throw PanicError();
}

pub fn panic(const char* msg) -> void {
    log::error(cstr(msg));
    throw PanicError();
}

pub fn panic() -> void {
    log::error("ustd::panic");
    throw PanicError();
}

}

#include "config.inl"

extern "C" pub fn mod_init() -> int;
extern "C" pub fn mod_exit() -> int;

extern "C" pub fn mod_init() -> int {
    printf("[##] ustd: init.\n");
    return 0;
}

extern "C" pub fn mod_exit() -> int {
    printf("[##] ustd: exit.\n");
    return 0;
}

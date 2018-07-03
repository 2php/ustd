#pragma once

#include "ustd/core/str.h"

namespace ustd
{

enum class PanicError
{};

[[noreturn]] pub fn panic()                -> void;
[[noreturn]] pub fn panic(str msg)         -> void;
[[noreturn]] pub fn panic(const char* msg) -> void;

}

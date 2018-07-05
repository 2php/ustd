#pragma once

#include "ustd/core/str.h"

namespace ustd
{

enum class PanicError
{};

pub fn panic()                -> void;
pub fn panic(str msg)         -> void;
pub fn panic(const char* msg) -> void;

}

#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

template<class T>
struct Slice;

using str = Slice<const char>;

enum class PanicError
{};

pub fn panic()                -> void;
pub fn panic(const str& msg)  -> void;

template<class T, class ...U>
fn panic(const str& msg, const T& t, const U& ...u) -> void;

}

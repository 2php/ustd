#pragma once

#include "ustd/core/str.h"

namespace ustd
{

template<class T>
struct Enum;

template<class T>
struct Enum
{
    T   _ok;

    fn name() const noexcept -> str;
};

template<class T>
inline fn Enum<T>::name() const noexcept -> str {
    let s = to_str(_ok);
    return s;
}

}

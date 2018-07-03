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

    __declspec(property(get = get_name)) str name;
    fn get_name() const noexcept -> str;
};

template<class T>
inline fn Enum<T>::get_name() const noexcept -> str {
    let s = to_str(_ok);
    return s;
}

}

#pragma once

#include "ustd/core/fmt.h"
#include "ustd/core/option.h"

namespace ustd
{

struct Type
{
    char _type;
    u16  _size;
    str  _desc;

    __declspec(property(get = get_fullname)) str fullname;
    pub fn get_fullname() const noexcept->str;

    __declspec(property(get = get_name)) str name;
    pub fn get_name() const noexcept->str;

    __declspec(property(get = get_mod)) str mod;
    pub fn get_mod() const noexcept->str;

    pub fn to_str() const noexcept->str;
};

pub fn trait_sfmt(Formatter& fmt, const Type& type) noexcept -> void;

template<typename T>
fn typeof() noexcept -> Type {
    let type = trait<T>::$sint ? 'i' : trait<T>::$uint ? 'u' : trait<T>::$float ? 'f' : '?';
    let size = u16(sizeof(T));
    let desc = str(__PRETTY_FUNCTION__);
    return { type, size, desc };
}

template<>
inline fn typeof<void>() noexcept -> Type {
    let desc = str(__PRETTY_FUNCTION__);
    return { '?', 0,  desc };
}

}

#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

template<class T>
struct property_t
{
    str _key;
    T*  _ptr;

    fn key() const noexcept -> str {
        return _key;
    }

    fn val() const noexcept -> T& {
        return *_ptr;
    }
};

template<class T>
fn make_property(str name, T& val) -> property_t<T> {
    return { name, &val };
}

}
#define ustd_property_begin     constexpr static let $property_idx = ustd::u32(__COUNTER__)
#define ustd_property_end       constexpr static let $property_cnt = ustd::u32(__COUNTER__) - $property_idx - 1
#define ustd_property_begin_x   constexpr static let $property_idx = base::$property_cnt -1

#define ustd_property(name)                                                     \
    name##_t;                                                                   \
private:                                                                        \
    constexpr static let $##name##_idx = __COUNTER__ - $property_idx - 1;       \
public:                                                                         \
    fn get_property(ustd::immut_t<ustd::u32, $##name##_idx>) const noexcept {   \
        return ustd::make_property(ustd::str(#name), name);                     \
    }                                                                           \
    fn get_property(ustd::immut_t<ustd::u32, $##name##_idx>)  noexcept {        \
        return ustd::make_property(ustd::str(#name), name);                     \
    }                                                                           \
    name##_t name

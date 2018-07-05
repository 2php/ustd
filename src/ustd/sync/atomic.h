#pragma once

#include "ustd/core/builtin.h"

namespace ustd::sync
{

template<class T>
fn fetch_and_add(T* target, T value) noexcept -> T {
    return __sync_fetch_and_add(target, value);
}

template<class T>
fn fetch_and_sub(T* target, T value) noexcept -> T {
    return __sync_fetch_and_sub(target, value);
}

template<class T>
fn compare_and_swap(T* target, T expect, T value) noexcept -> bool {
    return __sync_bool_compare_and_swap(target, expect, value);
}

template<class T>
struct Atomic
{
    T _val;

    operator T&() noexcept {
        return _val;
    }

    operator const T&() const noexcept {
        return _val;
    }

    fn operator+=(T val) noexcept -> T {
        return sync::fetch_and_add(&_val, val);
    }

    fn operator-=(T val) noexcept -> T {
        return sync::fetch_and_sub(&_val, val);
    }

    fn compare_and_swap(T old_val, T new_val) -> bool {
        sync::compare_and_swap(&_val, old_val, new_val);
    }

};

}

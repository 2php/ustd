#pragma once

#include "ustd/core/builtin.h"

namespace ustd::sync
{

// { tmp = *ptr; *ptr = val; return tmp; }
template<class T>
fn fetch_and_set(volatile T* ptr, T val) noexcept -> T {
    return __sync_lock_test_and_set(ptr, val);
}

// { tmp = *ptr; *ptr += val; return tmp; }
template<class T>
fn fetch_and_add(volatile T* ptr, T val) noexcept -> T {
    return __sync_fetch_and_add(ptr, val);
}

// { tmp = *ptr; *ptr -= val; return tmp; }
template<class T>
fn fetch_and_sub(volatile T* ptr, T val) noexcept -> T {
    return __sync_fetch_and_sub(ptr, val);
}

// { tmp = *ptr; *ptr &= val; return tmp; }
template<class T>
fn fetch_and_and(volatile T* ptr, T val) noexcept -> T {
    return __sync_fetch_and_sub(ptr, val);
}

// { tmp = *ptr; *ptr &= val; return tmp; }
template<class T>
fn fetch_and_or(volatile T* ptr, T val) noexcept -> T {
    return __sync_fetch_and_sub(ptr, val);
}

template<class T>
fn compare_and_swap(volatile T* ptr, T oldval, T newval) noexcept -> bool {
    return __sync_bool_compare_and_swap(ptr, oldval, newval);
}

}

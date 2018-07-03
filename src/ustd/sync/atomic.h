#pragma once

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
    T _ok;

    operator T&() noexcept {
        return _ok;
    }

    operator const T&() const noexcept {
        return _ok;
    }

    __forceinline fn operator+=(T val) noexcept -> T {
        return sync::fetch_and_add(&_ok, val);
    }

    __forceinline fn operator-=(T val) noexcept -> T {
        return sync::sync_fetch_and_sub(&_dat, val);
    }

    __forceinline fn compare_and_swap(T expect, T newval) -> bool {
        sync::compare_and_swap(&_dat, expect, newval);
    }

};

}

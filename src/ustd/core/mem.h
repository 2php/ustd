#pragma once

#include "ustd/core/builtin.h"
#include "ustd/core/type.h"
#include "ustd/core/vec.h"

namespace ustd
{

pub fn _mnew(Type type, const u64 dims[], u32 rank) noexcept -> void*;
pub fn _mdel(Type type, void* ptr)  noexcept -> void;
pub fn _mcpy(Type type, void* dst, const void* src, const u64 dims[], u32 count) noexcept -> void;

template<class T>
fn mnew(u64 cnt) noexcept -> T* {
    let res = _mnew(typeof<T>(), &cnt, 1);
    return static_cast<T*>(res);
}

template<class T, u32 N>
fn mnew(const u64 (&dims)[N]) noexcept -> T* {
    let res = _mnew(typeof<T>(), dims, N);
    return static_cast<T*>(res);
}

template<class T>
fn mdel(T* ptr) noexcept -> void {
    _mdel(typeof<T>(), ptr);
}

template<class T>
fn mcpy(T* dst, const T* src, u64 count) -> void {
    _mcpy(typeof<T>(), dst, src, &count, 1);
}

template<class T, u32 N>
fn mcpy(T* dst, const T* src, const u64 (&dims)[N]) -> void {
    _mcpy(typeof<T>(), dst, src, dims, 1);
}

template<class T>
fn mmov(T* dst, T* src, u64 count) -> void {
    if constexpr(trivial<T>::$copy) {
        ustd::mcpy(dst, src, count);
    }
    else {
        for (mut i = 0u; i < count; i++) {
            ustd::ctor(&dst[i], as_mov(src[i]));
        }
    }
}

}

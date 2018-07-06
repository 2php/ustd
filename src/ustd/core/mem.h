#pragma once

#include "ustd/core/builtin.h"
#include "ustd/core/type.h"
#include "ustd/core/vec.h"

namespace ustd
{

pub fn _mnew(Type type, u32 rank, const u64 dims[]) noexcept -> void*;
pub fn _mdel(Type type, void* ptr)  noexcept -> void;
pub fn _mcpy(Type type, void* dst, const void* src, u32 rank, const u64 dims[]) noexcept -> void;

template<class T>
fn mnew(u64 cnt) noexcept -> T* {
    let res = _mnew(typeof<T>(), 1, &cnt);
    return static_cast<T*>(res);
}

template<class T, u32 N>
fn mnew(const u64 (&dims)[N]) noexcept -> T* {
    let res = _mnew(typeof<T>(), N, dims);
    return static_cast<T*>(res);
}

template<class T, class I, u32 N>
fn mnew(const vec<I, N>& dims) noexcept -> T* {
    let u64_dims = vec_cast<u64>(dims);
    return mnew<T>(u64_dims._arr);
}

template<class T>
fn mdel(T* ptr) noexcept -> void {
    _mdel(typeof<T>(), ptr);
}

template<class T>
fn mcpy(T* dst, const T* src, u64 count) -> void {
    _mcpy(typeof<T>(), dst, src, 1, &count);
}

template<class T, u32 N>
fn mcpy(T* dst, const T* src, const u64(&dims)[N]) -> void {
    _mcpy(typeof<T>(), dst, src, N, dims);
}

template<class T, class I, u32 N>
fn mcpy(T* dst, const T* src, const vec<I, N>& dims) -> void {
    let u64_dims = vec_cast<u64>(dims);
    _mcpy(typeof<T>(), dst, src, N, u64_dims._arr);
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

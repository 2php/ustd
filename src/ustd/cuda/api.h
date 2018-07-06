#pragma once

#include "ustd/core.h"

namespace ustd::cuda
{

enum class Error: u32
{
    Success = 0,
    Invalid = 1,
};

template<typename T>
using Result = ustd::Result<T, Error>;

pub fn to_str(Error eid) noexcept->str;

enum class stream_t : u64;
enum class arr_t    : u64;
enum class tex_t    : u64;

pub fn to_str(arr_t arr) noexcept->FixedStr<32>;

enum class TexAddress {
    Wrap    = 0,
    Clamp   = 1,
    Mirror  = 2,
    Border  = 3,
};

enum class TexFilter {
    Point   = 0,
    Liner   = 1,
};

#pragma region detail
pub fn _dnew(Type type, u32 rank, const u64 dims[]) noexcept                                -> void*;
pub fn _ddel(Type type, void* ptr)                  noexcept                                -> void;

pub fn _hnew(Type type, u32 rank, const u64 dims[]) noexcept                                -> void*;
pub fn _hdel(Type type, void* ptr)                  noexcept                                -> void;

pub fn _mcpy(Type type, void* dst, const void* src, u32 rank, const u64 dims[]) noexcept    -> void;

pub fn _anew(Type type, u32 rank, const u64 dims[]) noexcept                               -> arr_t;
pub fn _adel(Type type, arr_t arr) noexcept                                                -> void;
pub fn _acpy(Type type, arr_t dst, const void* src, u32 rank, const u64 dims[]) noexcept   -> void;
pub fn _acpy(Type type, void* dst, arr_t       src, u32 rank, const u64 dims[]) noexcept   -> void;

pub fn _tnew(Type type, arr_t arr, TexAddress addr, TexFilter filter) noexcept             -> tex_t;
pub fn _tdel(Type type, tex_t tex) noexcept                                                -> void;

pub fn _sync() noexcept                                                                    -> void;
#pragma endregion

#pragma region context
inline fn sync() noexcept -> void {
    _sync();
}

#pragma endregion

#pragma region memory
template<class T>
fn dnew(u64 cnt) noexcept -> T* {
    let res = _dnew(typeof<T>(), &cnt, 1);
    return static_cast<T*>(res);
}

template<class T, u32 N>
fn dnew(const u64(&dims)[N]) noexcept -> T* {
    let res = _dnew(typeof<T>(), N, dims);
    return static_cast<T*>(res);
}

template<class T, class I, u32 N>
fn dnew(const vec<I, N>& dims) noexcept -> T* {
    let u64_dims = vec_cast<u64>(dims);
    return dnew<T>(u64_dims._arr);
}

template<class T>
fn ddel(T* ptr) noexcept -> void {
    _ddel(typeof<T>(), ptr);
}

template<class T>
fn hnew(u64 cnt) noexcept -> T* {
    let res = _hnew(typeof<T>(), &cnt, 1);
    return static_cast<T*>(res);
}

template<class T, u32 N>
fn hnew(const u64(&dims)[N]) noexcept -> T* {
    let res = _hnew(typeof<T>(), N, dims);
    return static_cast<T*>(res);
}

template<class T, class I, u32 N>
fn hnew(const vec<I, N>& dims) noexcept -> T* {
    let u64_dims = vec_cast<u64>(dims);
    return hnew<T>(u64_dims._arr);
}

template<class T>
fn hdel(T* ptr) noexcept -> void {
    _hdel(ptr);
}

template<class T>
fn mcpy(T* dst, const T* src, u64 count) -> void {
    cuda::_mcpy(typeof<T>(), dst, src, 1, &count);
}

template<class T, u32 N>
fn mcpy(T* dst, const T* src, const u64(&dims)[N]) -> void {
    cuda::_mcpy(typeof<T>(), dst, src, 1, dims);
}

template<class T, class I, u32 N>
fn mcpy(T* dst, const T* src, const vec<I, N>& dims) -> void {
    let u64_dims = vec_cast<u64>(dims);
    cuda::_mcpy(typeof<T>(), dst, src, N, u64_dims._arr);
}

template<class T>
fn tnew(arr_t arr, TexAddress addr, TexFilter filter) -> tex_t {
    let res = _tnew(typeof<T>(), arr, addr, filter);
    return res;
}

template<class T>
inline fn tdel(tex_t tex) -> void {
    _tdel(typeof<T>(), tex);
}
#pragma endregion

#pragma region array
template<class T, u32 N>
fn anew(const u64(&dims)[N]) -> arr_t {
    let res = _anew(typeof<T>(), N, dims);
    return res;
}

template<class T, class I, u32 N>
fn anew(const vec<I, N>& dims) -> arr_t {
    let u64_dims = vec_cast<u64>(dims);
    return cuda::anew<T>(u64_dims._arr);
}

template<class T>
inline fn adel(arr_t arr) -> void {
    _adel(typeof<T>(), arr);
}

template<class T, class I, u32 N>
fn acpy(arr_t dst, const T* src, const vec<I,N>& dims) noexcept -> void {
    let u64_dims = vec_cast<u64>(dims);
    _acpy(typeof<T>(), dst, src, N, u64_dims._arr);
}

template<class T, class I, u32 N>
fn acpy(T* dst, arr_t src, const vec<I,N>& dims) noexcept -> void {
    let u64_dims = vec_cast<u64>(dims);
    _acpy(typeof<T>(), dst, src, N, u64_dims._arr);
}
#pragma endregion

#pragma region stream
pub fn get_stream()                 noexcept -> stream_t;
pub fn set_stream(stream_t stream)  noexcept -> void;
#pragma endregion

}

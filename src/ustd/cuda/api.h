#pragma once

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
pub fn _dnew(Type type, usize cnt) noexcept                                                            -> Result<void*>;
pub fn _ddel(Type type, void* ptr) noexcept                                                            -> Result<void>;

pub fn _hnew(Type type, usize cnt) noexcept                                                            -> Result<void*>;
pub fn _hdel(Type type, void* ptr) noexcept                                                            -> Result<void>;

pub fn _mcpy(Type type, void* dst, const void* src, usize count) noexcept                               -> Result<void>;

pub fn _anew(Type type, u32 rank, const u32 dims[]) noexcept                                -> Result<arr_t>;
pub fn _adel(Type type, arr_t arr) noexcept                                                            -> Result<void>;
pub fn _acpy(Type type, arr_t dst, const void* src, u32 rank, const u32 dims[]) noexcept    -> Result<void>;
pub fn _acpy(Type type, void* dst, arr_t       src, u32 rank, const u32 dims[]) noexcept    -> Result<void>;

pub fn _tnew(Type type, arr_t arr, TexAddress addr, TexFilter filter) noexcept                         -> Result<tex_t>;
pub fn _tdel(Type type, tex_t tex) noexcept                                                            -> Result<void>;

pub fn _sync() noexcept                                                                     -> Result<void>;
#pragma endregion

#pragma region context
inline fn sync() noexcept -> Result<void> {
    let res = _sync();
    return res;
}

#pragma endregion

#pragma region memory
template<class T>
fn dnew(usize cnt) noexcept -> Result<T*> {
    mut res = _dnew(typeof<T>(), cnt);
    return res.map([](void* ptr) { 
        return static_cast<T*>(ptr);
    });
}

template<class T>
fn ddel(T* ptr) noexcept -> void {
    _ddel(typeof<T>(), ptr);
}

template<class T>
fn hnew(usize cnt) noexcept -> Result<T*> {
    mut res = _hnew(typeof<T>(), cnt);
    return res.map([](void* ptr) { return static_cast<T*>(ptr); })
}

template<class T>
fn hdel(T* ptr) noexcept -> void {
    _hdel(ptr);
}

template<class T>
fn mcpy(T* dst, const T* src, usize cnt) noexcept -> void {
    cuda::_mcpy(typeof<T>(), dst, src, cnt);
}

template<class T>
fn tnew(arr_t arr, TexAddress addr, TexFilter filter) -> Result<tex_t> {
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
fn anew(const u32(&dims)[N]) -> Result<arr_t> {
    let rank = N;
    let res = _anew(typeof<T>(), rank, dims);
    return res;
}

template<class T, u32 N>
fn anew(const vec<u32, N>& dims) -> Result<arr_t> {
    return cuda::anew<T>(dims._arr);
}

template<class T>
inline fn adel(arr_t arr) -> void {
    _adel(typeof<T>(), arr);
}

template<class T, u32 N>
fn acpy(arr_t dst, const T* src, vec<u32,N> dims) noexcept -> Result<void> {
    let res  = _acpy(typeof<T>(), dst, src, N, dims._arr);
    return res;
}

template<class T, u32 N>
fn acpy(T* dst, arr_t src, vec<u32,N> dims) noexcept -> Result<void> {
    let res  = _acpy(typeof<T>(), dst, src, N, dims._arr);
    return res;
}
#pragma endregion

#pragma region stream
pub fn get_stream()                 noexcept -> stream_t;
pub fn set_stream(stream_t stream)  noexcept -> void;
#pragma endregion

}

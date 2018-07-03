#pragma once

#include "ustd/core.h"

namespace ustd::sync
{

struct alignas(u64) mtx_t
{
    u8 _[os::mtx_size];
};

template<class T>
using Result = ustd::Result<T, os::Error>;

class Mutex
{
public:
    class Guard;

    mtx_t   _mtx;
    u32     _idx;

    enum {
        ConsoleIdx = 1
    };

    pub Mutex()  noexcept;
    pub ~Mutex() noexcept;

    Mutex(Mutex&& other) noexcept: _mtx(other._mtx), _idx(other._idx) {
        other._idx = 0;
    }

    fn operator==(const Mutex& other) const noexcept -> bool {
        return _idx == other._idx;
    }

    pub fn to_str() const noexcept -> FixedStr<32>;

    pub fn lock() noexcept -> Guard;

protected:
    pub fn unlock() noexcept -> void;
};

class Mutex::Guard
{
public:
    Mutex* _mtx;

    Guard(Guard&& other) noexcept : _mtx(other._mtx) {
        other._mtx = nullptr;
    }

    ~Guard() noexcept {
        if (_mtx == nullptr) return;
        _mtx->unlock();
    }

    fn unlock() noexcept -> void {
        if (_mtx == nullptr) return;
        _mtx->unlock();
        _mtx = nullptr;
    }

    fn forget() noexcept -> void {
        _mtx = nullptr;
    }

protected:
    friend class Mutex;

    Guard(Mutex& mtx) noexcept : _mtx(&mtx)
    {}
};

using MutexGuard = Mutex::Guard;

}

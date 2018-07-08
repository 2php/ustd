#pragma once

#include "ustd/core.h"
#include "ustd/os.h"

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

    enum {
        ConsoleIdx = 1
    };

    pub Mutex()  noexcept;
    pub ~Mutex() noexcept;
    pub Mutex(Mutex&& other) noexcept;

    pub fn lock()   noexcept -> Result<Guard>;
    pub fn unlock() noexcept -> Result<void>;
};

class Mutex::Guard
{
public:
    Mutex* _mtx;

    pub Guard(Guard&& other) noexcept;
    pub ~Guard() noexcept;

    pub fn unlock() noexcept -> void;
    pub fn forget() noexcept -> void;

protected:
    friend class Mutex;
    pub Guard(Mutex* mtx) noexcept;
};

using MutexGuard = Mutex::Guard;

}

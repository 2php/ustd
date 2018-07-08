#pragma once

#include "ustd/core.h"
#include "ustd/time.h"
#include "ustd/sync/mutex.h"

namespace ustd::sync
{

using time::Duration;

struct alignas(u64) cond_t {
    u8 _[os::cnd_size];
};

class CondVar
{
public:
    cond_t _cnd;

    pub CondVar()  noexcept;
    pub ~CondVar() noexcept;
    pub CondVar(CondVar&& other) noexcept;


    pub fn wait           (const MutexGuard& guard)               noexcept -> Result<void>;
    pub fn wait_timeout   (const MutexGuard& guard, Duration dur) noexcept -> Result<void>;
    pub fn wait_timeout_ms(const MutexGuard& guard, u32 ms)       noexcept -> Result<void>;

    pub fn notify_one() noexcept -> Result<void>;
    pub fn notify_all() noexcept -> Result<void>;
};

}

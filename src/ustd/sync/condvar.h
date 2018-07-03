#pragma once

#include "ustd/core.h"
#include "ustd/time.h"
#include "ustd/sync/mutex.h"

namespace ustd::sync
{

using time::Duration;

struct alignas(u64) condvar_t {
    u8 _[os::cnd_size];
};

class CondVar
{
public:
    condvar_t _cnd;
    u32       _idx;

    pub CondVar()  noexcept;
    pub ~CondVar() noexcept;

    CondVar(CondVar&& other) noexcept: _cnd(other._cnd), _idx(other._idx) {
        other._idx = 0;
    }

    fn operator==(const CondVar& other) const noexcept -> bool {
        return _idx == other._idx;
    }

    pub fn wait           (const MutexGuard& guard)               noexcept -> Result<none_t>;
    pub fn wait_timeout   (const MutexGuard& guard, Duration dur) noexcept -> Result<none_t>;
    pub fn wait_timeout_ms(const MutexGuard& guard, u32 ms)       noexcept -> Result<none_t>;

    pub fn notify_one() noexcept -> void;
    pub fn notify_all() noexcept -> void;

    pub fn to_str() const noexcept -> FixedStr<32>;
};

}

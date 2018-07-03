#include "config.inl"

#if __has_include(<pthread.h>)
#   define USTD_SYNC_PTHREAD
#   include <pthread.h>
#elif defined(_WIN32)
#   define USTD_SYNC_WIN32
#endif


#ifdef USTD_SYNC_WIN32
using ustd::sync::condvar_t;
using ustd::sync::mtx_t;

extern "C" fn InitializeConditionVariable(condvar_t*)                   -> void;
extern "C" fn WakeConditionVariable(condvar_t*)                         -> void;
extern "C" fn WakeAllConditionVariable(condvar_t*)                      -> void;
extern "C" fn SleepConditionVariableCS(condvar_t*, mtx_t* lock, u32 ms) -> i32;

namespace ustd::sync
{

pub CondVar::CondVar()  noexcept {
    static mut gidx = 1u;
    _idx = sync::fetch_and_add(&gidx, 1u);

    log::trace("ustd::sync::CondVar[{}].ctor()...", *this);
    ::InitializeConditionVariable(&_cnd);
}

pub CondVar::~CondVar() noexcept {
}

pub fn CondVar::wait(const MutexGuard& guard) noexcept -> Result<none_t> {
    if (_idx == 0) {
        log::error("ustd::sync::CondVar[{}].wait(guard=[mtx={}]) -> Error()", *this, guard._mtx);
        return Result<none_t>::Err(os::Error::InvalidData);
    }

    log::trace("ustd::sync::CondVar[{}].wait()...", *this);

    let infinate = 0u - 1u;
    let res      = ::SleepConditionVariableCS(&_cnd, &guard._mtx->_mtx, infinate);
    if (res == 0) {
        let eid = os::windows::get_error();
        log::warn("ustd::sync::CondVar[{}].wait(guard=#) -> Error(`{}`)", *this, eid);
        return Result<none_t>::Err(eid);
    }
    return Result<none_t>::Ok();
}

pub fn CondVar::wait_timeout(const MutexGuard& guard, Duration dur) noexcept -> Result<none_t> {
    log::trace("ustd::sync::CondVar[{}].wait_timeout(guard=#, dur={}) ...", *this, dur);

    let ms  = u32(dur.total_millis() + 0.5);
    let res = ::SleepConditionVariableCS(&_cnd, &guard._mtx->_mtx, ms);

    if (res == 0) {
        let eid = os::windows::get_error();

        eid == os::Error::TimedOut
            ? log::debug("ustd::sync::CondVar[{}].wait_timeout(guard=#, dur={}) -> Error(`{}`)", *this, ms, eid)
            : log::error("ustd::sync::CondVar[{}].wait_timeout(guard=#, dur={}) -> Error(`{}`)", *this, ms, eid);

        return Result<none_t>::Err(eid);
    }
    return Result<none_t>::Ok();
}

pub fn CondVar::wait_timeout_ms(const MutexGuard& guard, u32 timeout) noexcept -> Result<none_t> {
    log::trace("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms)...", *this, timeout);

    let res = ::SleepConditionVariableCS(&_cnd, &guard._mtx->_mtx, timeout);

    if (res == 0) {
        let eid = os::windows::get_error();
        eid != os::Error::TimedOut
            ? log::error("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms) -> Error(`{}`)", *this, timeout, eid)
            : log::debug("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms) -> Error(`{}`)", *this, timeout, eid);
        return Result<none_t>::Err(os::get_error());
    }

    log::trace("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms) -> Ok()", *this, timeout);

    return Result<none_t>::Ok();
}

pub fn CondVar::notify_one() noexcept -> void {
    log::trace("ustd::sync::CondVar[{}].notify_one() ...", *this);
    ::WakeConditionVariable(&_cnd);
}

pub fn CondVar::notify_all() noexcept -> void {
    log::trace("ustd::sync::CondVar[{}].notify_all()", *this);
    ::WakeAllConditionVariable(&_cnd);
}

pub fn CondVar::to_str() const noexcept -> FixedStr<32> {
    let res = snformat<32>("{{idx={}}}", _idx);
    return res;
}

}

#endif

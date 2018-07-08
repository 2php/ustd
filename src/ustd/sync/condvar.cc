#include "config.inl"

#ifdef USTD_OS_WIN32
using ustd::sync::cond_t;
using ustd::sync::mtx_t;
extern "C" fn InitializeConditionVariable(cond_t*)                   -> void;
extern "C" fn WakeConditionVariable(cond_t*)                         -> void;
extern "C" fn WakeAllConditionVariable(cond_t*)                      -> void;
extern "C" fn SleepConditionVariableCS(cond_t*, mtx_t* lock, u32 ms) -> i32;

namespace ustd::sync
{
static fn cond_init(cond_t* cnd) -> void {
    ::InitializeConditionVariable(&cnd);
}

static fn cond_destroy(cond_t* cnd) -> void {
}

static fn cond_wait_ms(u32 ms) -> bool {
    let stat     = ::SleepConditionVariableCS(&_cnd, &guard._mtx->_mtx, ms);
    return stat != 0;
}

static fn cond_wait() -> bool {
    let infinate = 0u - 1u;
    return cond_wait_ms(infinate);
}

static fn cond_notify_one(cond_t* cond) -> void {
    ::WakeConditionVariable(cond);
}

static fn cond_notify_all(cond_t* cond) -> void {
    ::WakeAllConditionVariable(cond);
}
}
#endif

#ifdef USTD_OS_UNIX
namespace ustd::sync
{
static fn cond_cast(cond_t* cond) -> pthread_cond_t* {
    return reinterpret_cast<pthread_cond_t*>(cond);
}

static fn mtx_cast(mtx_t* mtx) -> pthread_mutex_t* {
    return reinterpret_cast<pthread_mutex_t*>(mtx);
}

static fn cond_init(cond_t* cond) -> bool {
    let eid = ::pthread_cond_init(cond_cast(cond), nullptr);
    return eid == 0;
}

static fn cond_destroy(cond_t* cond) -> bool {
    let eid = ::pthread_cond_destroy(cond_cast(cond));
    return eid == 0;
}

static fn cond_wait(cond_t* cond, mtx_t* mtx) -> bool {
    let eid = ::pthread_cond_wait(cond_cast(cond), mtx_cast(mtx));
    return eid == 0;
}

static fn cond_wait_dur(cond_t* cond, mtx_t* mtx, time::Duration dur) -> bool {
    let time_now = time::SystemTime::now();
    let time_out = time_now + dur;
    struct timespec ts;
    ts.tv_sec  = time_out._secs;
    ts.tv_nsec = time_out._nanos;

    let eid  = ::pthread_cond_timedwait(cond_cast(cond), mtx_cast(mtx), &ts);
    return eid == 0;
}

static fn cond_wait_ms(cond_t* cond, mtx_t* mtx, u32 ms) -> bool {
    let dur = time::Duration::from_millis(ms);
    return cond_wait_dur(cond, mtx, dur);
}

static fn cond_notify_one(cond_t* cond) -> bool {
    let eid = ::pthread_cond_signal(cond_cast(cond));
    return eid == 0;
}

static fn cond_notify_all(cond_t* cond) -> bool {
    let eid = ::pthread_cond_broadcast(cond_cast(cond));    
    return eid == 0;
}
}
#endif

namespace ustd::sync
{

static fn is_zero(const cond_t& cnd) -> bool {
    for(mut i = 0; i < os::cnd_size; ++i){
        if (cnd._[i] != 0) return false;
    }
    return true;
}

static fn to_zero(cond_t& cnd) -> void {
    for(mut i = 0; i < os::cnd_size; ++i){
        cnd._[i] = 0;
    }
}

pub CondVar::CondVar()  noexcept {
    log::trace("ustd::sync::CondVar[{}].ctor()...", this);
    cond_init(&_cnd);
}

pub CondVar::~CondVar() noexcept {
    if (is_zero(_cnd)) {
        return;
    }
    cond_destroy(&_cnd);
}

pub CondVar::CondVar(CondVar&& other) noexcept: _cnd(other._cnd) {
    to_zero(other._cnd);
}

pub fn CondVar::wait(const MutexGuard& guard) noexcept -> Result<void> {
    log::trace("ustd::sync::CondVar[{}].wait()...", this);

    let stat = cond_wait(&_cnd, &guard._mtx->_mtx);
    if (!stat) {
        let eid = os::get_error();
        log::warn("ustd::sync::CondVar[{}].wait(guard=#) -> Error(`{}`)", this, eid);
        return Result<void>::Err(eid);
    }
    return Result<void>::Ok();
}

pub fn CondVar::wait_timeout(const MutexGuard& guard, Duration dur) noexcept -> Result<void> {
    log::trace("ustd::sync::CondVar[{}].wait_timeout(guard=#, dur={}) ...", this, dur);

    let stat = cond_wait_dur(&_cnd, &guard._mtx->_mtx, dur);
    if (!stat) {
        let eid = os::get_error();

        eid == os::Error::TimedOut
            ? log::debug("ustd::sync::CondVar[{}].wait_timeout(guard=#, dur={}) -> Error(`{}`)", this, dur, eid)
            : log::error("ustd::sync::CondVar[{}].wait_timeout(guard=#, dur={}) -> Error(`{}`)", this, dur, eid);

        return Result<void>::Err(eid);
    }
    return Result<void>::Ok();
}

pub fn CondVar::wait_timeout_ms(const MutexGuard& guard, u32 timeout) noexcept -> Result<void> {
    log::trace("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms)...", this, timeout);

    let stat = cond_wait_ms(&_cnd, &guard._mtx->_mtx, timeout);

    if (!stat) {
        let eid = os::get_error();
        eid != os::Error::TimedOut
            ? log::error("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms) -> Error(`{}`)", this, timeout, eid)
            : log::debug("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms) -> Error(`{}`)", this, timeout, eid);
        return Result<void>::Err(eid);
    }

    log::trace("ustd::sync::CondVar[{}].wait_timeout_ms(guard=#, timeout={}ms) -> Ok()", this, timeout);

    return Result<void>::Ok();
}

pub fn CondVar::notify_one() noexcept -> Result<void> {
    log::trace("ustd::sync::CondVar[{}].notify_one() ...", this);
    let stat = cond_notify_one(&_cnd);
    if (!stat) {
        let eid = os::get_error();
        log::error("ustd::sync::CondVar[{}].notify_one() -> Error(`{}`)", this, eid);
        return Result<void>::Err(eid);
    }
    return Result<void>::Ok();
}

pub fn CondVar::notify_all() noexcept -> Result<void> {
    log::trace("ustd::sync::CondVar[{}].notify_all()", this);
    let stat = cond_notify_all(&_cnd);
    if (!stat) {
        let eid = os::get_error();
        log::error("ustd::sync::CondVar[{}].notify_one() -> Error(`{}`)", this, eid);
        return Result<void>::Err(eid);
    }
    return Result<void>::Ok();
}

}


namespace ustd::sync
{

unittest(CondVar) {
    mut mutex    = Mutex();
    mut cond_var = CondVar();

    volatile mut value    = 10;

    mut t1 = thread::spawn([&]{
        while(true) {
            let lock = mutex.lock().unwrap();
            cond_var.wait_timeout_ms(lock, 1000);
            if (value<=0) {
                break;
            }
            log::info("recv: value = {}", value);
        }
    });

    mut t2 = thread::spawn([&]{
        while(value>=0) {
            {
                let lock = mutex.lock().unwrap();
                log::info("send: value = {}", value);
                cond_var.notify_one();
                
            }
            thread::sleep_ms(100);
            --value;
        }
    });

    t1.join();
    t2.join();
}

}
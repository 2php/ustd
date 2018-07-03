#include "config.inl"

#if __has_include(<pthread.h>)
#   define USTD_SYNC_PTHREAD
#   include <pthread.h>
#elif defined(_WIN32)
#   define USTD_SYNC_WINDOWS
#endif

#ifdef USTD_SYNC_PTHREAD
namespace ustd::sync
{

pub Mutex::Mutex() noexcept {

    static let attr = []() {
        pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_settype(&attr, 0);
        return attr;
    }();

    let eid = ::pthread_mutex_init(&_mtx, &attr);
    (void)eid;
}

pub Mutex::~Mutex() noexcept {
    if (!_valid) return;

    let eid = ::pthread_mutex_destroy(&_mtx);
    (void)eid;
}

pub fn Mutex::lock() noexcept -> Guard {
    if (!_valid) return LockResult<Guard>::Err(os::Error(EINVAL));

    let eid = ::pthread_mutex_lock(&_mtx);
    if (eid != 0) {
        ustd::panic("ustd::sync::Mutex[{}, thr={}].lock() -> Error(`{}`)", *this, thr, eid);
    }
    return { $type<Guard>, *this };
}

pub fn Mutex::unlock() noexcept -> void {
    if (!_valid) return LockResult<None>::Err(os::Error(EINVAL));

    let eid = ::pthread_mutex_unlock(&_mtx);
    if (eid != 0) {
        ustd::panic("ustd::sync::Mutex[{}, thr={}].unlock() -> Error(`{}`)", *this, thr, eid);
    }
}

}
#endif

#ifdef USTD_SYNC_WINDOWS
using ustd::sync::mtx_t;
extern "C" void InitializeCriticalSection(mtx_t*);
extern "C" void DeleteCriticalSection(mtx_t*);

extern "C" bool TryEnterCriticalSection(mtx_t*);
extern "C" void EnterCriticalSection(mtx_t*);
extern "C" void LeaveCriticalSection(mtx_t*);

namespace ustd::sync
{


pub Mutex::Mutex() noexcept: _idx(0) {
    static mut gidx = 100u;
    _idx = sync::fetch_and_add(&gidx, 1u);

    ::InitializeCriticalSection(&_mtx);
}

pub Mutex::~Mutex() noexcept {
    if (_idx == 0) return;

    ::DeleteCriticalSection(&_mtx);
}

pub fn Mutex::lock() noexcept -> Guard {
    if (_idx == 0) {
        ustd::panic("ustd::sync::Mutex[{}].lock() -> Error(`Invalid`)", *this);
    }

    ::EnterCriticalSection(&_mtx);
    return Guard(*this);
}

pub fn Mutex::unlock() noexcept -> void {
    if (_idx == 0) {
        ustd::panic("ustd::sync::Mutex[{}][thr={}].unlock() -> Error(`invalid`)", *this);
    }

    ::LeaveCriticalSection(&_mtx);
}

pub fn Mutex::to_str() const noexcept -> FixedStr<32> {
    let res = snformat<32>("{{idx={}}}", _idx);
    return res;
}

}
#endif

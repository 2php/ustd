#include "config.inl"


#ifdef USTD_OS_WINDOWS
using ustd::sync::mtx_t;
extern "C" void InitializeCriticalSection(mtx_t*);
extern "C" void DeleteCriticalSection(mtx_t*);

extern "C" bool TryEnterCriticalSection(mtx_t*);
extern "C" void EnterCriticalSection(mtx_t*);
extern "C" void LeaveCriticalSection(mtx_t*);

namespace ustd::sync
{
static fn mutex_init(mtx_t* mtx) {
    ::InitializeCriticalSection(mtx);
}

static fn mutex_destroy(mtx_t* mtx) {
    ::DeleteCriticalSection(mtx);
}

static fn mutex_lock(mtx_t* mtx) -> bool {
    ::EnterCriticalSection(&_mtx);
    return true;
}

static fn mutex_unlock(mtx_t* mtx) -> bool {
    ::LeaveCriticalSection(mtx);
    return true;
}
}
#endif

#ifdef USTD_OS_UNIX
namespace ustd::sync
{
static fn mtx_cast(mtx_t* mtx) -> pthread_mutex_t* {
    return reinterpret_cast<pthread_mutex_t*>(mtx);
}

static fn mutex_init(mtx_t* mtx) -> bool {
    let stat = ::pthread_mutex_init(mtx_cast(mtx), nullptr);
    return stat == 0;
}

static fn mutex_destroy(mtx_t* mtx) {
    let stat = ::pthread_mutex_destroy(mtx_cast(mtx));
    return stat == 0;
}

static fn mutex_lock(mtx_t* mtx) -> bool {
    let stat = ::pthread_mutex_lock(mtx_cast(mtx));
    return stat == 0;
}

static fn mutex_unlock(mtx_t* mtx) -> bool {
    let stat = ::pthread_mutex_unlock(mtx_cast(mtx));
    return stat == 0;
}
}
#endif

namespace ustd::sync
{

#pragma region mutex

static fn is_zero(const mtx_t& mtx) -> bool {
    for(mut i = 0; i < os::mtx_size; ++i){
        if (mtx._[i] != 0) return false;
    }
    return true;
}

static fn to_zero(mtx_t& mtx) -> void {
    for(mut i = 0; i < os::mtx_size; ++i){
        mtx._[i] = 0;
    }
}

pub Mutex::Mutex() noexcept {
    mutex_init(&_mtx);
}

pub Mutex::~Mutex() noexcept {
    if (is_zero(_mtx)) {
        return;
    }
    mutex_destroy(&_mtx);
}

pub Mutex::Mutex(Mutex&& other) noexcept: _mtx(other._mtx) {
    to_zero(other._mtx);
}

pub fn Mutex::lock() noexcept -> Result<Guard> {
    let stat = mutex_lock(&_mtx);
    if (!stat) {
        let eid = os::get_error();
        log::error("ustd::sync::Mutex[{}].lock() -> Error(`{}`)", this, eid);
        return Result<Guard>::Err(eid);
    }
    mut res = Guard(this);
    return Result<Guard>::Ok(as_mov(res));
}

pub fn Mutex::unlock() noexcept -> Result<void> {
    let stat = mutex_unlock(&_mtx);
    if (!stat) {
        let eid = os::get_error();
        log::error("ustd::sync::Mutex[{}].unlock() -> Error(`{}`)", this, eid);
        return Result<void>::Err(eid);
    }
    return Result<void>::Ok();
}

#pragma endregion 

#pragma region mutex_guard

pub Mutex::Guard::Guard(Mutex* mtx) noexcept : _mtx(mtx)
{}

pub Mutex::Guard::Guard(Mutex::Guard&& other) noexcept : _mtx(other._mtx) {
        other._mtx = nullptr;
}

pub Mutex::Guard::~Guard() noexcept {
    if (_mtx == nullptr) {
        return;
    }
    _mtx->unlock();
}

pub fn Mutex::Guard::unlock() noexcept -> void {
    if (_mtx == nullptr) {
        return;
    }
    _mtx->unlock();
    _mtx = nullptr;
}

pub fn Mutex::Guard::forget() noexcept -> void {
    _mtx = nullptr;
}
#pragma endregion

}

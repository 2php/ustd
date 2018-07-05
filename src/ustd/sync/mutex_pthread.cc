#include "config.inl"

namespace ustd::sync
{

static fn mtx_cast(mtx_t& mtx) -> pthread_mutex_t& {
    return reinterpret_cast<pthread_mutex_t&>(mtx);
}

pub Mutex::Mutex() noexcept {
    static mut gidx = 100u;
    _idx = sync::fetch_and_add(&gidx, 1u);

    static let attr = []() {
        pthread_mutexattr_t attr;
        ::pthread_mutexattr_init(&attr);
        ::pthread_mutexattr_settype(&attr, 0);
        return attr;
    }();

    let eid = ::pthread_mutex_init(&mtx_cast(_mtx), &attr);
    (void)eid;
}

pub Mutex::~Mutex() noexcept {
    if (_idx == 0) {
        return;
    }

    let eid = ::pthread_mutex_destroy(&mtx_cast(_mtx));
    (void)eid;
}

pub fn Mutex::lock() noexcept -> Result<Guard> {
    if (_idx == 0) {
        return Result<Guard>::Err(os::Error::InvalidData);
    }

    let eid = ::pthread_mutex_lock(&mtx_cast(_mtx));
    if (eid != 0) {
        log::error("ustd::sync::Mutex[{}].lock() -> Error(`{}`)", *this, eid);
        return Result<Guard>::Err(os::get_error());
    }

    mut guard = Guard(*this);
    return Result<Guard>::Ok(as_mov(guard));
}

pub fn Mutex::unlock() noexcept -> Result<void> {
    if (_idx == 0) {
        return Result<void>::Err(os::Error::InvalidData);
    }

    let eid = ::pthread_mutex_unlock(&mtx_cast(_mtx));
    if (eid != 0) {
        log::error("ustd::sync::Mutex[{}].unlock() -> Error(`{}`)", *this, eid);
        return Result<void>::Err(os::get_error());
    }

    return Result<void>::Ok();
}

pub fn Mutex::to_str() const noexcept -> FixedStr<32> {
    let res = snformat<32>("{{idx={}}}", _idx);
    return res;
}

}

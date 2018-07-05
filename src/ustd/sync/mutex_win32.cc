#include "config.inl"


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
    if (_idx == 0) {
        return;
    }

    ::DeleteCriticalSection(&_mtx);
}

pub fn Mutex::lock() noexcept -> Result<Guard> {
    if (_idx == 0) {
        ustd::panic("ustd::sync::Mutex[{}].lock() -> Error(`Invalid`)", *this);
        return Result<Guard>::Err(os::Error::InvalidData);
    }

    ::EnterCriticalSection(&_mtx);
    mut guard = Guard(*this);
    return Result<Guard>::Ok(as_mov(guard));
}

pub fn Mutex::unlock() noexcept -> Result<void> {
    if (_idx == 0) {
        ustd::panic("ustd::sync::Mutex[{}][thr={}].unlock() -> Error(`invalid`)", *this);
        return Result<void>::Err(os::Error::InvalidData);
    }

    ::LeaveCriticalSection(&_mtx);
    return Result<void>::Ok();
}

pub fn Mutex::to_str() const noexcept -> FixedStr<32> {
    let res = snformat<32>("{{idx={}}}", _idx);
    return res;
}

}

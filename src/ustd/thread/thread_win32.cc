#include "config.inl"

#if defined(_WIN32)

using thr_t = ustd::thread::thr_t;

extern "C" fn GetThreadId(thr_t thr) -> u32;
extern "C" fn CloseHandle(thr_t thr) -> u32; // S_OK[0], S_FALSE[1]
extern "C" fn GetCurrentThread() -> thr_t;
extern "C" fn CreateThread(void* attr, u64 stack_size, int(*func)(void*), void* params, u32 flags, void* id) -> thr_t;
extern "C" fn GetExitCodeThread(thr_t thr, u32* code) -> bool ;
extern "C" fn WaitForSingleObject(thr_t thr, u32 ms) -> u32;
extern "C" fn SwitchToThread()-> u32;
extern "C" fn LocalFree(void*) -> void;
extern "C" fn GetThreadDescription(thr_t thr, wchar_t** desc)->u32;
extern "C" fn SetThreadDescription(thr_t thr, const wchar_t* desc)->u32;

namespace ustd::thread
{


pub fn Thread::detach() noexcept -> void {
    if (!is_valid()) return;

    let res = ::CloseHandle(_thr);

    if (res != 0) {
        let eid = os::get_error();
        log::error("ustd::thread::Thread[{}].detach() -> Error(`{}`)", *this, eid);
    }

    _thr = thr_t::Invalid;
    log::debug("ustd::thread::Thread[{}].detach() -> Ok()", *this);
}

pub fn Thread::join() noexcept -> Result<void> {
    if (!is_valid()) {
        return Result<void>::Err(os::Error::InvalidData);
    }

    let $wait_failed = u32(0xFFFFFFFF);
    let $wait_time   = u32(0xFFFFFFFF);

    log::debug("ustd::thread::Thread[{}].join()...", *this);

    // thread: join
    if (::WaitForSingleObject(_thr, $wait_time) == $wait_failed) {
        let eid = os::get_error();
        log::error("ustd::thread::Thread[{}].join() -> Error(`{}`)", *this, eid);
        return Result<void>::Err(eid);
    }

    // thread: get return value
    mut res = u32(0);
    let ret = ::GetExitCodeThread(_thr, &res);
    if (!ret) {
        log::warn("ustd::thread::Thread[{}].join() -> Error(`{}`)", *this, res);
        return Result<void>::Err(os::Error::Other);
    }

    log::debug("ustd::thread::Thread[{}].join() -> Ok()", *this);

    // thread: close
    ::CloseHandle(_thr);
    _thr = thr_t::Invalid;

    return Result<void>::Ok();
}

pub fn Thread::set_name(str name)  noexcept -> void {
    (void)name;

    if (!is_valid()) {
        return;
    }

#ifdef WIN10_1607
    if (name.is_empty()) {
        name = "ustd::thread";
    }

    // str->osstr
    wchar_t wstr[256];
    mut num = usize(0);
    let ret = ::mbstowcs_s(&num, wstr, name.data, name.len);
    if (ret != 0) return;

    // setname
    let res = ::SetThreadDescription(_thr, wstr);
    (void)res;
#endif

}

pub fn Thread::name() const noexcept -> FixedStr<256> {
    if (!is_valid()) {
        return {};
    }

    // osstr->str
    FixedStr<256> name;

#ifdef WIN10_1607
    wchar_t* wstr;
    let ret = ::GetThreadDescription(_thr, &wstr);
    if ((ret & 0x7000000) != 0) {
        return {};
    }

    mut num = usize(0);
    let len = ::__builtin_wcslen(wstr);
    ::wcstombs_s(&num, name.data, name.capacity, wstr, len);
    if (len <= 0u) return {};

    name._size = u32(len);

    ::LocalFree(wstr);

#endif
    return name;
}

pub fn Thread::id() const noexcept -> u64 {
    if (!is_valid()) return 0u;

    let tid = ::GetThreadId(_thr);
    return tid;
}

static fn win32_thread_callback(void* args) noexcept -> i32 {
    using res_t = FnBox<void()>::res_t;

    mut fun = FnBox<void()>::from_raw(static_cast<res_t*>(args));
    mut res = 0;

    try {
        fun();
    }
    catch (...) {
        res = -1;
    }

    fun.forget();

    return res;
}

pub fn Builder::spawn_fn(FnBox<void()>::res_t* addr) const noexcept -> Thread {
    let thr = ::CreateThread(nullptr, _stack, &win32_thread_callback, addr, 0, nullptr);
    mut res = Thread(thr);

    if (thr == thr_t(0)) {
        let eid = os::get_error();
        ustd::panic("ustd::thread::Thread.spawn_fnbox(func=#) -> Error({})", eid);
    }
    else {
        if (_name != str()) {
            res.set_name(_name);
        }
        log::debug("ustd::thread::Thread.spawn_fnbox(func=#) -> Ok({}})", res);
    }

    return res;
}

pub fn current() noexcept -> Thread {
    let thr = ::GetCurrentThread();
    return Thread{ thr };
}

pub fn yield() noexcept -> void {
    ::SwitchToThread();
}

}

#endif

#include "config.inl"

#ifdef USTD_OS_WINDOWS
extern "C" fn GetThreadId(thr_t thr) -> u32;
extern "C" fn CloseHandle(thr_t thr) -> u32;
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
static fn thread_detach(thr_t thr) -> bool {
    let stat  = ::CloseHandle(_thr);
    return eid != 0;
}    

static fn thread_join(thr_t thr) -> bool {
    let time_out  = 0u - 1u;
    let wait_stat = ::WaitForSingleObject(_thr, time_out);

    mut thrd_res  = u32(0);
    let thrd_stat = ::GetExitCodeThread(_thr, &res);
    if ()
    if (wait_stat != 0) {
        return false;
    }
        if 
        let eid = os::get_error();
        log::error("ustd::thread::Thread[{}].join() -> Error(`{}`)", *this, eid);
        return Result<void>::Err(eid);
    }
}

static fn thread_yield() -> bool {
    ::SwitchToThread();
    return true;
}

static fn thread_current() -> thr_t {
    let thr = ::GetCurrentThread();
    return thr_t(u64(thr));
}
static fn thread_callback(void* args) noexcept -> i32 {
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

static fn thread_create(u32 stack, FnBox<void()>::res_t* func) -> thr_t {
    let thr = ::CreateThread(nullptr, stack, &thread_callback, addr, 0, nullptr);
    return thr;
}
#endif

#ifdef USTD_OS_UNIX
namespace ustd::thread
{
static fn thread_detach(thr_t thr) -> bool {
    let eid = ::pthread_detach(pthread_t(thr));
    return eid == 0;
}    

static fn thread_join(thr_t thr) -> bool {
    mut ptr = (void*)nullptr;
    let eid = ::pthread_join(pthread_t(thr), &ptr);
    return eid == 0;
}

static fn thread_yield() -> bool {
#ifdef USTD_OS_MACOS
    ::pthread_yield_np();
#else
    ::pthread_yield();
#endif   
    return true;
}

static fn thread_current() -> thr_t {
    let thr = ::pthread_self();
    return thr_t(u64(thr));
}

static fn thread_callback(void* args) -> void* {
    using res_t = FnBox<void()>::res_t;

    mut fun     = FnBox<void()>::from_raw(static_cast<res_t*>(args));
    let error   = reinterpret_cast<void*>(-1);
    mut res     = error;

    try {
        fun();
        res = nullptr;
    }
    catch (...) {
    }
    fun.forget();

    return res;
}

static fn thread_create(u32 stack, FnBox<void()>::res_t* func) -> thr_t {
    pthread_attr_t attr;
    ::pthread_attr_init(&attr);
    ::pthread_attr_setstacksize(&attr, stack);

    mut thr = pthread_t();
    let ret = ::pthread_create(&thr, &attr, &thread_callback, func);
    return thr_t(u64(thr));
}

}
#endif

namespace ustd::thread
{

pub fn Thread::detach() noexcept -> void {
    if (_thr == thr_t::Invalid) {
        return;
    }

    thread_detach(_thr);
    _thr = thr_t::Invalid;
}

pub fn Thread::join() noexcept -> Result<void> {
    if (_thr == thr_t::Invalid) {
        return Result<void>::Err(os::Error::InvalidData);
    }

    let stat = thread_join(_thr);
    if (!stat) {
        return Result<void>::Err(os::get_error());
    }

    _thr = thr_t::Invalid;
    return Result<void>::Ok();
}


pub fn Builder::spawn_fn(FnBox<void()>::res_t* func) const noexcept -> Thread {
    let thr = thread_create(_stack, func);
    return Thread(thr);
}

pub fn current() noexcept -> Thread {
    let tid = thr_t(u64(::pthread_self()));
    return Thread{ tid };
}


pub fn yield() noexcept -> void {
    thread_yield();
}

pub fn Thread::is_valid() const noexcept -> bool {
    return _thr != thr_t::Invalid;
}

pub fn trait_sfmt(Formatter& fmt, const Thread& thr) noexcept -> void {
    fmt.write_fmt("{}", u64(thr._thr));
}


unittest(Thread) {
    mut val = 0;

    mut t0 = thread::spawn([&]() {
        thread::sleep_ms(50);
        log::info("ustd::thread::test[Thread]: val = 1");
        val = 1;
    });

    mut t1 = thread::spawn([&]() {
        thread::sleep_ms(100);
        log::info("ustd::thread::test[Thread]: val = 2");
        val = 2;
    });

    assert_eq(val, 0);

    t0.join();
    assert_eq(val, 1);

    t1.join();
    assert_eq(val, 2);
}

}

#include "config.inl"

namespace ustd::thread
{

pub fn Thread::detach() noexcept -> void {
    if (_thr == thr_t::Invalid) return;

    let eid = ::pthread_detach(pthread_t(_thr));
    (void)eid;

    _thr = thr_t::Invalid;
}

pub fn Thread::join() noexcept -> Result<void> {
    if (_thr == thr_t::Invalid) {
        return Result<void>::Err(os::Error::InvalidData);
    }

    mut ret = (void*)nullptr;
    let eid = ::pthread_join(pthread_t(_thr), &ret);
    if (eid != 0) return Result<void>::Err(os::get_error());

    _thr = thr_t::Invalid;
    return Result<void>::Ok();
}

pub fn Thread::id() const noexcept -> u64 {
    return u64(_thr);
}

pub fn Thread::name() const noexcept -> FixedStr<256> {
    return "";
}

pub fn Thread::set_name(str name) noexcept -> void {
    (void)name;
}

static fn posix_thread_callback(void* args) -> void* {
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

pub fn Builder::spawn_fn(FnBox<void()>::res_t* addr) const noexcept -> Thread {
    let attr = [=]() {
        pthread_attr_t attr;
        ::pthread_attr_init(&attr);
        ::pthread_attr_setstacksize(&attr, _stack);
        return attr;
    }();

    mut thr = pthread_t();
    let ret = ::pthread_create(&thr, &attr, &posix_thread_callback, addr);
    let res = Thread(thr_t(u64(thr)));

    if (ret != 0) {
        return res;
    }

#ifndef USTD_OS_MACOS
    let name = FixedCStr<1024>(_name);
    ::pthread_setname_np(thr, name);
#endif

    return res;
}

pub fn current() noexcept -> Thread {
    let tid = thr_t(u64(::pthread_self()));
    return Thread{ tid };
}


pub fn yield() noexcept -> void {
#ifdef USTD_OS_MACOS
    ::pthread_yield_np();
#else
    ::pthread_yield();
#endif    
}

}


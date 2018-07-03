#pragma once

#include "ustd/core.h"

namespace ustd::thread
{

enum class thr_t : u64 {
    Invalid = 0
};

using Error = os::Error;

template<class T>
using Result = ustd::Result<T, Error>;

struct Thread;
struct Builder;

template<class T>
class JoinHandle;

struct Thread
{
    thr_t _thr;

    constexpr Thread(thr_t thr) noexcept: _thr(thr)
    {}

    constexpr Thread(const Thread& other) noexcept: _thr(other._thr)
    {}

    constexpr Thread(Thread&& other)noexcept: _thr(other._thr) {
        other._thr = thr_t::Invalid;
    }

    __declspec(property(get = get_id)) u64 id;
    pub fn get_id() const     noexcept -> u64;

    __declspec(property(get = get_name, put = set_name)) FixedStr<256> name;
    pub fn get_name()         const noexcept -> FixedStr<256>;
    pub fn set_name(str name)       noexcept -> void;

    pub fn is_valid() const noexcept -> bool;
    pub fn detach()         noexcept -> void;
    pub fn join()           noexcept -> Result<void>;

};

pub fn trait_sfmt(Formatter& fmt, const Thread& thr) noexcept -> void;


template<class T>
class JoinHandle;

template<>
class JoinHandle<void>
{
public:
    Thread          _thr;
    FnBox<void()>   _fun;

    JoinHandle(Thread thr, FnBox<void()>&& fun) noexcept
        : _thr(thr), _fun(as_mov(fun))
    {}

    ~JoinHandle() noexcept {
        if (!_thr.is_valid()) {
            return;
        }
        _thr.join();
    }

    JoinHandle(JoinHandle&& other) noexcept
        : _thr(as_mov(other._thr))
        , _fun(as_mov(other._fun))
    {}

    fn join() -> Result<void> {
        if (!_thr.is_valid()) {
            return Result<void>::Err(Error::InvalidData);
        }

        let res = _thr.join();
        return res;
    }
};

struct Builder
{
    usize   _stack  = 0;
    str     _name   = "";

    Builder() noexcept
    {}

    fn set_stack(usize stack) -> Builder& {
        _stack = stack;
        return *this;
    }

    fn set_name(str name) noexcept -> Builder& {
        _name = name;
        return *this;
    }

    template<class F, class R = decltype(declval<val_t<F>>()()) >
    fn spawn(F&& f) const noexcept -> JoinHandle<R> {
        if constexpr($is_same<R, void>) {
            mut fun = FnBox<R()>::from_fn(as_fwd<F>(f));
            mut thr = this->spawn_fn(fun._res);
            return JoinHandle<R>(thr, as_mov(fun));
        }
    }

private:
    pub fn spawn_fn(FnBox<void()>::res_t* addr) const noexcept -> Thread;
};

pub fn current()    noexcept -> Thread;
pub fn yield()      noexcept -> void;

template<typename F, class R = decltype(declval<val_t<F>>()())>
fn spawn(F&& func, str name = "") noexcept -> JoinHandle<R> {
    let thr_builder = Builder().set_stack(0).set_name(name);
    return thr_builder.spawn(as_fwd<F>(func));
}

}


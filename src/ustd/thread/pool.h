#pragma once

#include "ustd/core.h"
#include "ustd/time.h"
#include "ustd/thread/thread.h"

namespace ustd::thread
{

class Pool
{
public:
    using func_t = Fn<void()>;
    using time_t = time::Instant;

    struct AsyncFn
    {
        time_t _time;
        func_t _fun;

        AsyncFn(time_t time, func_t&& fun)
            : _time(time)
            , _fun(as_mov(fun))
        {}

        fn operator<(const AsyncFn& other) const noexcept -> bool {
            return _time < other._time;
        }

        fn operator<=(const AsyncFn& other) const noexcept -> bool {
            return _time <= other._time;
        }
    };

public:
    Heap<AsyncFn>   _fns;

    Pool(Pool&& other) noexcept
        : _fns(as_mov(other._fns))
    {}

    ~Pool() noexcept
    {}

    static fn with_capacity(u32 capacity) noexcept -> Pool {
        return Pool(capacity);
    }

    pub fn run()                    noexcept -> void;
    pub fn async_run(str thr_name)  noexcept -> JoinHandle<void>;

    template<class F>
    fn push(F&& f, time_t time) noexcept -> Option<Pool&> {
        return push_fn(func_t::from_fn(as_fwd<F>(f)), time);
    }

    template<class F>
    fn push(F&& f, time::Duration dur) noexcept -> Option<Pool&> {
        let time = time_t::now() + dur;
        return this->push(func_t::from_fn(as_fwd<F>(f)), time);
    }

    template<class F>
    fn push(F&& f) noexcept -> Option<Pool&> {
        return push(func_t::from_fn(as_fwd<F>(f)), time::Duration(0, 0));
    }

private:
    explicit Pool(u32 capacity) noexcept
        : _fns(List<AsyncFn>::with_capacity(capacity).as_heap())
    {}

    fn push_fn(func_t&& fun, time_t time) noexcept -> Option<Pool&> {
        mut res = _fns.push(AsyncFn{ time, as_mov(fun) });
        return res & Option<Pool&>::Some(*this);
    }
};

}

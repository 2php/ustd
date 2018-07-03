#include "config.inl"

namespace ustd::thread
{

pub fn Pool::run() noexcept -> void {
    while (true) {
        mut task_opt = _fns.pop();
        if (task_opt.is_none()) {
            break;
        }

        mut& task = task_opt._val;
        thread::sleep_to(task._time);
        task._fun();
    }
}

pub fn Pool::async_run(str thr_name) noexcept -> JoinHandle<void> {
    let thr_info = thread::Builder().set_name(thr_name);

    mut thr_join = thr_info.spawn([&]() {
        this->run();
    });
    return as_mov(thr_join);
}

unittest(Pool) {
    mut pool = Pool::with_capacity(4096);

    volatile mut j = 0u;
    for(mut i = 0u; i < 100u; ++i) {
        pool.push([i, &j] {
            let k = sync::fetch_and_add(&j, 1u);
            test::assert_eq(i, k);
        }, time::Duration::from_millis(10*i));
    }

    mut t0 = pool.async_run("thread 0");
    mut t1 = pool.async_run("thread 1");
    t0.join();
    t1.join();
}

}

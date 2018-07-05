#include "config.inl"

namespace ustd::thread
{

pub fn Thread::is_valid() const noexcept -> bool {
    return _thr != thr_t::Invalid;
}

pub fn trait_sfmt(Formatter& fmt, const Thread& thr) noexcept -> void {
    let name = thr.name();
    let tid  = thr.id();
    fmt.write_fmt("{{name=`{}`, tid={}}}", name, tid);
}


unittest(Thread) {
    mut val = 0;

    mut t0 = thread::spawn([&]() {
        thread::sleep_ms(50);
        log::info("ustd::thread::test[Thread]: val = 1");
        val = 1;
    }, "thread 0");

    mut t1 = thread::spawn([&]() {
        thread::sleep_ms(100);
        log::info("ustd::thread::test[Thread]: val = 2");
        val = 2;
    }, "thread 1");

    assert_eq(val, 0);

    t0.join();
    assert_eq(val, 1);

    t1.join();
    assert_eq(val, 2);
}

}

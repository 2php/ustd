#include "config.inl"

#ifdef _WIN32
using namespace ustd;
using namespace ustd::thread;

extern "C" fn SleepEx(u32 milli_seconds, i32 alert_able) -> u32 ;

static fn nanosleep(const timespec* req, timespec* rem) noexcept -> i32 {
    let t0 = time::Instant::now();

    let ms = req->tv_sec * 1000 + (req->tv_nsec + 500000) / 1000000;

    if (ms <= 0) {
        return 0;
    }

    let eid = ::SleepEx(ulong(ms), 1);
    (void)eid;

    let t1 = time::Instant::now();
    let res = t1.duration_since(t0);

    rem->tv_sec  = time_t(res._secs);
    rem->tv_nsec = long(res._nanos);

    return 0;
}
#endif

namespace ustd::thread
{

pub fn sleep(time::Duration dur) noexcept -> time::Duration {
    let ts_dur  = timespec{ time_t(dur._secs) , long(dur._nanos) };
    mut ts_rem  = timespec{};
    let eid     = ::nanosleep(&ts_dur, &ts_rem); (void)eid;

    return { u64(ts_rem.tv_sec), u32(ts_rem.tv_nsec) };
}

pub fn sleep_ms(u32 ms) noexcept -> u32 {
    let dur = time::Duration::from_millis(ms);
    let rem = thread::sleep(dur);
    let res = rem.total_millis();
    return u32(res + 0.5);
}

pub fn sleep_to(time::Instant time_to)  noexcept->time::Duration {
    let time_now = time::Instant::now();

    if (time_to <= time_now) {
        return time::Duration::from_secs(0);
    }

    let time_dur = time_to - time_now;
    let time_res = thread::sleep(time_dur);
    return time_res;
}

}

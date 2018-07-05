#include "config.inl"

#ifdef USTD_OS_WINDOWS
extern "C" fn QueryPerformanceFrequency (u64* freq) -> i32;
extern "C" fn QueryPerformanceCounter   (u64 *cnt)  -> i32;

enum ClockId
{
    CLOCK_MONOTONIC
};

static fn clock_gettime(ClockId clk_id, struct timespec *res) -> i32 {
    // CLOCK_MONOTONIC
    if (clk_id == CLOCK_MONOTONIC) {
        static let freq = [] {
            u64 val;
            QueryPerformanceFrequency(&val);
            return val;
        }();

        let tcnt = [] {
            u64 val;
            QueryPerformanceCounter(&val);
            return val;
        }();

        res->tv_sec  = time_t(tcnt / freq);
        res->tv_nsec = long(((tcnt % freq) * 1000000000 + 500000000) / freq);

        return 0;
    }

    return -1;
}

#endif

#if defined(USTD_OS_LINUX) || defined(USTD_OS_MACOS)

static constexpr let TIME_UTC = CLOCK_REALTIME;

static fn timespec_get(struct timespec* time, int base) -> int {
    let res = ::clock_gettime(clockid_t(base), time);
    return res;
}

#endif

namespace ustd::time
{

pub fn Time::operator+(Time rhs) const noexcept->Time {
    // check overflow
    let cond = 1000000000u - _nanos > rhs._nanos;
    let res = cond
        ? Time{ _secs + rhs._secs + 0, _nanos + rhs._nanos }
        : Time{ _secs + rhs._secs + 1, _nanos + rhs._nanos - 1000000000u }
    ;

    return res;
}

pub fn Time::operator-(Time rhs) const noexcept -> Time {
    // check overflow
    let cond = _nanos > rhs._nanos;
    let res = cond
        ? Time{ _secs - rhs._secs + 0, _nanos - rhs._nanos }
        : Time{ _secs - rhs._secs - 1, _nanos - rhs._nanos + 1000000000u }
    ;

    return res;
}

pub fn Duration::to_str() const noexcept -> FixedStr<32> {
    let secs = total_secs();
    let sbuf = snformat<32>("{}s", secs);
    return sbuf;
}

pub fn trait_sfmt(Formatter& fmt, const Duration& dur) -> void {
    let secs = dur.total_secs();
    mut spec = fmt._style;

    switch (spec._type) {
    case 's':
        if (spec._prec == 0) {
            spec._prec = 6;
        }
        fmt(secs);
        fmt._outbuf.push('s');
        break;
    case 'm':
        if (spec._prec == 0) {
            spec._prec = 3;
        }
        fmt(secs*1e3);
        fmt._outbuf.push_slice(str("ms"));
        break;
    case 'n':
        if (spec._width == 0) {
            spec._width = 1;
        }
        fmt(secs*1e9);
        fmt._outbuf.push_slice(str("ns"));
        break;
    default:
        if (spec._prec == 0) {
            spec._prec = 6;
        }
        fmt(secs);
        break;
    }
}

static fn make_instat() noexcept -> Instant {
    mut res = timespec{};
    let ret = ::clock_gettime(CLOCK_MONOTONIC, &res);
    (void)ret;
    return Instant{ u64(res.tv_sec), u32(res.tv_nsec) };
}

static let g_instant = make_instat();

pub fn Instant::now() noexcept -> Instant {
    let time_set = g_instant;
    let time_cur = make_instat();
    let time_res = time_cur - time_set;
    return { time_res._secs, time_res._nanos };
}

pub fn SystemTime::now() noexcept -> SystemTime {
    mut current_ts = timespec{};

    ::timespec_get(&current_ts, TIME_UTC);
    return SystemTime{ u64(current_ts.tv_sec), u32(current_ts.tv_nsec) };
}

pub fn trait_sfmt(Formatter& fmt, const SystemTime& time) -> void {
    (void)fmt;
    (void)time;
    fmt(str("SystemTime(?)"));
}

}

                                                                                                             #pragma once

#include "ustd/core.h"

namespace ustd::time
{

struct Time
{
    u64 _secs;
    u32 _nanos;

    static fn from_secs(u64 secs) noexcept -> Time {
        return { secs, 0 };
    }

    static fn from_millis(u64 millis) noexcept -> Time {
        return { millis / 1000ull, u32(millis % 1000ull * 1000000ull) };
    }

    static fn from_nanos(u64 nanos) noexcept -> Time {
        return { nanos / 1000000000ull, u32(nanos % 1000000000ull) };
    }

    fn total_secs() const noexcept -> f64 {
        return f64(_secs) + f64(_nanos)*1e-9;
    }

    fn total_millis() const noexcept -> f64 {
        return f64(_secs)*1e3 + f64(_nanos)*1e-6;
    }

    fn total_nanos() const noexcept -> u64 {
        return _secs * u64(1e9) + _nanos;
    }

#pragma region operators
    pub fn operator+(Time rhs) const noexcept -> Time;
    pub fn operator-(Time rhs) const noexcept -> Time;

    fn operator==(const Time& rhs) const noexcept -> bool {
        return (_secs == rhs._secs) && (_nanos == rhs._nanos);
    }

    fn operator!=(const Time& rhs) const noexcept -> bool {
        return (_secs != rhs._secs) || (_nanos != rhs._nanos);
    }

    fn operator<(const Time& rhs) const noexcept -> bool {
        return (_secs < rhs._secs) || ((_secs == rhs._secs) && (_nanos < rhs._nanos));
    }

    fn operator>(const Time& rhs) const noexcept -> bool {
        return (_secs > rhs._secs) || ((_secs == rhs._secs) && (_nanos > rhs._nanos));
    }

    fn operator<=(const Time& rhs) const noexcept -> bool {
        return (_secs < rhs._secs) || ((_secs == rhs._secs) && (_nanos <= rhs._nanos));
    }

    fn operator>=(const Time& rhs) const noexcept -> bool {
        return (_secs > rhs._secs) || ((_secs == rhs._secs) && (_nanos >= rhs._nanos));
    }
#pragma endregion
};

struct Duration: Time
{
    constexpr Duration(u64 secs, u32 nanos) noexcept: Time{ secs, nanos }
    {}

    static fn from_secs(u64 secs) noexcept -> Duration {
        return Time::from_secs(secs);
    }

    static fn from_millis(u64 millis) noexcept -> Duration {
        return Time::from_millis(millis);
    }

    static fn from_nanos(u64 millis) noexcept -> Duration {
        return Time::from_nanos(millis);
    }

    pub fn to_str() const noexcept -> FixedStr<32>;

private:
    constexpr Duration(const Time& time) noexcept : Time(time)
    {}
};

struct Instant final: Time
{
    constexpr Instant(u64 secs, u32 nanos) noexcept: Time{ secs, nanos }
    {}

    static fn from_secs(u64 secs) noexcept -> Instant {
        return Time::from_secs(secs);
    }

    static fn from_millis(u64 millis) noexcept -> Instant {
        return Time::from_millis(millis);
    }

    static fn from_nanos(u64 millis) noexcept -> Instant {
        return Time::from_nanos(millis);
    }

    static pub fn now() noexcept->Instant;

    fn elapsed() const noexcept -> Duration {
        let time_now = Instant::now();
        return time_now.duration_since(*this);
    }

    fn duration_since(const Instant& earlier) const noexcept -> Duration {
        let time_dur = Time(*this) - Time(earlier);
        return { time_dur._secs, time_dur._nanos };
    }

    fn operator-(const Instant& earlier) const noexcept -> Duration {
        return duration_since(earlier);
    }

    fn operator+(const Duration& dur) const noexcept -> Instant {
        let res = Time(*this) + Time(dur);
        return { res._secs, res._nanos };
    }

    fn operator-(const Duration& dur) const noexcept -> Instant {
        let res = Time(*this) - Time(dur);
        return { res._secs, res._nanos };
    }

    fn operator==(const Instant& other) const noexcept -> bool {
        return Time::operator==(other);
    }

    fn operator!=(const Instant& other) const noexcept -> bool {
        return Time::operator!=(other);
    }

    fn operator<(const Instant& other) const noexcept -> bool {
        return Time::operator<(other);
    }

    fn operator>(const Instant& other) const noexcept -> bool {
        return Time::operator>(other);
    }

    fn operator<=(const Instant& other) const noexcept -> bool {
        return Time::operator<=(other);
    }

    fn operator>=(const Instant& other) const noexcept -> bool {
        return Time::operator>=(other);
    }

private:
    constexpr Instant(const Time& time) noexcept : Time(time)
    {}
};

struct SystemTime final: Time
{
    constexpr SystemTime(u64 secs, u32 nanos) noexcept : Time{ secs, nanos }
    {}

    static fn from_nanos(u64 nanos) noexcept -> SystemTime {
        let t = Time::from_nanos(nanos);
        return SystemTime(t._secs, t._nanos);
    }

    static pub fn now() noexcept -> SystemTime;

    fn duration_since(const SystemTime& earlier) const noexcept -> Duration {
        let time_dur = *this - earlier;
        return { time_dur._secs, time_dur._nanos };
    }

    fn elapsed() const noexcept -> Duration {
        let time_now = SystemTime::now();
        return time_now.duration_since(*this);
    }

    fn operator-(SystemTime since) const noexcept -> Duration {
        let res = Time(*this) - Time(since);
        return { res._secs, res._nanos };
    }

    fn operator+(const Duration& dur) const noexcept -> SystemTime {
        let res = Time(*this) + Time(dur);
        return { res._secs, res._nanos };
    }

    fn operator-(const Duration& dur) const noexcept -> SystemTime {
        let res = Time(*this) - Time(dur);
        return { res._secs, res._nanos };
    }

    fn operator==(const SystemTime& rhs) const noexcept -> bool {
        return Time::operator==(rhs);
    }

    fn operator!=(const SystemTime& rhs) const noexcept -> bool {
        return Time::operator!=(rhs);
    }

    fn operator<(const SystemTime& rhs) const noexcept -> bool {
        return Time::operator<(rhs);
    }

    fn operator>(const SystemTime& rhs) const noexcept -> bool {
        return Time::operator>(rhs);
    }

    fn operator<=(const SystemTime& rhs) const noexcept -> bool {
        return Time::operator<=(rhs);
    }

    fn operator>=(const SystemTime& rhs) const noexcept -> bool {
        return Time::operator>=(rhs);
    }
};

pub fn trait_sfmt(Formatter& fmt, const SystemTime& time) -> void;

}

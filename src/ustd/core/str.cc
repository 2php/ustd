#include "config.inl"

namespace ustd
{

pub fn cstr(const char* s)  noexcept->str {
    if (s == nullptr) {
        return str();
    }

    let len = __builtin_strlen(s);
    return str(s, len);
}

pub fn hash(str s) noexcept -> u32 {
    mut seed = 131u;
    mut res  = 0u;

    for (mut i = 0u; i < s._size; ++i) {
        res = res * seed + u32(s._data[i]);
    }

    return res;
}


template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<long> {
    if (s.is_empty()) return Option<long>::None();

    mut end = static_cast<char*>(nullptr);
    let num = ::strtol(s._data, &end, 0);
    if (end == nullptr) return Option<long>::None();
    if (rem != nullptr) *rem = str{ end, s._size - u32(end - s._data) };

    return Option<long>::Some(num);
}


template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<ulong> {
    if (s.is_empty()) return Option<ulong>::None();

    mut end = static_cast<char*>(nullptr);
    let num = ::strtoul(s._data, &end, 0);
    if (end == nullptr) return Option<ulong>::None();
    if (rem != nullptr) *rem = str{ end, s._size - u32(end - s._data) };

    return Option<ulong>::Some(num);
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<llong> {
    if (s.is_empty()) return Option<llong>::None();

    mut end = static_cast<char*>(nullptr);
    let num = ::strtoll(s._data, &end, 0);
    if (end == nullptr) return Option<llong>::None();
    if (rem != nullptr) *rem = str{ end, s._size - u32(end - s._data) };

    return Option<llong>::Some(num);
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<ullong> {
    if (s.is_empty()) return Option<ullong>::None();

    mut end = static_cast<char*>(nullptr);
    let num = ::strtoull(s._data, &end, 0);

    if (end == nullptr) return Option<ullong>::None();
    if (rem != nullptr) *rem = str{ end, s._size - u32(end - s._data) };
    return Option<ullong>::Some(num);
}


template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<byte> {
    return str_parse_num<long>(s, rem).map([](long x) {return byte(x); });
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<ubyte> {
    return str_parse_num<ulong>(s, rem).map([](ulong x) { return ubyte(x); });
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<short> {
    return str_parse_num<long>(s, rem).map([](long x) { return short(x); });
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<ushort> {
    return str_parse_num<ulong>(s, rem).map([](ulong x) { return ushort(x); });
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<int> {
    return str_parse_num<long>(s, rem).map([](long x) { return int(x); });
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<uint> {
    return str_parse_num<ulong>(s, rem).map([](ulong x) {return uint(x); });
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<f32> {
    if (s.is_empty()) return Option<f32>::None();

    mut end = static_cast<char*>(nullptr);
    let num = ::strtof(s._data, &end);

    if (end == nullptr) return Option<f32>::None();
    if (rem != nullptr) *rem = str{ end, s._size - u32(end - s._data) };

    return Option<float>::Some(num);
}

template<>
pub fn str_parse_num(str s, str* rem) noexcept -> Option<f64> {
    if (s.is_empty()) return Option<f64>::None();

    mut end = static_cast<char*>(nullptr);
    let num = ::strtod(s._data, &end);

    if (end == nullptr) return Option<f64>::None();
    if (rem != nullptr) *rem = str{ end, s._size - u32(end - s._data) };

    return Option<double>::Some(num);
}


pub fn println(const str& msg) noexcept -> void {
    io::stdout().writeln(msg);
}

}

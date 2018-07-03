#pragma once

#include "ustd/core/list.h"

namespace ustd
{

using str       = Slice<const char>;
using StrView   = Slice<char>;
using String    = List<char>;

template<u32 N>
struct FixedStr : public StrView
{
    using base = StrView;

    char _buff[N];

    constexpr FixedStr() noexcept : base(_buff, 0u, u32(N))
    {}

    FixedStr(str s) noexcept : FixedStr() {
        base::push_slice(s);
    }

    template<u32 N>
    FixedStr(const char(&s)[N]) noexcept : FixedStr(str(s)) {
    }

    FixedStr(const FixedStr& other) noexcept: FixedStr(str(other)) {
    }
};

template<u32 N>
struct FixedCStr : public FixedStr<N - 1>
{
    using base = FixedStr<N - 1>;

    using base::_data;
    using base::_size;

    constexpr FixedCStr() noexcept : base()
    {}

    FixedCStr(str s) noexcept : base(s) {
        _data[_size] = '\0';                       
    }

    template<u32 N>
    FixedCStr(const char(&s)[N]): FixedCStr(str(s)) {
    }

    FixedCStr(const FixedCStr& other) noexcept: FixedCStr(str(other)) {
    }

    operator const char*() const noexcept {
        return _data;
    }
};

pub fn hash(str s)          noexcept -> u32;
pub fn cstr(const char* s)  noexcept -> str;

#pragma region parse_num
template<class T>
fn str_parse_num(str s, str* rem=nullptr) noexcept -> Option<T>;

template<> fn str_parse_num(str s, str* rem) noexcept->Option<byte  >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<ubyte >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<short >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<ushort>;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<int   >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<uint  >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<long  >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<ulong >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<llong >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<ullong>;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<float >;
template<> fn str_parse_num(str s, str* rem) noexcept->Option<double>;
#pragma endregion

#pragma region parse_enum
template<typename T>
fn str_parse_enum(str s) noexcept -> Option<T> {
    constexpr static let max_cnt = 100u;

    if (s.is_empty()) return Option<T>::None();

    for (u32 i = 0; i < max_cnt; ++i) {
        let name = to_str(T(i));
        if (name == str()) break;

        if (s==name) {
            return Option<T>::Some(T(i));
        }
    }

    return Option<T>::None();
}

#pragma endregion

template<typename T>
fn str_parse(str s) noexcept -> Option<T> {
    if (s.is_empty()) return Option<T>::None();

    if constexpr(trait<T>::$num) {
        return str_parse_num<T>(s, nullptr);
    }
    if constexpr(trait<T>::$enum) {
        return str_parse_enum<T>(s);
    }
}

}

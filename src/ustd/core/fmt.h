#pragma once

#include "ustd/core/string.h"
#include "ustd/core/panic.h"
#include "ustd/core/vec.h"
#include "ustd/core/tuple.h"

namespace ustd
{

struct FmtStyle
{
    i8  _fill       = ' ';  // [a-Z]?
    i8  _align      = '>';  // [<>^]?
    i8  _sign       = 0;    // [+- ]?
    u8  _width      = 0;    // [0-9]*

    u8  _prec       = 0;    // [0-9|*]*
    u8  _type       = 0;    // [a-Z]

    u8  _spec_len   = 0;    // [0~11]
    i8  _spec[9];           // [...]

    __declspec(property(get = get_spec)) str spec;
    fn get_spec() const noexcept -> str {
        return str(_spec, _spec_len);
    }

    static pub fn from_str(str s) noexcept -> FmtStyle;
};

template<typename ...T>
fn sformat(StrView& outbuf, str fmt, const T& ...args) noexcept->StrView;

class Formatter
{
public:
    FmtStyle    _style;
    StrView&    _outbuf;
    u32         _indent;

    Formatter(FmtStyle style, StrView& outbuf)
        : _style(style), _outbuf(outbuf), _indent(0)
    {}

    fn push_str(str s) -> Formatter& {
        _outbuf.push_slice(s);
        return *this;
    }

    fn push_char(char c) -> Formatter& {
        _outbuf.push(c);
        return *this;
    }

    fn indent(i32 val = 0) -> void {
        static let indent_spaces = 4;

        _indent += val;
        _outbuf.pushn(_indent * indent_spaces, ' ');
    }

    template<class T>
    fn operator()(const T& val) noexcept {
        do_fmt(val, ver_t<4>{});
    }

    template<typename ...U>
    fn write_fmt(str fmt, const U& ...u) -> void {
        sformat(_outbuf, fmt, u...);
    }

private:
#pragma region level 4
    template<class T, class=when<trait<T>::$num || $is_same<T, bool> > >
    fn do_fmt(const T& val, ver_t<4>) -> void {
        do_fmt_val(val);
    }

    template<class T>
    fn do_fmt(const T* val, ver_t<4>) -> void {
        do_fmt_val(val);
    }

    template<u32 N>
    fn do_fmt(const char(&s)[N], ver_t<4>) -> void {
        let s = str(s);
        do_fmt_val(s);
    }

    fn do_fmt(const char* val, ver_t<4>) -> void {
        let s = cstr(val);
        do_fmt_val(s);
    }
#pragma endregion

#pragma region level 3
    template<class T>
    fn do_fmt(const Option<T>& opt, ver_t<3>) -> void {
        mut& self = *this;
        opt.is_none() ? self(str("null")) : self(opt._val);
    }

    template<class ...T>
    fn do_fmt(const Tuple<T...>& val, ver_t<3>) noexcept -> void {
        do_fmt_tuple(val);
    }

    template<class T>
    fn do_fmt(const Slice<T>& val, ver_t<3>) noexcept -> void {
        do_fmt_arr(val);
    }

    fn do_fmt(const Slice<char>& val, ver_t<3>) noexcept -> void  {
        do_fmt_val(str(val));
    }

    fn do_fmt(const Slice<const char>& val, ver_t<3>) noexcept -> void {
        do_fmt_val(val);
    }

    template<class T, u32 N>
    fn do_fmt(const vec<T, N>& val, ver_t<3>) -> void {
        let list = Slice<const T>(val._arr, N);
        do_fmt_arr(list);
    }

    template<class T, class = when<trait<T>::$enum> >
    fn do_fmt(const T& t, ver_t<3>) noexcept -> void {
        let s = to_str(t);
        do_fmt_val(s);
    }
#pragma endregion

#pragma region level 2
    template<class T, class = decltype(trait_sfmt(*declptr<Formatter>(), *declptr<T>())) >
    fn do_fmt(const T& val, ver_t<2>) -> void {
        trait_sfmt(*this, val);
    }
#pragma endregion

#pragma region level 1
    template<class T, class = decltype(T::$property_idx) >
    fn do_fmt(const T& obj, ver_t<1>) -> void {
        do_fmt_obj(obj);
    }
#pragma endregion

#pragma region level 0
    template<class T, class = decltype(&T::to_str) >
    fn do_fmt(const T& obj, ver_t<0>) -> void {
        let s = obj.to_str();
        do_fmt_val(s);
    }
#pragma endregion

private:
#pragma region  impl
    pub fn do_fmt_val(i8          val) noexcept -> void;
    pub fn do_fmt_val(u8          val) noexcept -> void;
    pub fn do_fmt_val(i16         val) noexcept -> void;
    pub fn do_fmt_val(u16         val) noexcept -> void;
    pub fn do_fmt_val(i32         val) noexcept -> void;
    pub fn do_fmt_val(u32         val) noexcept -> void;
    pub fn do_fmt_val(i64         val) noexcept -> void;
    pub fn do_fmt_val(u64         val) noexcept -> void;
    pub fn do_fmt_val(f32         val) noexcept -> void;
    pub fn do_fmt_val(f64         val) noexcept -> void;
    pub fn do_fmt_val(bool        val) noexcept -> void;
    pub fn do_fmt_val(str         val) noexcept -> void;
    pub fn do_fmt_val(const void* val) noexcept -> void;
    pub fn do_fmt_val(const char* val) noexcept -> void;

    template<class T>
    fn do_fmt_tuple(const T& val) noexcept -> void {
        constexpr static let N = T::$size;
        mut& self = *this;

        push_char('(');
        if constexpr(N > 0) { self(val.$0); }
        if constexpr(N > 1) { push_str(", "); self(val.$1); }
        if constexpr(N > 2) { push_str(", "); self(val.$2); }
        if constexpr(N > 3) { push_str(", "); self(val.$3); }
        if constexpr(N > 4) { push_str(", "); self(val.$4); }
        if constexpr(N > 5) { push_str(", "); self(val.$5); }
        if constexpr(N > 6) { push_str(", "); self(val.$6); }
        if constexpr(N > 7) { push_str(", "); self(val.$7); }
        if constexpr(N > 8) { push_str(", "); self(val.$8); }
        if constexpr(N > 9) { push_str(", "); self(val.$9); }
        push_char(')');
    }

    template<class T>
    fn do_fmt_arr(const T& arr) noexcept -> void {
        let max_len     = 8;
        let use_indent  = arr.len > max_len;

        push_str("[");

        if (use_indent) {
            push_str("\n");
            indent(+1);
        }

        for (mut i = 0u; i < arr.len; ++i) {
            do_fmt_arr_element(i, arr, use_indent);
        }
        if (use_indent) indent(-1);
        push_str("]");
    }

    template<class T>
    fn do_fmt_obj(const T& obj) -> void {
        push_str("{\n");
        indent(+1);
        if constexpr(T::$property_cnt >  0) { do_fmt_obj_element< 0>(obj); }
        if constexpr(T::$property_cnt >  1) { do_fmt_obj_element< 1>(obj); }
        if constexpr(T::$property_cnt >  2) { do_fmt_obj_element< 2>(obj); }
        if constexpr(T::$property_cnt >  3) { do_fmt_obj_element< 3>(obj); }
        if constexpr(T::$property_cnt >  4) { do_fmt_obj_element< 4>(obj); }
        if constexpr(T::$property_cnt >  5) { do_fmt_obj_element< 5>(obj); }
        if constexpr(T::$property_cnt >  6) { do_fmt_obj_element< 6>(obj); }
        if constexpr(T::$property_cnt >  7) { do_fmt_obj_element< 7>(obj); }
        if constexpr(T::$property_cnt >  8) { do_fmt_obj_element< 8>(obj); }
        if constexpr(T::$property_cnt >  9) { do_fmt_obj_element< 9>(obj); }
        if constexpr(T::$property_cnt > 10) { do_fmt_obj_element<10>(obj); }
        if constexpr(T::$property_cnt > 11) { do_fmt_obj_element<11>(obj); }
        if constexpr(T::$property_cnt > 12) { do_fmt_obj_element<12>(obj); }
        if constexpr(T::$property_cnt > 13) { do_fmt_obj_element<13>(obj); }
        if constexpr(T::$property_cnt > 14) { do_fmt_obj_element<14>(obj); }
        if constexpr(T::$property_cnt > 15) { do_fmt_obj_element<15>(obj); }
        if constexpr(T::$property_cnt > 16) { do_fmt_obj_element<16>(obj); }
        if constexpr(T::$property_cnt > 17) { do_fmt_obj_element<17>(obj); }
        if constexpr(T::$property_cnt > 18) { do_fmt_obj_element<18>(obj); }
        if constexpr(T::$property_cnt > 19) { do_fmt_obj_element<19>(obj); }
        if constexpr(T::$property_cnt > 20) { do_fmt_obj_element<20>(obj); }
        if constexpr(T::$property_cnt > 21) { do_fmt_obj_element<21>(obj); }
        if constexpr(T::$property_cnt > 22) { do_fmt_obj_element<22>(obj); }
        if constexpr(T::$property_cnt > 23) { do_fmt_obj_element<23>(obj); }
        if constexpr(T::$property_cnt > 24) { do_fmt_obj_element<24>(obj); }
        if constexpr(T::$property_cnt > 25) { do_fmt_obj_element<25>(obj); }
        if constexpr(T::$property_cnt > 26) { do_fmt_obj_element<26>(obj); }
        if constexpr(T::$property_cnt > 27) { do_fmt_obj_element<27>(obj); }
        if constexpr(T::$property_cnt > 28) { do_fmt_obj_element<28>(obj); }
        if constexpr(T::$property_cnt > 29) { do_fmt_obj_element<29>(obj); }
        indent(-1);
        push_str("}");
    }

    template<u32 I, class T>
    fn do_fmt_obj_element(const T& obj) -> void {
        let kv = obj.get_property(immut_t<u32, I>{});

        if (I!=0) indent();

        push_str(kv.key);
        push_str(": ");
        (*this)(kv.val);

        let is_last = (I + 1 == T::$property_cnt);
        push_str(is_last ? str("\n") : str(",\n"));
    }

    template<class T>
    fn do_fmt_arr_element(u32 idx, const T& arr, bool use_indent) -> void {
        using E         = val_t<decltype(arr[0])>;
        let max_len     = 8;
        let is_last     = idx + 1 == arr.len;

        if (use_indent && idx != 0) {
            if ( (!trait<E>::$num) || (idx % max_len == 0)) {
                indent(+0);
            }
        }

        (*this)(arr[idx]);

        if (!use_indent) {
            push_str(is_last ? str("") : str(", "));
        }
        else {
            if (trait<E>::$num && (idx + 1) % max_len != 0) {
                push_str(is_last ? str("\n") : str(", "));
            }
            else {
                push_str(is_last ? str("\n") : str(",\n"));
            }
        }
    }

#pragma endregion
};

#pragma region FmtFill
struct FmtFill
{
    char    sign;
    u32     size;
};
pub fn trait_sfmt(Formatter& fmt, FmtFill val) noexcept -> void;

#pragma endregion

#pragma region detail

pub fn _sformat_parse(StrView& outbuf, str& fmtstr, str *spec, i32 *idx) noexcept -> bool;

template<class T, typename ...U>
fn _sformat_index(StrView& outbuf, str fmtstr, i32 idx, const T& t, const U& ...args) noexcept -> void {
    if (idx == 0) {
        let style = FmtStyle::from_str(fmtstr);
        mut fmter = Formatter(style, outbuf);
        fmter(t);
        return;
    }

    if constexpr(sizeof...(U) != 0) {
        _sformat_index(outbuf, fmtstr, idx - 1, args...);
    }
}
#pragma endregion

template<typename ...T>
fn sformat(StrView& outbuf, str fmt, const T& ...args) noexcept -> StrView {
    i32 arg_idx = -1;
    str fmt_str = {};

    if constexpr(sizeof...(args) == 0) {
        outbuf.push_slice(fmt);
    }
    else {
        while (_sformat_parse(outbuf, fmt, &fmt_str, &arg_idx)) {
            _sformat_index(outbuf, fmt_str, arg_idx, args...);
        }
    }

    return outbuf;
}

template<u32 N, typename ...T>
fn snformat(str fmt, const T& ...args) noexcept -> FixedStr<N> {
    FixedStr<N> outbuf;
    ustd::sformat(outbuf, fmt, args...);
    return as_mov(outbuf);
}

template<class T, class ...U>
fn println(const str& fmt, const T& t, const U& ...u) noexcept -> void {
    let outstr = snformat<4*1024>(fmt, t, u...);
    ustd::println(outstr);
}

template<typename T, class ...U>
fn panic(str fmt, const T& t, const U& ...u) -> void {
    let outstr = snformat<4*1024>(fmt, t, u...);
    ustd::panic(outstr);
}

}

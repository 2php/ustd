#include "config.inl"

#ifdef __INTELLISENSE__
fn clz  (u32 val) -> u32;
fn clzll(u64 val) -> u32;
#endif

namespace ustd
{

#pragma region fmt: str
static void _sfmt_str(const Formatter& fmt, str s) noexcept {
    let& style   = fmt._style;
    mut& outbuf = fmt._outbuf;

    let num_width = u32(style._width);
    let style_s   = fmt._style._type == 's';
    let num_chars = s._size + (style_s ? 2u : 0u);

    mut p = outbuf._data + outbuf._size;
    if (style._width <= num_chars) {
        outbuf._size += num_chars;

        if (style_s) *p++ = '"';
        for (mut i = 0u; i < s._size; ++i) {
            *p++ = s[i];
        }
        if (style_s) *p++ = '"';
    }
    else {
        outbuf._size += style._width;

        let num_fills = num_width - num_chars;
        if (style._align == '>') {
            for (mut i = 0u; i < num_fills; ++i) {
                *p++ = style._fill;
            }
            if (style_s) *p++ = '"';
            for (mut i = 0u; i < s._size; ++i) {
                *p++ = s[i];
            }
            if (style_s) *p++ = '"';
        }
        else if (style._align == '^') {
            for (mut i = 0u; i < num_fills / 2; ++i) {
                *p++ = style._fill;
            }

            if (style_s) *p++ = '"';
            for (mut i = 0u; i < s._size; ++i) {
                *p++ = s[i];
            }
            if (style_s) *p++ = '"';

            for (mut i = 0u; i < (num_fills + 1) / 2; ++i) {
                *p++ = style._fill;
            }
        }
        else {  // '<'
            if (style_s) *p++ = '"';
            for (mut i = 0u; i < s._size; ++i) {
                *p++ = s[i];
            }
            if (style_s) *p++ = '"';

            for (mut i = 0u; i < num_fills; ++i) {
                *p++ = style._fill;
            }
        }
    }
}
#pragma endregion

#pragma region fmt: int
inline u32 _count_digits(u32 n) noexcept {
    static const u32 powers[] = {
        0,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000
    };

    let t = (32 - ustd_builtin(clz)(n | 1)) * 1233 >> 12;
    return u32(t) - (n < powers[t]) + 1;
}

inline fn _count_digits(u64 n) noexcept -> u32 {
    static const u64 powers[] = {
        0,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000,
        100000000000,
        1000000000000,
        10000000000000,
        100000000000000,
        1000000000000000,
        10000000000000000,
        100000000000000000,
        1000000000000000000,
        10000000000000000000ull
    };

    let t = (64 - ustd_builtin(clzll)(n | 1)) * 1233 >> 12;
    return u32(t) - (n < powers[t]) + 1;
}

inline fn _count_digits(u16 n) noexcept -> u32 {
    return _count_digits(u32(n));
}

inline fn _count_digits(u8 n) -> u32 {
    return _count_digits(u32(n));
}

static fn _sfmt_int_box(const Formatter& fmt, const char prefix[], u32 num_prefix, u32 num_digits) noexcept -> char* {
    let style = fmt._style;
    let s = fmt._outbuf._data + fmt._outbuf._size;
    
    // check: res.width is small
    if (fmt._style._width < num_digits + num_prefix) {
        goto END;
    }

    fmt._outbuf._size += u32(fmt._style._width);
    if (style._align == '>') {
        // ....~~~~####....
        //     ^   ^   ^
        //     s   q   p
        let q = s + style._width - num_prefix - num_digits;
        let p = s + style._width;

        for (auto f = s; f < q; ++f) {
            *f = fmt._style._fill;
        }

        for (u32 i = 0; i < num_prefix; ++i) {
            q[i] = prefix[i];
        }
        return p;
    }
    else if (style._align == '^') {
        // ....~~~####~~~....
        //     ^  ^   ^  ^
        //     s  q   p  e
        let e = s + style._width;
        let q = s + (style._width - num_prefix - num_digits) / 2;
        let p = q + num_prefix + num_digits;

        for (u32 i = 0; i < num_prefix; ++i) {
            q[i] = prefix[i];
        }

        for (auto f = s; f < q; ++f) {
            *f = style._fill;
        }

        for (auto f = p; f < e; ++f) {
            *f = style._fill;
        }
        return p;
    }
    else { // '<'
        // ....####~~~~....
        //     ^   ^   ^
        //     s   p   e
        let e = s + style._width;
        for (u32 i = 0; i < num_prefix; ++i) {
            s[i] = prefix[i];
        }
        let p = s + num_prefix + num_digits;

        for (mut f = p; f < e; ++f) {
            *f = style._fill;
        }
        return p;
    }

END:
    fmt._outbuf._size += num_prefix + num_digits;
    let p = s + num_prefix + num_digits;

    for (u32 i = 0; i < num_prefix; ++i) {
        s[i] = prefix[i];
    }
    return p;
}

static fn _sfmt_int_body(char* p, u64 value) -> void {
    static const char digits[] =
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";

    while (value >= 100) {
        let index = u32((value % 100) * 2);
        value /= 100;
        *--p = digits[index + 1];
        *--p = digits[index];
    }

    if (value < 10) {
        *--p = char('0' + value);
        return;
    }

    let index = u32(value * 2);
    *--p = digits[index + 1];
    *--p = digits[index];
}

template<typename T>
static fn _sfmt_int(const Formatter& fmt, T val) noexcept -> void {
    let style      = fmt._style;
    let abs_val = to_uint(val > 0 ? T(val) : T(0 - val));

    char prefix[4];
    mut num_prefix = 0u;

    if (val < 0) {
        prefix[0] = '-';
        ++num_prefix;
    }
    else if (style._sign == '+') {
        prefix[0] = '+';
        ++num_prefix;
    }
    else if (style._sign == ' ') {
        prefix[0] = ' ';
        ++num_prefix;
    }

    switch (style._type) {
    case 'x': case 'X': {
        mut num_digits = 0u;
        {
            mut n = abs_val;
            do {
                ++num_digits;
            } while ((n >>= 4) != 0);
        }

        let digits = style._type == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";

        mut p = _sfmt_int_box(fmt, prefix, u32(num_prefix), num_digits);
        mut n = abs_val;
        do {
            *--p = digits[n & 0xf];
        } while ((n >>= 4) != 0);
        break;
    }
    case 'o': case 'O': {
        prefix[num_prefix++] = '0';

        mut num_digits = 0u;
        {
            mut n = abs_val;
            do {

                ++num_digits;
            } while ((n >>= 3) != 0);
        }

        mut p = _sfmt_int_box(fmt, prefix, num_prefix, num_digits);
        mut n = abs_val;
        do {
            *--p = char('0' + (n & 7));
        } while ((n >>= 3) != 0);
        break;
    }
    case 'b': case 'B': {
        prefix[num_prefix++] = '0';
        prefix[num_prefix++] = style._type;

        mut num_digits = 0u;
        {
            mut n = abs_val;
            do {
                ++num_digits;
            } while ((n >>= 1) != 0);
        }

        mut p = _sfmt_int_box(fmt, prefix, num_prefix, num_digits);
        mut n = abs_val;
        do {
            *--p = char('0' + (n & 1));
        } while ((n >>= 1) != 0);
        break;
    }
    case 'c': {
        mut p =_sfmt_int_box(fmt, prefix, num_prefix, 1);
        *--p = char(val);
        break;
    }
    default: {
        let num_digits = _count_digits(abs_val);
        let p = _sfmt_int_box(fmt, prefix, num_prefix, num_digits);
        _sfmt_int_body(p, abs_val);
        break;
    }
    }
}

#pragma endregion

#pragma region fmt: float
static fn _sfmt_float(const Formatter& fmt, f64 value) noexcept -> void {
    let style = fmt._style;

    if (isinf(value)) {
        _sfmt_str(fmt, "inf");
        return;
    }

    if (isnan(value)) {
        _sfmt_str(fmt, "nan");
        return;
    }

    let abs_value = f64(value >= 0 ? value : -value);

    char tmp_buf[256];

    let p = style._width == 0 ? (fmt._outbuf._data + fmt._outbuf._size) : tmp_buf;
    mut num_digits = 0u;

    if (value < 0) {
        p[num_digits++] = '-';
    }
    else if (style._sign == '+') {
        p[num_digits++] = '+';
    }
    else if (style._sign == ' ') {
        p[num_digits++] = ' ';
    }

    if (style._prec == 0) { // default
        let fmt_cnt = style._type == 'e'
            ? snprintf(p + num_digits, sizeof(tmp_buf) - 2, "%e", abs_value) : style._type == 'g'
            ? snprintf(p + num_digits, sizeof(tmp_buf) - 2, "%g", abs_value)
            : snprintf(p + num_digits, sizeof(tmp_buf) - 2, "%f", abs_value);

        if (fmt_cnt > 0) num_digits += u32(fmt_cnt);
    }
    else {
        let fmt_cnt = style._type == 'e'
            ? snprintf(p + num_digits, sizeof(tmp_buf) - 2, "%.*e", style._prec, abs_value) : style._type == 'g'
            ? snprintf(p + num_digits, sizeof(tmp_buf) - 2, "%.*g", style._prec, abs_value)
            : snprintf(p + num_digits, sizeof(tmp_buf) - 2, "%.*f", style._prec, abs_value);

        if (fmt_cnt > 0) num_digits += u32(fmt_cnt);
    }

#ifdef ustd_snprintf
#   undef ustd_snprintf
#endif

    if (p == tmp_buf) {
        _sfmt_str(fmt, str(tmp_buf, u32(num_digits)));
    }
    else {
        fmt._outbuf._size += num_digits;
    }
}
#pragma endregion

pub fn Formatter::do_fmt_val(i8   val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(u8   val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(i16  val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(u16  val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(i32  val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(u32  val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(i64  val) noexcept -> void { _sfmt_int(*this, val); }
pub fn Formatter::do_fmt_val(u64  val) noexcept -> void { _sfmt_int(*this, val); }

pub fn Formatter::do_fmt_val(f32  val) noexcept -> void { _sfmt_float(*this, f64(val)); }
pub fn Formatter::do_fmt_val(f64  val) noexcept -> void { _sfmt_float(*this, f64(val)); }

pub fn Formatter::do_fmt_val(const void* ptr) noexcept -> void {
    if (_style._width == 0) {
        mut style = FmtStyle();
        style._fill  = '0';
        style._align = '>';
        style._type  = 'X';
        style._width = 16;

        mut new_fmt = Formatter(style, _outbuf);
        let new_val = u64(ptr);
        new_fmt(new_val);
    }
    else {
        _sfmt_int(*this, u64(ptr));
    }
}

pub fn Formatter::do_fmt_val(const char* val) noexcept -> void {
    let s = cstr(val);
    do_fmt_val(s);
}

pub fn Formatter::do_fmt_val(str val) noexcept -> void {
    _sfmt_str(*this, val);
}

pub fn Formatter::do_fmt_val(bool val) noexcept -> void {
    let s = val ? str("true") : str("false");
    do_fmt_val(s);
}

pub fn trait_sfmt(Formatter& sfmt, FmtFill val) noexcept -> void {
    sfmt._outbuf.pushn(val.size, val.sign);
}

pub fn _sformat_parse(StrView& outbuf, str& fmtstrs, str *fmtstr, i32* idx) noexcept -> bool {
    // ......{........}....
    //       ^        ^    ^
    //       p        q    e

    let s = fmtstrs._data;
    let e = s + fmtstrs._size;
    mut p = s;

    // find: p
    while (p < e) {
        let c = *p;
        if (c == '{') {
            if (p + 1 < e && *(p + 1) == '{') {
                outbuf.push('{');
                ++p;
            }
            else {
                break;
            }
        }
        else if (c == '}') {
            if (p + 1 < e && *(p + 1) == '}') {
                outbuf.push('}');
                ++p;
            }
        }
        else {
            outbuf.push(c);
        }
        ++p;
    }

    if (p == e) {
        return false;
    }

    // find: q
    mut q = p + 1;
    while (q < e) {
        let c = *q;
        if (c == '}') {
            break;
        }
        q++;
    }

    if (q == e) {
        return false;
    }

    // return
    fmtstrs = str{ q + 1, u32(e - q - 1) };
    *fmtstr = str{ p + 1, u32(q - p - 1) };
    *idx += 1;

    return true;
}

// [[fill]align][sign][width]['.'precision][type]
pub fn FmtStyle::from_str(str text) noexcept -> FmtStyle {
    mut res = FmtStyle{};
    res._fill = ' ';

    if (text._size == 0) {
        return res;
    }

    mut p = text._data;

    // [[fill]align]
    if (p[1] == '>' || p[1] == '<' || p[1] == '^') {
        res._fill = *p++;
        res._align = *p++;
    }
    else if (*p == '>' || *p == '<' || *p == '^') {
        res._fill = ' ';
        res._align = *p++;
    }

    // [sign]
    if (*p == '+' || *p == '-' || *p == ' ') {
        res._sign = *p++;
    }

    // [width]
    res._width = 0;
    while ('0' <= *p && *p <= '9') {
        res._width = u8(res._width * 10 + ((*p++) - '0'));
    }

    // [prec]
    if (*p == '.') {
        ++p;

        res._prec = 0;
        while ('0' <= p[0] && p[0] <= '9') {
            res._prec = u8(res._prec * 10 + ((*p++) - '0'));
        }
    }

    // [type]
    if (*p != '}' && *p != ':') {
        res._type = *p++;
    }

    // [prec]
    if (*p == ':') {
        ++p;

        mut& idx = res._spec_len;
        mut& dst = res._spec;
        for (idx = 0; idx < sizeof(_spec) - 1 && p[idx] != '}'; ++idx) {
            dst[idx] = p[idx];
        }
    }

    return res;
}

#pragma region unittest
unittest(fmt_int)
{
    let val = -12345;
    test::assert_eq(snformat<32>("{}",   val), str("-12345"));
    test::assert_eq(snformat<32>("{2}",  val), str("-12345"));
    test::assert_eq(snformat<32>("{8}",  val), str("  -12345"));
    test::assert_eq(snformat<32>("{<8}", val), str("-12345  "));
    test::assert_eq(snformat<32>("{>8}", val), str("  -12345"));
    test::assert_eq(snformat<32>("{^8}", val), str(" -12345 "));
}

unittest(fmt_float)
{
    let val = -12.34;
    test::assert_eq(snformat<32>("{}",    val), str("-12.340000"));
    test::assert_eq(snformat<32>("{2.2}", val), str("-12.34"));
    test::assert_eq(snformat<32>("{12}",  val), str("  -12.340000"));
    test::assert_eq(snformat<32>("{<12}", val), str("-12.340000  "));
    test::assert_eq(snformat<32>("{>12}", val), str("  -12.340000"));
    test::assert_eq(snformat<32>("{^12}", val), str(" -12.340000 "));
}

struct Product {
    ustd_property_begin;
    typedef FixedStr<32>    ustd_property(name) = "petct";
    typedef u32             ustd_property(year) = 2018;
    ustd_property_end;
};

unittest(fmt_object) {
    Product petct;
    log::info("product = {s}", petct);
}
#pragma endregion

}

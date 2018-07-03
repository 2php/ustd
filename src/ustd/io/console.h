#pragma once

#include "ustd/core.h"
#include "ustd/fs.h"
#include "ustd/sync/mutex.h"

namespace ustd::io
{

struct CSI
{
    enum Cmd {
        CUU = 'A',  /// cursor up
        CUD = 'B',  /// cursor down
        CUF = 'C',  /// cursor forward
        CUB = 'D',  /// cursor back
        CNL = 'E',  /// cursor next line
        CPL = 'F',  /// cursor previous line
        CHA = 'G',  /// cursor horzontal absolute
        EL  = 'K',  /// erase in line
    };

    Cmd _cmd;
    i32 _val;
};

enum class SGR: i8
{
    RST = 0,

    CUR = 5,    cur = 25,
    RSV = 7,    rsv = 27,

    FONT_B   = 1, FONT_b = 21,
    FONT_F   = 2, FONT_f = 22,
    FONT_I   = 3, FONT_i = 23,
    FONT_U   = 4, FONT_u = 24,

    FONT_0 = 10,
    FONT_1 = 11,
    FONT_2 = 12,
    FONT_3 = 13,
    FONT_4 = 14,
    FONT_5 = 15,
    FONT_6 = 16,
    FONT_7 = 17,
    FONT_8 = 18,
    FONT_9 = 19,

    FG_BLK  = 30,   BG_BLK  = 40,
    FG_RED  = 31,   BG_RED  = 41,
    FG_GRN  = 32,   BG_GRN  = 42,
    FG_YEL  = 33,   BG_YEL  = 43,
    FG_BLU  = 34,   BG_BLU  = 44,
    FG_MAG  = 35,   BG_MAG  = 45,
    FG_CYN  = 36,   BG_CYN  = 46,
    FG_WHT  = 37,   BG_WHT  = 47,
    FG_DEF  = 38,   BG_DEF  = 48,
};

// SRC
enum class FGC: i8
{
    BLK,
    RED,
    GRN,
    YEL,
    BLU,
    MAG,
    CYN,
    WHT,
    DEF,
};

// ANSI Escape Code
enum class BGC : i8
{
    BLK,
    RED,
    GRN,
    YEL,
    BLU,
    MAG,
    CYN,
    WHT,
    DEF,
};

pub fn to_str(SGR code) noexcept->str;
pub fn to_str(FGC code) noexcept->str;
pub fn to_str(BGC code) noexcept->str;

class ConsoleLockGuard;

class Console
{
public:
    using LockGuard = ConsoleLockGuard;

    fs::fid_t _fid;

    Console(fs::fid_t fid) noexcept : _fid(fid)
    {}

    pub fn get_columns()    const noexcept -> u32;
    pub fn is_tty()         const noexcept -> bool;
    pub fn write_str(str s) const noexcept -> u32;

    template<class T0, class ...Ts>
    fn write_fmt(str fmt, const T0& t0, const Ts& ...ts) const noexcept -> u32 {
        mut& sbuf = get_sbuf();
        sbuf.clear();
        sformat(sbuf, fmt, t0, ts...);
        return write_str(sbuf);
    }

    template<class ...U>
    fn writeln(str fmt, const U& ...args) const noexcept -> u32 {
        mut& sbuf = get_sbuf();
        sbuf.clear();

        if constexpr(sizeof...(U)==0) {
            sbuf.push_slice(fmt);
            sbuf.push_slice(str("\n"));
        }
        else {
            sformat(sbuf, fmt, args...);
            sbuf.push_slice(str("\n"));
        }
        return write_str(sbuf);
    }

    fn lock() noexcept->LockGuard;

private:
    static pub fn get_sbuf() noexcept -> StrView&;
};

class ConsoleLockGuard: public Console {
public:
    using base = Console;
    sync::MutexGuard _guard;

protected:
    friend class Console;

    ConsoleLockGuard(const Console& val) noexcept
        : base(val), _guard(lock())
    {}

private:
    static pub fn lock()->sync::MutexGuard;
};

inline fn Console::lock() noexcept -> Console::LockGuard {
    return Console::LockGuard(*this);
}

class Stdout: public Console
{
public:
    static pub fn instance() -> Stdout& {
        static Stdout res;
        return res;
    }

private:
    Stdout() noexcept : Console(fs::fid_t::Stdout)
    {}

};

class Stderr: public Console
{
public:
    static pub fn instance() -> Stderr& {
        static Stderr res;
        return res;
    }

private:
    Stderr() noexcept : Console(fs::fid_t::Stderr)
    {}

};

inline fn stdout() -> Stdout& {
    return Stdout::instance();
}

inline fn stderr() -> Stderr& {
    return Stderr::instance();
}

}

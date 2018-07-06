
#include "config.inl"

#ifdef _WIN32
extern "C" fn GetStdHandle  (i32 nStdHandle)                                                            -> void*;
extern "C" fn GetConsoleMode(void* handle, u32* mode)                                                   -> i32;
extern "C" fn SetConsoleMode(void* handle, u32  mode)                                                   -> i32;
extern "C" fn WriteConsoleA(void* handle, const void* buffer, u32 size, u32*  nwrite, void* reserved)   -> bool;
extern "C" fn GetConsoleScreenBufferInfo(void* handle, struct CONSOLE_SCREEN_BUFFER_INFO* info)         -> i32;

using std_handle_t = void*;

static fn _get_std_handle(int fid) -> std_handle_t {
    mut res = static_cast<std_handle_t>(nullptr);
    let ret = ::_isatty(fid);
    let hid = -10 - fid;

    if (ret != 0) {
        /* @see https://docs.microsoft.com/en-us/windows/console/setconsolemode */
        res = ::GetStdHandle(hid);

        if (res != reinterpret_cast<void*>(0xFFFFFFFF)) {
            u32 mode = 0;
            if (::GetConsoleMode(res, &mode) == 1) {
                mode |= 0x0014;
                ::SetConsoleMode(res, mode);
            }
        }
    }

    return res;
}

static fn _write_console(int fid, const void* buff, u64 size) noexcept -> u32 {
    static let stdout_handle = ::_get_std_handle(STDOUT_FILENO);
    static let stderr_handle = ::_get_std_handle(STDERR_FILENO);
    let handle = fid == STDOUT_FILENO ? stdout_handle : stderr_handle;

    mut nret = 0u;
    (void)::WriteConsoleA(handle, buff, size, &nret, nullptr);
    return nret;
}

struct CONSOLE_SCREEN_BUFFER_INFO
{
    u16 size[2];
    u16 cursor_pos[2];
    u16 attr;
    u16 window[4];
    u16 max_window_size[2];
};

enum IoctlRequest
{
    TIOCGWINSZ
};

struct winsize
{
    u16 ws_row;
    u16 ws_col;
};

static fn _ioctl(int fid, IoctlRequest request, winsize* ws) -> i32 {
    (void)request;

    static let stdout_handle = ::_get_std_handle(STDOUT_FILENO);
    static let stderr_handle = ::_get_std_handle(STDERR_FILENO);
    let handle = fid == STDOUT_FILENO ? stdout_handle : stderr_handle;

    mut info = CONSOLE_SCREEN_BUFFER_INFO{};
    let ret = ::GetConsoleScreenBufferInfo(handle, &info);
    if (ret == 0) {
        return -1;
    }

    ws->ws_col = info.size[0];
    ws->ws_row = info.size[1];
    return 0;
}
#pragma endregion

#endif

#ifndef _UCRT
#   define _ioctl           ioctl
#   define _isatty          isatty
#   define _write_console   write
#endif

namespace ustd::io
{

#pragma region enums

pub fn trait_sfmt(Formatter& fmt, const CSI& val) noexcept {
    sformat(fmt._outbuf, "\x1b[{}{}", val._val, char(val._cmd));
}

static fn sgr_to_str(u32 code) noexcept -> str {
    static str ss[] = {
        "\x1b[0m"  ,"\x1b[1m" , "\x1b[2m" , "\x1b[3m",  "\x1b[4m",  "\x1b[5m",  "\x1b[6m",  "\x1b[7m",  "\x1b[8m",  "\x1b[9m",
        "\x1b[10m", "\x1b[11m", "\x1b[12m", "\x1b[13m", "\x1b[14m", "\x1b[15m", "\x1b[16m", "\x1b[17m", "\x1b[18m", "\x1b[19m",
        "\x1b[20m", "\x1b[21m", "\x1b[22m", "\x1b[23m", "\x1b[24m", "\x1b[25m", "\x1b[26m", "\x1b[27m", "\x1b[28m", "\x1b[29m",
        "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m", "\x1b[38m", "\x1b[39m",
        "\x1b[40m", "\x1b[41m", "\x1b[42m", "\x1b[43m", "\x1b[44m", "\x1b[45m", "\x1b[46m", "\x1b[47m", "\x1b[48m", "\x1b[49m" };

    let cnt = sizeof(ss) / sizeof(ss[0]);
    let idx = u32(code) < cnt ? u32(code) : 0;
    return ss[idx];
}


pub fn to_str(FGC code) noexcept -> str {
    let csi = u32(code) + 30;
    return sgr_to_str(csi);
}

pub fn to_str(BGC code) noexcept -> str {
    let csi = u32(code) + 40;
    return sgr_to_str(csi);
}

pub fn to_str(SGR code) noexcept -> str {
    let csi = u32(code);
    return sgr_to_str(csi);
}

#pragma endregion

#pragma region console

pub fn Console::write_str(str s) const noexcept -> u32 {
    return ::_write_console(int(_fid), s._data, s._size);
}

pub fn Console::is_tty() const noexcept -> bool {
    let ret = ::_isatty(int(_fid));
    if (ret == 0) {
        return false;
    }

    return true;
}

pub fn Console::get_sbuf() noexcept -> StrView& {
    static let $buff_size = 4 * 1024 * 1024;    // 4MB
    static thread_local FixedStr<$buff_size> res;
    return res;
}

pub fn Console::get_columns() const noexcept-> u32 {
    mut ws = ::winsize{};
    ::_ioctl(i32(_fid), TIOCGWINSZ, &ws);
    return u32(ws.ws_col);
}
#pragma endregion

#pragma region ConsoleLockGuard
pub fn ConsoleLockGuard::lock() -> sync::MutexGuard {
    static sync::Mutex mtx;
    return mtx.lock().unwrap();
}
#pragma endregion

}

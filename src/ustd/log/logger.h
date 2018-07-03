#pragma once

#include "ustd/core.h"
#include "ustd/io.h"
#include "ustd/fs.h"

namespace ustd::log
{

enum Level
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
};

pub fn to_str(Level level) noexcept -> str;

class Logger
{
public:
    Level               _level;
    Option<fs::TxtFile> _file_opt;

    pub Logger()  noexcept;
    pub ~Logger() noexcept;
    pub Logger(Logger&& other) noexcept;

    static pub fn instance() noexcept -> Logger&;

    // property[rw]: level
    __declspec(property(get = get_level, put = set_level)) Level level;
    fn get_level() const noexcept -> Level {
        return _level;
    }
    fn set_level(Level level) noexcept -> void {
        _level = level;
    }

    // property[w]: path
    __declspec(property(put = set_path)) fs::Path path;
    pub fn set_path(fs::Path path) noexcept -> void;

    template<class ...U> void trace(str fmt, const U& ...args) noexcept { log_fmt(Level::Trace, fmt, args...); }
    template<class ...U> void debug(str fmt, const U& ...args) noexcept { log_fmt(Level::Debug, fmt, args...); }
    template<class ...U> void info (str fmt, const U& ...args) noexcept { log_fmt(Level::Info,  fmt, args...); }
    template<class ...U> void warn (str fmt, const U& ...args) noexcept { log_fmt(Level::Warn,  fmt, args...); }
    template<class ...U> void error(str fmt, const U& ...args) noexcept { log_fmt(Level::Error, fmt, args...); }
    template<class ...U> void fatal(str fmt, const U& ...args) noexcept { log_fmt(Level::Fatal, fmt, args...); }

    pub fn log_msg(Level lvel, str text) noexcept -> void;
    pub fn log_msg(io::SGR title_sgr, str title, io::SGR body_sgr, str body_text) noexcept -> void;

    template<class ...U>
    fn log_fmt(Level level, str fmt, const U& ...args) noexcept -> void {
        if (level < _level) {
            return;
        }
        mut& sbuf = get_sbuf();
        sbuf.clear();
        ustd::sformat(sbuf, fmt, args...);
        log_msg(level, sbuf);
    }

    template<class ...U>
    fn log_fmt(io::SGR title_sgr, str title_text, io::SGR body_sgr, str fmt, const U& ...args) noexcept -> void {
        mut& sbuf = get_sbuf();
        sbuf.clear();
        ustd::sformat(sbuf, fmt, args...);
        log_msg(title_sgr, title_text, body_sgr, sbuf);
    }

private:
    // 64 KB
    static pub fn get_sbuf() noexcept->StrView&;
};

inline fn logger() noexcept -> Logger& {
    return Logger::instance();
}

template<class ...U> fn trace(str fmt, const U& ...args) noexcept -> void { logger().trace(fmt, args...); }
template<class ...U> fn debug(str fmt, const U& ...args) noexcept -> void { logger().debug(fmt, args...); }
template<class ...U> fn info (str fmt, const U& ...args) noexcept -> void { logger().info (fmt, args...); }
template<class ...U> fn warn (str fmt, const U& ...args) noexcept -> void { logger().warn (fmt, args...); }
template<class ...U> fn error(str fmt, const U& ...args) noexcept -> void { logger().error(fmt, args...); }
template<class ...U> fn fatal(str fmt, const U& ...args) noexcept -> void { logger().fatal(fmt, args...); }

}

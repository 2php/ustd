#include "config.inl"

namespace ustd::log
{

using namespace io;

pub fn to_str(Level level) noexcept -> str {
    switch (level) {
        case Trace: return "trace";
        case Debug: return "debug";
        case Info:  return "info";
        case Warn:  return "warn";
        case Error: return "error";
        case Fatal: return "fatal";
    }
    return str();
}

static fn to_sgr(Level level) noexcept -> io::SGR {
    switch (level) {
        case Trace: return SGR::FG_DEF;
        case Debug: return SGR::FG_GRN;
        case Info:  return SGR::FG_CYN;
        case Warn:  return SGR::FG_YEL;
        case Error: return SGR::FG_RED;
        case Fatal: return SGR::BG_RED;
    }
    return SGR::RST;
}

static fn to_title(Level level) noexcept -> str {
    switch (level) {
        case Trace: return "--";
        case Debug: return "**";
        case Info:  return "::";
        case Warn:  return "??";
        case Error: return "!!";
        case Fatal: return "xx";
    }
    return "  ";
}

pub Logger::Logger() noexcept
    : _level(Level::Debug)
{
    let level_str = env::var("ustd_log_level");
    let level_val = str_parse<Level>(level_str);

    if (level_val.is_some()) {
        _level = level_val._val;
    }
}

pub Logger::~Logger() noexcept {
}

pub Logger::Logger(Logger&& other) noexcept
    : _level(other.level)
{}

// 64 KB
pub fn Logger::get_sbuf() noexcept -> StrView& {
    static thread_local FixedStr<64 * 1024> res;
    return res;
}

pub fn Logger::instance() noexcept -> Logger& {
    static Logger res;
    return res;
}

pub fn Logger::set_path(fs::Path path) noexcept -> void {
    mut file = fs::TxtFile::create(path).ok();
    ustd::swap(_file_opt, file);
}

pub fn Logger::log_msg(Level level, str text) noexcept -> void {
    if (level < _level) {
        return;
    }

    let title_sgr   = to_sgr(level);
    let title_text  = to_title(level);
    let body_sgr    = io::SGR::RST;
    let body_text   = text;
    log_msg(title_sgr, title_text, body_sgr, body_text);
}

pub fn Logger::log_msg(io::SGR title_sgr, str title_text, io::SGR body_sgr, str body_text) noexcept -> void {
    let  time_point = time::Instant::now();
    let  time_secs = time_point.total_secs();

    if (io::stdout().is_tty()) {
        let columns         = io::stdout().get_columns();
        mut stdout_lock    = io::stdout().lock();
        stdout_lock.write_fmt("{}[{}]\x1b[0m {}{}\x1b[{}G\x1b[36m{>12.3}\x1b[0m\x1b[0G\n", title_sgr, title_text, body_sgr, body_text, (columns - 12), time_secs);
    }

    if (_file_opt.is_some()) {
        _file_opt._val.write_fmt("{>12.3} [{}] {}", time_secs, title_text, body_text);
    }
}

}
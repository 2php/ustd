#include "config.inl"


#ifdef _WIN32
extern "C" fn GetModuleHandleA(const char* path) -> void*;
extern "C" fn GetModuleFileNameA(void* handle, char path[], u32 size) -> u32;

extern "C" fn _rmdir(const char* path) -> i32;
extern "C" fn _mkdir(const char* path) -> i32;
extern "C" fn _getcwd(char* buf, i32 buf_size) -> char*;
extern "C" fn _chdir(const char* path) -> i32;

static fn ustd_mkdir(const char* path, int mode) -> i32 {
    (void)mode;
    return _mkdir(path);
}

#define _mkdir  ustd_mkdir

#endif

#ifndef _WIN32
#   define _stat64      stat
#   define _fstat64     fstat
#   define _access      access

#   define _mkdir       mkdir
#   define _rmdir       rmdir

#   define _getcwd      getcwd
#   define _chdir       chdir
#   define _access      access

#   define _setmode(...)
#endif


namespace ustd::fs
{

pub fn Path::get_fullpath() const noexcept -> FixedCStr<1024> {
    static let exe_path = current_exe();
    static let bin_path = exe_path.parrent;
    static let app_path = bin_path.parrent;

    mut res = FixedCStr<1024>();
    mut s   = str(*this);

#ifdef USTD_OS_WINDOWS
    static let home_path = env::var("USERPROFILE");
    static let temp_path = env::var("TEMP");

    if (this->starts_with(str("~/"))) {
        if (res.push_slice(home_path).is_none()) goto ERROR;
        s = s.slice(1u, s.len - 1u);
    }
    else if (s.starts_with(str("/tmp"))) {
        if (res.push_slice(temp_path).is_none()) goto ERROR;
        s = s.slice(4u, s.len - 4u);
    }
#endif

    if (s.starts_with(str("#/"))) {
        if (res.push_slice(app_path).is_none()) { goto ERROR; }
        s = s.slice(1u, s.len - 1);
    }

    if (res.push_slice(s).is_none()) goto ERROR;

#ifdef USTD_OS_WINDOWS
    res.replace('\\', '/');
#endif

    if (res._size < res._capacity) {
        res[res._size] = '\0';
    }

    return res;

ERROR:
    return {};
}

pub fn Path::get_parent() const noexcept -> Path {
    if (this->is_empty()) return {};

    // #####/#####
    //      ^
    //     idx
    mut idx = this->len - 1;
    while (idx > 0 && _data[idx] != '/') {
        --idx;
    }

    if (idx == 0) return {};
    let s = this->slice(0u, idx - 1);
    let p = Path(s);
    return p;
}

pub fn Path::get_file_name() const noexcept -> Path {
    if (this->is_empty()) return {};

    let len = this->len;
    if (len == 0)               return {};
    if (_data[len - 1] == '/')  return {};

    // #####/#####
    //      ^
    //     idx
    mut idx = len - 1;
    while (idx > 0 && _data[idx] != '/') {
        --idx;
    }

    let s = this->slice(idx + 1, len - 1);
    return s;
}

pub fn Path::get_extension() const noexcept -> Path {
    if (this->is_empty()) return {};

    let name = get_file_name();
    if (name.is_empty()) return {};

    // #####.#####
    //      ^
    //     idx
    let  len = name.len;
    mut  idx = len - 1;

    while (idx > 0 && _data[idx] != '.') { --idx; }

    if (this->_data[idx] != '.') return {};

    let s = name.slice(idx + 1, len - 1);
    return s;
}

pub fn Path::is_file() const noexcept -> bool {
    if (this->is_empty()) return false;

    let full_path = get_fullpath();

    struct ::_stat64 st;
    let eid = ::_stat64(full_path.data, &st);
    if (eid != 0) return false;

    return st.st_mode == S_IFREG;
}

pub fn Path::is_dir() const noexcept -> bool {
    if (this->is_empty()) return false;

    let full_path = get_fullpath();

    struct ::_stat64 st;
    let eid = ::_stat64(full_path.data, &st);
    if (eid != 0) {
        log::error("ustd::fs::Path[`{}`, path=`{}`].is_dir(): failed, error={}", this, *this, eid);
        return false;
    }

    return st.st_mode == S_IFDIR;
}

pub fn Path::is_exists() const noexcept -> bool {
    let full_path = get_fullpath();

    let stat = ::_access(full_path.data, 0);
    return stat == 0;
}

#pragma region funcs

pub fn current_dir() noexcept -> Path {
    static thread_local FixedStr<FixedPath<>::$capacity> s;

    let ptr = ::_getcwd(s.data, i32(s.capacity));
    if (ptr == nullptr) {
        return {};
    }
    let tmp = cstr(s.data);
    s._size = tmp._size;

    return str(s);
}

pub fn set_current_dir(Path path) noexcept -> Result<none_t> {
    let full_path = path.get_fullpath();

    let stat = ::_chdir(full_path.data);
    if (stat != 0) {
        let eid = os::get_error();
        log::error("ustd::fs::set_current_dir(path=`{}`): failed, error={}", path, eid);
        return Result<none_t>::Err(eid);
    }

    log::info("ustd::fs::set_current_dir(path=`{}`): success.", path);
    return Result<none_t>::Ok();
}

pub fn current_exe() noexcept -> Path {
    static FixedStr<FixedPath<>::$capacity> res;

#ifdef _WIN32
    let mod = ::GetModuleHandleA(nullptr);
    let len = ::GetModuleFileNameA(mod, res._data, res.capacity);
    res._size = len;
    res.replace('\\', '/');
#endif

#ifdef __APPLE__
    res._size = res.capacity;
    _NSGetExecutablePath(tmp, &tmp._size);
#endif

#ifdef __linux
    res._size = ::readlink("/proc/self/exe", res._data, res.capacity);
#endif

    return str(res);
}

pub fn current_app() noexcept -> Path {
    let exe_path = current_exe();
    let app_path = exe_path.parrent;
    return app_path;
}

pub fn rename(Path src, Path dst) noexcept -> Result<none_t> {
    let path_src = src.get_fullpath();
    let path_dst = dst.get_fullpath();

    let ret = ::rename(path_src.data, path_dst.data);
    if (ret != 0) {
        let eid = os::get_error();
        log::error("ustd::fs::rename(src=`{}`, dst=`{}`): failed, error=`{}`", src, dst, eid);
        return Result<none_t>::Err(eid);
    }
    return Result<none_t>::Ok();
}

pub fn create_dir(Path p) noexcept -> Result<none_t> {
    if (p.is_empty()) {
        return Result<none_t>::Err(os::Error::InvalidInput);
    }

    let full_path   = p.get_fullpath();
    let ret         = ::_mkdir(full_path.data, 0666);
    if (ret != 0) {
        let eid = os::get_error();
        log::error("ustd::fs::create_dir(path=`{}`): failed, error=`{}`", p, eid);
        return Result<none_t>::Err(eid);
    }

    log::info("ustd::fs::create_dir(path=`{}`): success", p);
    return Result<none_t>::Ok();
}

pub fn remove_dir(Path p) noexcept -> Result<none_t> {
    if (p.is_empty()) {
        return Result<none_t>::Err(os::Error::InvalidInput);
    }

    let full_path   = p.get_fullpath();
    let ret         = ::_rmdir(full_path.data);
    if (ret != 0) {
        let eid = os::get_error();
        log::error("ustd::fs::remove_dir(path=`{}`): failed, error=`{}`", p, eid);
        return Result<none_t>::Err(eid);
    }

    log::info("ustd::fs::remove_dir(path=`{}`): success", p);
    return Result<none_t>::Ok();
}

pub fn remove_file(Path p) noexcept -> Result<none_t> {
    if (p.is_empty()) {
        return Result<none_t>::Err(os::Error::InvalidInput);
    }

    let full_path   = p.get_fullpath();
    let ret         = ::remove(full_path.data);
    if (ret != 0) {
        let eid = os::get_error();
        log::error("ustd::fs::remove_file(path=`{}`): failed, error=`{}`", p, eid);
        return Result<none_t>::Err(eid);
    }

    log::info("ustd::fs::remove_file(path=`{}`): success", p);
    return Result<none_t>::Ok();
}

}

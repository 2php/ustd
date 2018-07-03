#pragma once

#include "ustd/core.h"

namespace ustd::fs
{

template<typename T>
using Result = ustd::Result<T, os::Error>;

struct Path: str
{
    using base = str;

    constexpr Path() noexcept : base()
    {}

    constexpr Path(str s) noexcept : base(s)
    {}

    template<u32 N>
    constexpr Path(const char(&s)[N]) noexcept: base(s)
    {}

    // property[r]: parrent
    __declspec(property(get = get_parent)) Path parrent;
    pub fn get_parent() const noexcept->Path;

    // property[r]: file_name
    __declspec(property(get = get_file_name)) Path file_name;
    pub fn get_file_name() const noexcept->Path;

    // property[r]: extension
    __declspec(property(get = get_extension)) Path extension;
    pub fn get_extension() const noexcept->Path;

    // method: check if is file
    pub fn is_file() const noexcept -> bool;

    // method: check if is dir
    pub fn is_dir() const noexcept -> bool;

    // method: check if exists
    pub fn is_exists() const noexcept -> bool;

    // method: full path
    pub fn get_fullpath() const noexcept -> FixedCStr<1024>;
};

template<u32 N=1024>
struct FixedPath: Path
{
    static constexpr let $capacity = N;
    using FixedCStr = FixedCStr<$capacity>;

    FixedCStr _buff;

    FixedPath() noexcept: Path{}, _buff{}
    {}

    FixedPath(str path) noexcept: Path(), _buff(path) {
        init();
    }

    FixedPath(const FixedPath& other) noexcept: Path(), _buff(other._buff) {
        init();
    }

    template<typename ...T>
    static fn from_fmt(str fmt, const T& ...args) noexcept -> FixedPath {
        let s = snformat<$capacity>(fmt, args...);
        return FixedPath(str(s));
    }

private:
    fn init() noexcept {
        static_cast<Path&>(*this) = str(_buff);
    }
};

pub fn current_exe      ()                      noexcept -> Path;
pub fn current_app      ()                      noexcept -> Path;
pub fn current_dir      ()                      noexcept -> Path;
pub fn set_current_dir  (Path  path)            noexcept -> Result<none_t>;
pub fn create_dir       (Path  path)            noexcept -> Result<none_t>;
pub fn remove_dir       (Path  path)            noexcept -> Result<none_t>;
pub fn remove_file      (Path  path)            noexcept -> Result<none_t>;
pub fn rename           (Path  from, Path to)   noexcept -> Result<none_t>;

}

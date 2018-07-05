#pragma once

#include "ustd/core.h"
#include "ustd/fs/path.h"
#include "ustd/os.h"

namespace ustd::fs
{

using Error = os::Error;

template<typename T>
using Result = ustd::Result<T, os::Error>;

enum class fid_t : i32
{
    Invalid = -1,
    Stdin   = 0,
    Stdout  = 1,
    Stderr  = 2,
};

enum class FileMode
{
    Open,
    Create,
};

pub fn to_str(FileMode mode) noexcept -> str;

enum class FileType
{
    Binary,
    Txt
};

pub fn to_str(FileType type) noexcept -> str;


class File
{
public:
    using Mode = FileMode;
    using Type = FileType;
    
    struct SeekFrom
    {
        enum SeekPos {
            Start,
            End,
            Current
        } Type;

        SeekPos _pos;
        i64     _offset;

        SeekFrom(SeekPos pos, i64 offset) noexcept: _pos(pos), _offset(offset)
        {}

        static fn start  (i64 offset) noexcept -> SeekFrom { return { Start,  offset }; }
        static fn end    (i64 offset) noexcept -> SeekFrom { return { End,    offset }; }
        static fn current(i64 offset) noexcept -> SeekFrom { return { Current,offset }; }
    };

public:
    fid_t _fid;

public:
    // ctor: move
    pub File(File&& other) noexcept;

    // dtor
    pub ~File() noexcept;

    // ctor
    static pub fn create(Path path) noexcept->Result<File>;

    // ctor
    static pub fn open(Path path) noexcept->Result<File>;

    // property[r]: is_valid
    pub fn is_valid() const noexcept;

    // property[r]: size
    pub fn size() const noexcept -> u64;

    // method: close
    pub fn close() noexcept -> void;

    // method: seek
    pub fn seek(SeekFrom pos) noexcept ->Result<u64>;

    // method: write
    pub fn read(void* dat, u64 size) noexcept->Result<u64>;

    // method: write
    pub fn write(const void* dat, u64 size) noexcept->Result<u64>;

protected:
    // ctor: struct
    explicit pub File(fid_t fid) noexcept;

    // ctor
    pub File(Path path, Mode mode, Type type) noexcept;

    // ctor: open for read
    static pub fn open_impl(Path path, Type type) noexcept -> Result<File>;

    // ctor: create for write
    static pub fn create_impl(Path path, Type type) noexcept -> Result<File>;
};

class TxtFile: public File
{
public:
    using File::_fid;

    pub TxtFile(TxtFile&& other) noexcept;
    pub ~TxtFile() noexcept;

    // ctor: open for read
    static pub fn open(Path path) noexcept->Result<TxtFile>;

    // ctor: create for write
    static pub fn create(Path path) noexcept->Result<TxtFile>;

    pub fn write_str(str text) noexcept->Result<u64>;

    template<class ...U>
    fn write_fmt(str fmt, const U& ...u) noexcept -> Result<u64> {
        let text    = sformat(fmt, u...);
        let res     = write_str(text);
        return res;
    }

    template<class ...U>
    static fn sformat(str fmt, const U& ...u) noexcept -> str {
        mut& sbuf = get_strbuf();

        sbuf.clear();
        let res = ustd::sformat(sbuf, fmt, u...);
        return res;
    }

protected:
    pub TxtFile(File&& other) noexcept;

    static pub fn get_strbuf() noexcept->StrView&;
};

class Stream
{
public:
    using Buff = List<u8>;

    // 64 KB
    constexpr static let $buf_size = u32(64 * 1024);   // 64KB

    File*   _file;
    Buff    _rbuf;
    Buff    _wbuf;

    pub Stream(Stream&& other) noexcept;
    pub ~Stream() noexcept;

    static fn from_file(File& file)->Stream;

    pub fn flush()                          noexcept -> Result<u64>;
    pub fn write(const void* dat, u64 size) noexcept -> Result<u64>;
    pub fn read (void*       dat, u64 size) noexcept -> Result<u64>;

#pragma region text file
    pub fn write_str(str s)         noexcept -> Result<u64>;
    pub fn read_str(StrView& str)   noexcept -> Result<u64>;
    pub fn read_str()               noexcept -> Result<String>;

    // method: write_fmt
    template<class ...U>
    fn write_fmt(str fmt, const U& ...u) noexcept -> Result<u64> {
        let outbuf = format(fmt, u...);
        let res    = write_str(outbuf);
        return res;
    }
#pragma endregion

protected:
    pub Stream(File& file) noexcept;
};

pub fn load_str(Path path)           -> Result<String>;
pub fn save_str(Path path, str text) -> Result<u64>;

template<class ...U>
fn save_fmt(Path path, str fmt, const U& ...u) -> Result<u64> {
    let text = TxtFile::sformat(fmt, u...);
    let res  = save_str(path, text);
    return res;
}

}

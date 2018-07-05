#include "config.inl"

namespace ustd::fs
{

#pragma region enums

pub fn to_str(FileMode mode) noexcept->str {
    switch(mode) {
        case FileMode::Create:  return "Create";
        case FileMode::Open:    return "Open";
    }
    return "";
}

pub fn to_str(FileType type) noexcept->str {
    switch (type) {
        case FileType::Binary:  return "Binary";
        case FileType::Txt:     return "Txt";
    }
    return "";
}


#pragma endregion

#pragma region File

template<typename U>
static fn check_head(fid_t fid, const U& expect) -> bool {
    mut tmp = U();
    let cnt = ::_read(int(fid), &tmp, sizeof(tmp));
    (void)::_lseek(int(fid), 0, SEEK_SET);

    if (u64(cnt) == sizeof(tmp) && bool(expect == tmp) ) return true;
    return false;
}

pub File::File(Path path, Mode mode, Type type) noexcept: _fid(fid_t::Invalid) {
    if (path.is_empty()) return;

    let full_path = path.get_fullpath();

    let open_flag = mode == Mode::Open ? O_RDONLY : (O_RDWR | O_CREAT | O_TRUNC);

#ifdef _UCRT
    let share_flag = _SH_DENYWR;
    let perm_flag = mode == Mode::Open ? _S_IREAD : _S_IWRITE;
    ::_sopen_s(reinterpret_cast<int*>(&_fid), full_path, open_flag, share_flag, perm_flag);
#else
    let perm_flag = 0644;
    _fid = fid_t(::_open(full_path, open_flag, perm_flag));
#endif

    if (_fid == fid_t::Invalid) return;

    // BOM [utf-8]
    if (type == Type::Txt) {
#ifdef _WIN32
        ::_setmode(int(_fid), O_TEXT);
#endif
        if (mode == Mode::Open) {
            if (check_head(_fid, i8x3('\xEF', '\xBB', '\xBF'))) {
                ::_lseek(int(_fid), 3, SEEK_SET);
            }
        }
    }
}

pub fn File::open_impl(Path path, Type type) noexcept -> Result<File> {

    if (path.is_empty()) {
        log::error("ustd::fs::File.open_impl(path=`{}`, type=`{}`): path is empty!!!", path, type);
        return Result<File>::Err(os::Error::NotFound);
    }

    if (!path.is_exists()) {
        log::error("ustd::fs::File.open_impl(path=`{}`, type=`{}`):  not exists", path, type);
        return Result<File>::Err(os::Error::NotFound);
    }

    if (path.is_dir()) {
        log::error("ustd::fs::File.open_impl(path=`{}`, type=`{}`): is dir, not file", path, type);
        return Result<File>::Err(os::Error::NotFound);
    }

    mut res = File(path, FileMode::Open, type);
    if (res._fid == fid_t::Invalid) {
        let eid = os::get_error();
        log::warn("ustd::fs::File.open_impl(path=`{}`, type=`{}`): error = {}", path, type, eid);
        return Result<File>::Err(eid);
    }

    log::debug("ustd::fs::File[fid={}].open_impl(path=`{}`, type=`{}`): success", i32(res._fid), path, type);
    return Result<File>::Ok(as_mov(res));
}

pub fn File::create_impl(Path path, Type type) noexcept -> Result<File> {
    if (path.is_empty()) {
        log::error("ustd::fs::File.create_impl(path=`{}`, type=`{}`): path is empty.", path, type);
        return Result<File>::Err(os::Error::InvalidInput);
    }

    if (path.is_exists()) {
        if (path.is_file()) {
            log::warn("ustd::fs::File.create_impl(path=`{}`, type=`{}`): file exists, overwrite.", path, type);
        }
        else {
            if (path.is_dir()) {
                log::error("ustd::fs::File.create_impl(path=`{}`, type=`{}`): create failed, is a dir", path, type);
                return Result<File>::Err(os::Error::AlreadyExists);
            }
            else {
                log::warn("ustd::fs::File.create_impl(path=`{}`, type=`{}`): file exists, overwrite.", path, type);
            }
        }
    }

    mut res = File(path, FileMode::Create, type);
    if (res._fid == fid_t::Invalid) {
        let eid = os::get_error();
        log::error("ustd::fs::File.create_impl(path=`{}`): failed, error = {}", path, eid);
        return Result<File>::Err(eid);
    }

    log::debug("ustd::fs::File[fid={}].create_impl(path=`{} `): success", i32(res._fid), path);
    return Result<File>::Ok(as_mov(res));
}

// ctor: struct
pub File::File(fid_t fid) noexcept : _fid(fid)
{}

pub File::File(File&& other) noexcept : _fid(other._fid) {
    other._fid = fid_t::Invalid;
}

pub File::~File() noexcept {
    this->close();
}

// ctor
pub fn File::create(Path path) noexcept -> Result<File> {
    return create_impl(path, Type::Binary);
}

// ctor
pub fn File::open(Path path) noexcept -> Result<File> {
    return open_impl(path, Type::Binary);
}

// property[r]: is_valid
pub fn File::is_valid() const noexcept {
    return _fid != fid_t::Invalid;
}

pub fn File::close() noexcept -> void {
    if (_fid == fid_t::Invalid) return;
    log::debug("ustd::fs::File[fid={}].close()", i32(_fid));
    ::_close(i32(_fid));
}

pub fn File::get_size() const noexcept -> usize {
    if (_fid == fid_t::Invalid) return 0;

    struct ::_stat64i32 st;
    let eid = ::_fstat64i32(int(_fid), &st);
    if (eid != 0) return 0;

    return usize(st.st_size);
}

pub fn File::seek(SeekFrom pos) noexcept -> Result<u64> {
    let origin =
        pos._pos == SeekFrom::Start   ? SEEK_SET :
        pos._pos == SeekFrom::End     ? SEEK_END :
        pos._pos == SeekFrom::Current ? SEEK_CUR : SEEK_SET;

#ifdef _UCRT
    let res = ::_lseek(int(_fid), pos._pos, origin);
#else
    let res = ::lseek(int(_fid), pos._pos, origin);
#endif

    if (res < 0 ) return Result<u64>::Err(os::get_error());

    return Result<u64>::Ok(usize(res));
}

pub fn File::read(void* dat, u64 size) noexcept -> Result<u64> {
    if (_fid == fid_t::Invalid) {
        return Result<u64>::Err(os::Error::InvalidData);
    }

#ifdef _UCRT
    let ret = ::_read(int(_fid), dat, u32(size));
#else
    let ret = ::read(int(_fid), dat, size);
#endif

    if (ret < 0)  return Result<u64>::Err(os::get_error());
    if (ret == 0) return Result<u64>::Err(os::Error::UnexpectedEof);

    return Result<u64>::Ok(u64(ret));
}

pub fn File::write(const void* dat, u64 size) noexcept -> Result<u64> {
    if (_fid == fid_t::Invalid) {
        return Result<u64>::Err(os::Error::InvalidData);
    }

#ifdef _UCRT
    let ret = ::_write(int(_fid), dat, u32(size));
#else
    let ret = ::write(int(_fid), dat, size);
#endif

    if (ret <  0) return Result<u64>::Err(os::get_error());
    if (ret == 0) return Result<u64>::Err(os::Error::UnexpectedEof);

    return Result<u64>::Ok(u64(ret));
}

#pragma endregion

#pragma region TxtFile

pub TxtFile::TxtFile(File&& other) noexcept
    : File(as_mov(other))
{}

// ctor: move
pub TxtFile::TxtFile(TxtFile&& other) noexcept : File(as_mov(other))
{}

// dtor
pub TxtFile::~TxtFile() noexcept
{}

// ctor: open for read
pub fn TxtFile::open(Path path) noexcept->Result<TxtFile> {
    return File::open_impl(path, FileType::Txt).map([](File&& f) { return TxtFile(as_mov(f)); });
}

// ctor: create for write
pub fn TxtFile::create(Path path) noexcept->Result<TxtFile> {
    return File::create_impl(path, FileType::Txt).map([](File&& f) { return TxtFile(as_mov(f)); });
}

pub fn TxtFile::write_str(str fmt) noexcept -> Result<u64> {
    let res = File::write(fmt._data, fmt._size);
    return res;
}
pub fn TxtFile::get_strbuf() noexcept -> StrView& {
    // 4 MB
    static thread_local FixedStr<4 * 1024 * 1024> res;
    return res;
}

#pragma endregion

#pragma region Stream

pub Stream::Stream(File& file) noexcept
    : _file(&file), _rbuf(Buff::with_capacity($buf_size)), _wbuf(Buff::with_capacity($buf_size))
{}

pub Stream::Stream(Stream&& other) noexcept
    : _file(other._file), _rbuf(as_mov(other._rbuf)), _wbuf(as_mov(other._wbuf))
{
    other._file = nullptr;
}

pub Stream::~Stream() noexcept {
    flush();
}

fn Stream::from_file(File& file) -> Stream {
    return Stream(file);
}

pub fn Stream::flush() noexcept -> Result<u64> {
    if (_file == nullptr)   return Result<u64>::Err(os::Error::InvalidData);
    if (_wbuf.len == 0)     return Result<u64>::Ok(0u);

    let res = _file->write(_wbuf.data, _wbuf.len);
    _wbuf._size = 0;
    return res;
}

pub fn Stream::write(const void* data, u64 size) noexcept -> Result<u64> {
    if (_file == nullptr)               return Result<u64>::Err(os::Error::InvalidData);
    if (data == nullptr || size == 0)   return Result<u64>::Err(os::Error::InvalidInput);

    // flush
    if (_wbuf.len + size > _wbuf.capacity) {
        flush();
    }

    // write dat to file
    if (_wbuf.len == 0 && size > _wbuf.capacity) {
        return _file->write(data, size);
    }

    // push dat to buffer
    _wbuf.push_slice(Slice<const u8>(static_cast<const u8*>(data), size));
    return Result<u64>::Ok(size);
}

pub fn Stream::write_str(str s) noexcept->Result<u64> {
    return write(reinterpret_cast<const u8*>(s._data), s._size);
}

pub fn Stream::read(void* data, u64 size) noexcept -> Result<u64> {
    if (_file == nullptr)               return Result<u64>::Err(os::Error::InvalidData);
    if (data == nullptr || size == 0)   return Result<u64>::Err(os::Error::InvalidInput);

    mut rem_dat = static_cast<u8*>(data);
    mut rem_cnt = size;

    // flush
    if (_rbuf._size != 0) {
        let cnt = ustd::min(rem_cnt, _rbuf._size);
        mcpy(static_cast<u8*>(rem_dat), _rbuf.data, cnt);
        _rbuf._data += cnt;
        _rbuf._size -= cnt;
        _rbuf._capacity -= cnt;

        if (_rbuf._size == 0) {
            _rbuf._data -= $buf_size - _rbuf._capacity;
            _rbuf._capacity = $buf_size;
        }

        rem_dat += cnt;
        rem_cnt -= cnt;
    }

    if (rem_cnt == 0) return Result<u64>::Ok(size);

    // read from file
    if (rem_cnt >= $buf_size) {
        _file->read(rem_dat, rem_cnt).map([=](u64 x) { return x + (size - rem_cnt); });
    }

    // read from buf 
    // note: now _rbuf is empty
    if (_rbuf.is_empty()) {
        let res = _file->read(_rbuf.data, _rbuf.capacity);
        if (res.is_err()) return res;
        _rbuf._size = u32(res._ok);
    }

    return this->read(rem_dat, rem_cnt).map([=](u64 x) { return x + (size - rem_cnt);  });
}

pub fn Stream::read_str(StrView& str) noexcept -> Result<u64> {
    let ptr = str._data + str._size;
    let cnt = str._capacity - str._size;
    let res = this->read(reinterpret_cast<u8*>(ptr), cnt);
    return res;
}

pub fn Stream::read_str() noexcept -> Result<String> {
    if (_file->_fid == fid_t::Invalid) return Result<String>::Err(os::Error::InvalidData);

    let file_size = _file->size;
    if (file_size == 0) { return Result<String>::Ok(); }

    let cnt = u32(file_size + 3) / 4 * 4;
    mut res = String::with_capacity(cnt);
    return read_str(res).map([&](u32) -> String {return as_mov(res); });
}


#pragma endregion

#pragma region funs

pub fn save_str(Path path, str text) -> Result<u64> {
    mut file_opt = TxtFile::create(path);
    if (file_opt.is_err()) {
        return Result<u64>::Err(file_opt._err);
    }

    mut& file = file_opt._ok;
    let res = file.write_str(text);
    return res;
}

pub fn load_str(Path path) -> Result<String> {
    mut file_opt = TxtFile::open(path);
    if (file_opt.is_err()) {
        return Result<String>::Err(file_opt._err);
    }

    mut& file     = file_opt._ok;
    let  file_len = file.size;
    mut  read_str = String::with_capacity(file_len);

    let  read_res = file.read(read_str._data, read_str._capacity);
    if (read_res.is_err()) {
        return Result<String>::Err(read_res._err);
    }

    read_str._size = read_res._ok;
    return Result<String>::Ok(as_mov(read_str));
}

#pragma endregion

}

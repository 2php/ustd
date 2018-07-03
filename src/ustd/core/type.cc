#include "config.inl"

namespace ustd
{

static str u_names[] = { "u0", "u8", "u16", "u24", "u32", "u40", "u48", "u56", "u64" };
static str i_names[] = { "i0", "i8", "i16", "i24", "i32", "i40", "i48", "i56", "i64" };
static str f_names[] = { "f0", "f8", "f16", "f24", "f32", "f40", "f48", "f56", "f64" };

static fn _get_builtin_names(Type type) noexcept -> str {
    switch(type._type) {
        case 'u': return u_names[type._size];
        case 'i': return i_names[type._size];
        case 'f': return f_names[type._size];
        default: return "";
    }
    return "";
}

pub fn Type::get_fullname() const noexcept -> str {
    if (_type != '?' && _size <= 8){
        let res = _get_builtin_names(*this);
        return res;
    }

    let prefix_len = u32(sizeof("ustd::str ustd::typeof() [T = "));
    let suffix_len = u32(sizeof("]") - 1);
    let res        = str(_desc._data + prefix_len, _desc.len - prefix_len - suffix_len);
    return res;
}

pub fn Type::get_name() const noexcept -> str {
    if (_type != '?' && _size <= 8) {
        let res = _get_builtin_names(*this);
        return res;
    }

    let name = get_fullname();
    let idx_opt = name.rfind(':');
    if (idx_opt.is_none()) {
        return name;
    }

    let idx = idx_opt._val;
    let res = str(name._data + idx + 1, name._size - idx - 1);
    return res;
}

pub fn Type::get_mod() const noexcept -> str {
    let name = get_fullname();
    let idx_opt = name.rfind(':');
    if (idx_opt.is_none() || idx_opt._val <= 1) return {};

    let idx = idx_opt._val;
    let mod = str(name._data, idx - 1);
    return mod;
}

pub fn Type::to_str() const noexcept -> str {
    let res = get_fullname();
    return res;
}

pub fn trait_sfmt(Formatter& fmt, const Type& type) noexcept -> void {
    let s = type.name;
    fmt(s);
}

}
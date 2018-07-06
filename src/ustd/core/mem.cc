#include "config.inl"

#ifdef __INTELLISENSE__
fn __builtin_operator_new   (u64 size)  -> void*;
fn __builtin_operator_delete(void* ptr)   -> void;
#endif

namespace ustd
{

static fn make_dims_list(u32 rank, const u64 dims[]) -> FixedList<u64, 8> {
    mut dims_list = FixedList<u64, 8>();

    for (mut i = 0u; i < rank; ++i) {
        dims_list.push(dims[i]);
    }

    return dims_list;
}

static fn get_total_cnt(u32 rank, const u64 dims[]) -> u64 {
    mut total_cnt = u64(1);

    for (mut i = 0u; i < rank; ++i) {
        total_cnt *= dims[i];
    }

    return total_cnt;
}

pub fn _mnew(Type type, u32 rank, const u64 dims[]) noexcept -> void* {
    let dims_list = make_dims_list(rank, dims);
    let total_cnt = get_total_cnt(rank, dims);

    mut res = static_cast<void*>(nullptr);

    try {
        if (total_cnt != 0) {
            res = ::__builtin_operator_new(type._size*total_cnt);
        }
        log::trace("ustd::mem::mnew<{}>(dims={}) -> Ok({})", type, dims_list, res);
    }
    catch(...) {
        log::trace("ustd::mem::mnew<{}>(dims={}) -> Error()", type, dims_list);
    }

    return res;
}

pub fn _mdel(Type type, void* raw) noexcept -> void {
    mut ptr = reinterpret_cast<u64*>(raw);

    try {
        if (ptr != nullptr) {
            ::__builtin_operator_delete(raw);
        }
        log::debug("ustd::mem::mdel<{}>(ptr=`{}`) -> Ok()", type, ptr);
    }
    catch(...) {
        log::error("ustd::mem::mdel<{}>(ptr=`{}`) -> Error()", type, ptr);
        return;
    }
}

pub fn _mcpy(Type type, void* dst, const void* src, u32 rank, const u64 dims[]) noexcept -> void {
    (void)type;
    let dims_list = make_dims_list(rank, dims);
    let total_cnt = get_total_cnt(rank, dims);
    ustd_builtin(memcpy)(dst, src, type._size*total_cnt);
}

}

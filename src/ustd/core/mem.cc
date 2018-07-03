#include "config.inl"

#ifdef __INTELLISENSE__
fn __builtin_operator_new   (usize size)  -> void*;
fn __builtin_operator_delete(void* ptr)   -> void;
#endif

namespace ustd
{

static fn make_dims_list(const usize dims[], u32 rank) -> FixedList<usize, 8> {
    mut dims_list = FixedList<usize, 8>();

    for (mut i = 0u; i < rank; ++i) {
        dims_list.push(dims[i]);
    }

    return dims_list;
}

static fn get_total_cnt(const usize dims[], u32 rank) -> usize {
    mut total_cnt = usize(1);

    for (mut i = 0u; i < rank; ++i) {
        total_cnt *= dims[i];
    }

    return total_cnt;
}

pub fn _mnew(Type type, const usize dims[], u32 rank) noexcept -> void* {
    let dims_list = make_dims_list(dims, rank);
    let total_cnt = get_total_cnt(dims, rank);

    try {
        let ptr = ::__builtin_operator_new(type._size*total_cnt);
        log::trace("ustd::mem::mnew<{}>(dims={}) -> Ok({})", type, dims_list, ptr);
        return ptr;
    }
    catch(...) {
        log::trace("ustd::mem::mnew<{}>(dims={}) -> Error()", type, dims_list);
        return nullptr;
    }
}

pub fn _mdel(Type type, void* raw) noexcept -> void {
    mut ptr = reinterpret_cast<u64*>(raw);

    try {
        ::__builtin_operator_delete(raw);
        log::debug("ustd::mem::mdel<{}>(ptr=`{}`) -> Ok()", type, ptr);
    }
    catch(...) {
        log::error("ustd::mem::mdel<{}>(ptr=`{}`) -> Error()", type, ptr);
        return;
    }
}

pub fn _mcpy(Type type, void* dst, const void* src, const usize dims[], u32 rank) noexcept -> void {
    (void)type;
    let dims_list = make_dims_list(dims, rank);
    let total_cnt = get_total_cnt(dims, rank);
    log::trace("ustd::mem::mcpy<{}>(dst={}, src={}, dims={4})", dst, src, dims_list);

    ustd_builtin(memcpy)(dst, src, type._size*total_cnt);
}

}

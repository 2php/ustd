#include "config.inl"

namespace ustd::cuda
{

enum ctx_t : u64 {};    // void*
enum dev_t : u32 {};    // int

enum class MemoryType {
    Host    = 0x1,
    Device  = 0x2,
    Array   = 0x3,
    Unified = 0x4,
};

pub fn to_str(arr_t arr) noexcept -> FixedStr<32> {
    mut ptr = reinterpret_cast<void*>(u64(arr));
    mut res = snformat<32>("{}", ptr);
    return res;
}

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
#pragma region context

class Context
{
public:
    explicit Context() {
        mod_init();
        driver_init();
        device_init();
        ctx_init();
        device_show();
    }

    ~Context() {
        ctx_destroy();
    }

    fn get_fn(str name) noexcept -> ffi::fun_t {
        if (_mod_opt.is_none()) {
            log::error("ustd::cuda::Context::get_fn(name={}): cannot load `nvcuda`", name);
            return ffi::fun_t(0);
        }

        let& mod = _mod_opt._val;
        let  fid_opt = mod[name];
        if (fid_opt.is_none()) {
            log::error("ustd::cuda::Context::get_fn(name={}): func not found", name);
            return ffi::fun_t(0);
        }

        let fid = fid_opt._val;
        return fid;
    }

private:
    Option<ffi::Mod> _mod_opt;
    dev_t            _dev_id;
    ctx_t            _ctx_id;

    fn mod_init() -> bool {
        mut mod_opt = ffi::Mod::load("nvcuda");
        if (mod_opt.is_none()) {
            log::error("ustd::cuda::Context.mod_init(): cannot load `nvcuda`");
            return false;
        };

        ustd::swap(_mod_opt, mod_opt);
        return true;
    }

    fn driver_init() -> bool {
        static let cu_init_fid = get_fn("cuInit");
        static let cu_init     = ffi::Fn<Error(u32)>(cu_init_fid);
        if (cu_init_fid == ffi::fun_t(0)) {
            log::error("ustd::cuda::Context.driver_init(): cannot find find `cuInit`");
            return false;
        }

        static let cu_init_eid = cu_init(0);
        if (cu_init_eid != Error::Success) {
            log::error("ustd::cuda::Context.driver_init(): error = {}", cu_init_eid);
            return false;
        }
        return true;
    }

    fn device_init() -> bool {
        static let cu_dev_get_fid = get_fn("cuDeviceGet");
        static let cu_dev_get     = ffi::Fn<Error(dev_t*, int)>(cu_dev_get_fid);
        if (cu_dev_get_fid == ffi::fun_t(0)) {
            log::error("ustd::cuda::Context.device_init(): cannot find find `cuDeviceGet`");
            return false;
        }

        let cu_dev_eid = cu_dev_get(&_dev_id, 0);
        if (cu_dev_eid != Error::Success) {
            log::error("ustd::cuda::Context.(): cannot get device, error={}", cu_dev_eid);
            return false;
        }

        return true;
    }


    fn ctx_init() -> bool {
        static let cu_ctx_create_fid = get_fn("cuCtxCreate_v2");
        static let cu_ctx_create     = ffi::Fn<Error(ctx_t* ctx, u32 flags, dev_t dev_id)>(cu_ctx_create_fid);
        if (cu_ctx_create_fid == ffi::fun_t(0)) {
            log::error("ustd::cuda::Context.ctx_init(): cannot find find `cuCtxCreate_v2`");
            return false;
        }

        let eid = cu_ctx_create(&_ctx_id, 0u, _dev_id);
        if (eid != Error::Success) {
            log::error("ustd::cuda::init(): cannot setup context, error = {}", eid);
            return false;
        }

        return true;
    }

    fn ctx_destroy() -> void {
        static let cu_ctx_destroy_fid = get_fn("cuCtxDestroy_v2");
        static let cu_ctx_destroy     = ffi::Fn<Error(ctx_t ctx)>(cu_ctx_destroy_fid);
        if (cu_ctx_destroy_fid == ffi::fun_t(0)) {
            log::error("ustd::cuda::Context.ctx_destroy(): cannot find find `cuCtxDestroy_v2`");
            return;
        }
        cu_ctx_destroy(_ctx_id);
    }

    fn get_device_name() -> FixedStr<256> {
        static let cu_dev_get_name_fid = get_fn("cuDeviceGetName");
        static let cu_dev_get_name     = ffi::Fn<Error(char* name, u32 len, dev_t id)>(cu_dev_get_name_fid);
        if (cu_dev_get_name_fid == ffi::fun_t(0)) {
            log::error("ustd::cuda::Context.get_device_name(): cannot find find `cuDeviceGetName`");
            return {};
        }

        mut cu_dev_name = FixedStr<256>();
        let cu_dev_name_eid = cu_dev_get_name(cu_dev_name._data, cu_dev_name._capacity, _dev_id);
        if (cu_dev_name_eid != Error::Success) {
            log::error("ustd::cuda::Context.get_device_name(): cannot get device name, error={}", cu_dev_name_eid);
            return {};
        }
        cu_dev_name._size = ::__builtin_strlen(cu_dev_name._data);
        return cu_dev_name;
    }

    fn get_device_mem() -> u64 {
        static let cu_dev_get_mem_fid = get_fn("cuDeviceTotalMem_v2");
        static let cu_dev_get_mem     = ffi::Fn<Error(u64* bytes, dev_t id)>(cu_dev_get_mem_fid);
        if (cu_dev_get_mem_fid == ffi::fun_t(0)) {
            log::error("ustd::cuda::Context.get_device_mem(): cannot find find `cuDeviceTotalMem_v2`");
            return 0;
        }

        mut cu_dev_mem = u64(0);
        let cu_dev_name_eid = cu_dev_get_mem(&cu_dev_mem, _dev_id);
        if (cu_dev_name_eid != Error::Success) {
            log::error("ustd::cuda::Context.get_device_mem(): error={}", cu_dev_name_eid);
            return 0;
        }
        return cu_dev_mem;
    }

    fn device_show() -> bool {
        let dev_name    = get_device_name();
        let dev_mem     = get_device_mem();
        let dev_mem_gb  = f64(dev_mem) / 1024.0f / 1024.0f / 1024.0f;

        if (dev_mem != 0) {
            log::info("ustd::cuda: dev_name=`{}`, total_mem={.3}GB", dev_name, dev_mem_gb);
        }
        return true;
    }

};

static fn get_fn_impl(str name) noexcept -> ffi::fun_t {
    static Context ctx;
    let fid = ctx.get_fn(name);
    return fid;
}

template<typename F>
static fn get_fn(str name) noexcept -> F* {
    let fid = get_fn_impl(name);

    if (fid == ffi::fun_t(0)) {
        return nullptr;
    }

    let fun = ffi::Fn<F>(fid).as_fun();
    return fun;
}

pub fn to_str(Error err_id) noexcept -> str {
    static let cu_get_err_name = get_fn<Error(Error, const char**)>("cuGetErrorName");
    if (cu_get_err_name == nullptr) {
        return {};
    }

    mut err_str  = static_cast<const char*>(nullptr);
    let err_eid  = cu_get_err_name(err_id, &err_str);

    if (err_eid != Error::Success) {
        return "CudaErrorUnknow";
    };

    let err_name = cstr(err_str);
    return err_name;
}

pub fn _sync() noexcept -> void {
    static let cu_ctx_sync = get_fn<Error()>("cuCtxSynchronize");
    if (cu_ctx_sync == nullptr) {
        return;
    }

    mut eid = cu_ctx_sync();
    if (eid != Error::Success) {
        log::error("ustd::cuda::sync() -> Error({})", eid);
        return;
    }

    log::info("ustd::cuda::sync() -> Ok()");
}
#pragma endregion

#pragma region stream
static fn _current_stream() -> stream_t& {
    static thread_local mut res = stream_t(0);
    return res;
}

pub fn get_stream() noexcept -> stream_t {
    return _current_stream();
}

pub fn set_stream(stream_t stream) noexcept -> void {
    _current_stream() = stream;
}
#pragma endregion

#pragma region memory
pub fn _dnew(Type type, u32 rank, const u64 dims[]) noexcept -> void* {
    static let cu_dev_malloc = get_fn<Error(void**, u64)>("cuMemAlloc_v2");
    if (cu_dev_malloc == nullptr) {
        return nullptr;
    }

    let dims_list = make_dims_list(rank, dims);
    let total_cnt = get_total_cnt(rank, dims);

    if (total_cnt == 0u) {
        return nullptr;
    }

    mut ptr = static_cast<void*>(nullptr);
    mut eid = cu_dev_malloc(&ptr, type._size*total_cnt);
    if (eid != Error::Success) {
        log::error("ustd::cuda::dnew<{}>(dims={}) -> Error({})", type, dims_list, eid);
        return nullptr;
    }

    log::info("ustd::cuda::dnew<{}>(dims={}) -> Ok({})", type, dims_list, ptr);
    return ptr;
}

pub fn _ddel(Type type, void* ptr) noexcept -> void {
    static let cu_dev_free = get_fn<Error(void*)>("cuMemFree_v2");
    if (cu_dev_free == nullptr) {
        return;
    }

    if (ptr == nullptr) {
        return;
    }

    let eid = cu_dev_free(ptr);
    if (eid != Error::Success) {
        log::error("ustd::cuda::ddel<{}>(ptr={}) -> Error({})", type, ptr, eid);
        return;
    }
    log::info("ustd::cuda::ddel<{}>(ptr={}) -> Ok()", type, ptr);
}

pub fn _hnew(Type type, u32 rank, const u64 dims[]) noexcept -> void* {
    static let cu_host_malloc = get_fn<Error(void**, u64)>("cuMemAllocHost_v2");
    if (cu_host_malloc == nullptr) {
        return nullptr;
    }

    let dims_list = make_dims_list(rank, dims);
    let total_cnt = get_total_cnt(rank, dims);

    if (total_cnt == 0u) {
        return nullptr;
    }

    mut ptr = static_cast<void*>(nullptr);
    let eid = cu_host_malloc(&ptr, type._size*total_cnt);
    if (eid != Error::Success) {
        log::error("ustd::cuda::hnew<{}>(dims={}) -> Error({})", type, dims_list, eid);
        return nullptr;
    }
    log::info("ustd::cuda::hnew<{}>(dims={}): Ok({})", type, dims_list, ptr);

    return ptr;
}

pub fn _hdel(Type type, void* ptr) noexcept -> void {
    static let cu_host_free = get_fn<Error(void*)>("cuMemFreeHost");
    if (cu_host_free == nullptr) {
        return;
    }

    if (ptr == nullptr) {
        return;
    }

    let eid = cu_host_free(ptr);
    if (eid != Error::Success) {
        log::error("ustd::cuda::hdel<{}>(ptr={}) -> Error({})", type, ptr, eid);
        return;
    }
    log::info("ustd::cuda::hdel<{}>(ptr={}) -> Ok()", type, ptr);
}

pub fn _mcpy(Type type, void* dst, const void* src, u32 rank, const u64 dims[]) noexcept -> void {
    static let cu_memcpy = get_fn<Error(void* dst, const void* src, u64 size)>("cuMemcpy");
    if (cu_memcpy == nullptr) {
        return;
    }

    let dims_list = make_dims_list(rank, dims);
    let total_cnt = get_total_cnt(rank, dims);

    if (total_cnt == 0u) {
        return;
    }

    let eid = cu_memcpy(dst, src, type._size*total_cnt);

    if (eid != Error::Success) {
        log::error("ustd::cuda::mcpy<{}>(dst={}, src={}, dims={}) -> Error({})", type, dst, src, dims_list, eid);
        return;
    }
    log::info("ustd::cuda::mcpy<{}>(dst={}, src={}, dims={}) -> Ok()", type, dst, src, dims_list);
}

#pragma endregion

#pragma region array

enum class arr_fmt_t: u32 {
    u8  = 0x01,
    u16 = 0x02,
    u32 = 0x03,

    i8  = 0x08,
    i16 = 0x09,
    i32 = 0x0a,

    f16 = 0x10,
    f32 = 0x20
};

enum class arr_flags_t: u32 {
    none        = 0x0,
    layered     = 0x1,
    surf        = 0x2,
    cubemap     = 0x4,
    tex_gather  = 0x8,
};

struct arr_desc_t {
    u64       width;
    u64       height;
    u64       depth;

    arr_fmt_t   format;
    u32         num_channels;
    arr_flags_t flags;
};

static fn get_array_fmt(Type type) -> arr_fmt_t {
    if (type._type == 'u') {
        switch (type._size) {
            case 1: return arr_fmt_t::u8;
            case 2: return arr_fmt_t::u16;
            case 4: return arr_fmt_t::u32;
            default:break;
        }
    }
    if (type._type == 'i') {
        switch (type._size) {
            case 1: return arr_fmt_t::i8;
            case 2: return arr_fmt_t::i16;
            case 4: return arr_fmt_t::i32;
            default:break;
        }
    }

    if (type._type == 'f') {
        switch (type._size) {
            case 2: return arr_fmt_t::f16;
            case 4: return arr_fmt_t::f32;
            default:break;
        }
    }

    return arr_fmt_t(0);
}

static fn arr_get_desc(arr_t arr) -> Result<arr_desc_t> {
    static let cu_arr_get_desc = get_fn<Error(arr_desc_t*, arr_t)>("cuArray3DGetDescriptor_v2");
    if (cu_arr_get_desc == nullptr) {
        return Result<arr_desc_t>::Err(Error::Invalid);
    }

    mut desc = arr_desc_t{};
    let eid  = cu_arr_get_desc(&desc, arr);
    if (eid != Error::Success) {
        log::error("ustd::cuda::arr_get_desc(arr={}): error = {}", u64(arr), eid);
        return Result<arr_desc_t>::Err(eid);
    }
    return Result<arr_desc_t>::Ok(desc);
}

pub fn _anew(Type type, u32 rank, const u64 dims[]) noexcept -> arr_t {
    static let cu_array_create = get_fn<Error(arr_t*, const arr_desc_t*)>("cuArray3DCreate_v2");
    if (cu_array_create == nullptr) {
        return arr_t(0);
    }

    if (rank == 0 || dims[0] == 0) {
        return arr_t(0);
    }

    let fmt = get_array_fmt(type);
    arr_desc_t desc = {
        rank > 0 ? dims[0] : 0,     // width
        rank > 1 ? dims[1] : 0,     // height
        rank > 2 ? dims[2] : 0,     // depth

        fmt,                        // format
        1,                          // chanels
        arr_flags_t::none,          // flags
    };

    mut res = arr_t(0);
    let eid = cu_array_create(&res, &desc);

    mut dims_list = FixedList<u32, 3>();
    for (mut i = 0u; i < rank; ++i) {
        dims_list.push(u32(dims[i]));
    }
    if (eid != Error::Success) {
        log::error("ustd::cuda::anew<{}>(dims={}) -> Error({})", type, dims_list, eid);
        return arr_t(0);
    }
    log::info("ustd::cuda::anew<{}>(dims={}) -> Ok({})", type, dims_list, res);

    return res;
}

pub fn _adel(Type type, arr_t arr) noexcept -> void {
    static let cu_array_destroy = get_fn<Error(arr_t)>("cuArrayDestroy");
    if (cu_array_destroy == nullptr) {
        return;
    }

    if (arr == arr_t(0)) {
        return;
    }

    mut eid = cu_array_destroy(arr);
    if (eid != Error::Success) {
        log::error("ustd::cuda::adel<{}>(arr={}) -> Error({})", type, arr, eid);
        return;
    }

    log::info("ustd::cuda::adel<{}>(arr={}) -> Ok()", type, arr);
}

struct mcpy_params_t {
    u64         src_x           = 0;
    u64         src_y           = 0;
    u64         src_z           = 0;
    u64         src_lod         = 0;
    MemoryType  src_type        = MemoryType::Unified;
    const void* src_host        = nullptr;
    const void* src_device      = nullptr;
    arr_t       src_array       = arr_t(0);
    void*       src_reserved    = nullptr;
    u64         src_pitch       = 0;
    u64         src_height      = 0;

    u64         dst_x           = 0;
    u64         dst_y           = 0;
    u64         dst_z           = 0;
    u64         dst_lod         = 0;
    MemoryType  dst_type        = MemoryType::Unified;
    const void* dst_host        = nullptr;
    const void* dst_device      = nullptr;
    arr_t       dst_array       = arr_t(0);
    void*       dst_reserved    = nullptr;
    u64         dst_pitch       = 0;
    u64         dst_height      = 0;

    u64         width           = 0;
    u64         height          = 0;
    u64         depth           = 0;
};

// ptr->arr
pub fn _acpy(Type type, arr_t dst, const void* src, u32 rank, const u64 dims[]) noexcept -> void {
    static let cu_memcpy = get_fn<Error(const mcpy_params_t*)>("cuMemcpy3D_v2");
    if (cu_memcpy == nullptr) {
        return;
    }

    // dims
    mut dims_list = FixedList<u32, 3>();
    for (mut i = 0u; i < dims_list._capacity; ++i) {
        let val = i < rank ? u32(dims[i]) : 1u;
        dims_list.push(val);
    }

    // params
    mcpy_params_t params;
    params.src_type   = MemoryType::Unified;
    params.src_device = src;

    params.dst_type   = MemoryType::Array;
    params.dst_array  = dst;

    params.width  = dims_list[0] * type._size;
    params.height = dims_list[1];
    params.depth  = dims_list[2];

    // call: cu_memcpy
    let eid = cu_memcpy(&params);

    // log message
    if (eid != Error::Success) {
        log::error("ustd::cuda::acpy(dst=arr:{}, src={}:{}, dims={}) -> Error({})", dst, type, src, dims_list, eid);
        return;
    }

    log::info("ustd::cuda::acpy(dst=arr:{}, src={}:{}, dims={}) -> Ok()", dst, type, src, dims_list);
}

// arr->ptr
pub fn _acpy(Type type, void* dst, arr_t src, u32 rank, const u64 dims[]) noexcept -> void {
    static let cu_memcpy = get_fn<Error(const mcpy_params_t*)>("cuMemcpy3D_v2");
    if (cu_memcpy == nullptr) {
        return;
    }

    // dims
    mut dims_list = FixedList<u32, 3>();
    for (mut i = 0u; i < dims_list._capacity; ++i) {
        let val = i < rank ? u32(dims[i]) : 1u;
        dims_list.push(val);
    }

    // params
    mcpy_params_t params;
    params.src_type   = MemoryType::Array;
    params.src_array  = src;

    params.dst_type   = MemoryType::Unified;
    params.dst_device = dst;

    params.width  = dims_list[0] * type._size;
    params.height = dims_list[1];
    params.depth  = dims_list[2];

    // call: cu_memcpy
    let eid = cu_memcpy(&params);

    // log message
    if (eid != Error::Success) {
        log::error("ustd::cuda::acpy(dst={}:{}, src=arr:{}, dims={}) -> Error({})", type, dst, src, dims_list, eid);
        return;
    }

    log::info("ustd::cuda::acpy(dst={}:{}, src=arr:{}, dims={}) -> Ok() ", type, dst, src, dims_list);
}

#pragma endregion

#pragma region texture

struct res_desc_t 
{
    u32         res_type;
    union {
        arr_t   array;
        u32     reserved[32];
    };
    u32         flags;
};

struct tex_desc_t {
    TexAddress  address_mode[3];
    TexFilter   filter_mode;
    u32         flags;
    u32         maxAnisotropy;
    TexFilter   mipmap_mode;
    f32         mipmap_evel_bias;
    f32         min_mipmap_level_clamp;
    f32         max_mipmap_level_clamp;
    f32         border_color[4];
    i32         reserved[12];
};

struct res_view_t {
    u32         format;
    u64       width;
    u64       height;
    u64       depth;
    u32         first_mipmap_level;
    u32         last_mipmap_level;
    u32         first_layer;
    u32         last_layer;
    u32         reserved[16];
};

static fn get_res_format(arr_fmt_t type, u32 n) -> u32 {

    let cnt = n / 2u;

    switch (type) {
        case arr_fmt_t::u8:     return 0x01 + cnt;  // u8
        case arr_fmt_t::u16:    return 0x07 + cnt;  // u16
        case arr_fmt_t::u32:    return 0x0d + cnt;  // u32
        case arr_fmt_t::i8 :    return 0x04 + cnt;  // i8
        case arr_fmt_t::i16:    return 0x0a + cnt;  // i16
        case arr_fmt_t::i32:    return 0x10 + cnt;  // i32
        case arr_fmt_t::f16:    return 0x13 + cnt;  // f16
        case arr_fmt_t::f32:    return 0x16 + cnt;  // f32
        default:                return 0x00;
    }

    return 0x00;
}

pub fn _tnew(Type type, arr_t arr, TexAddress address_mode, TexFilter filter_mode) noexcept -> tex_t {
    static let cu_tex_create = get_fn<Error(tex_t*, res_desc_t*, tex_desc_t*, res_view_t*)>("cuTexObjectCreate");
    if (cu_tex_create == nullptr) {
        return tex_t(0);
    }

    mut tex = tex_t(0);

    // res view
    res_desc_t res_desc = {};
    res_desc.res_type   = 0;  // cu_ressource_type_array
    res_desc.array      = arr;

    // tex desc
    tex_desc_t tex_desc = {};
    tex_desc.address_mode[0] = address_mode;
    tex_desc.address_mode[1] = address_mode;
    tex_desc.address_mode[2] = address_mode;
    tex_desc.filter_mode     = filter_mode;

    // arr desc
    let arr_desc_opt = arr_get_desc(arr);
    if (arr_desc_opt.is_err()) {
        log::error("ustd::cuda::tnew<{}>(...) -> Error({})", type, "InvalidData");
        return tex_t(0);
    }
    let& arr_desc = arr_desc_opt._ok;

    // res view
    res_view_t res_view = {};
    res_view.format = get_res_format(arr_desc.format, arr_desc.num_channels);
    res_view.width  = arr_desc.width;
    res_view.height = arr_desc.height;
    res_view.depth  = arr_desc.depth;

    let tex_eid = cu_tex_create(&tex, &res_desc, &tex_desc, &res_view);
    if (tex_eid != Error::Success) {
        log::error("ustd::cuda::tnew<{}>(...) -> Error({})", type, tex_eid);
        return tex_t(tex_eid);
    }
    log::info("ustd::cuda::tnew<{}>(...) -> Ok({})", type, u32(tex));

    return tex_t(tex_eid);
}

pub fn _tdel(Type type, tex_t tex) noexcept -> void {
    static let cu_tex_destroy = get_fn<Error(tex_t)>("cuTexObjectDestroy");
    if (cu_tex_destroy == nullptr) {
        return;
    }

    let eid = cu_tex_destroy(tex);
    if (eid != Error::Success) {
        log::error("ustd::cuda::tdel<{}>(tex={}) -> Error({})", type, u32(tex), eid);
        return;
    }
    log::info("ustd::cuda::tdel<{}>(tex={}) -> Ok()", type, u32(tex));
}

#pragma endregion

}

namespace ustd::cuda
{

unittest(mem) {
    mut h0 = math::NDArray<f32, 2>::with_dims({256, 256});
    h0 <<= math::vline(1.0f, 0.1f);
    assert_eq(h0(1, 1), 1.1f);

    mut d0 = cuda::NDArray<f32, 2>::with_dims({256, 256});
    d0 <<= h0;

    mut h1 = math::NDArray<f32, 2>::with_dims({ 256, 256 });
    h1 <<= d0;

    assert_eq(h1(1, 1), 1.1f);
}

}

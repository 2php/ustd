#include "config.inl"

#ifdef _WIN32
extern "C"
{
    fn LoadLibraryA(const char* path)                   -> void*;
    fn FreeLibrary(void* handle)                        -> i32;
    fn GetProcAddress(void* handle, const char* symbol) -> void*;
}

enum
{
    RTLD_LAZY = 0x1
};

static fn dlopen(const char* path, int mode) -> void* {
    (void)mode;

    let res = static_cast<void*>(::LoadLibraryA(path));
    if (res == reinterpret_cast<void*>(0xFFFFFFFF)) {
        return nullptr;
    }
    return res;
}

static fn dlsym(void* handle, const char* symbol) -> void* {
    let res = reinterpret_cast<void*>(GetProcAddress(handle, symbol));
    return res;
}

static fn dlclose(void* handle) -> i32 {
    mut res = ::FreeLibrary(handle);
    return res;
}
#endif

namespace ustd::ffi
{

using namespace ustd::io;

static fn get_mod_path(str name) noexcept -> fs::FixedPath<> {
#if defined(USTD_OS_WINDOWS)
    let res = fs::FixedPath<>::from_fmt("{}.dll", name);
#elif defined(USTD_OS_LINUX)
    let res = fs::FixedPath<>::from_fmt("lib{}.so", name);
#elif
    let res = fs::FixedPath<>(name);
#endif
    return res;
}

pub Mod::Mod(str name) noexcept {
    let path        = get_mod_path(name);
    let full_path   = path.get_fullpath();

    _mod = mod_t(reinterpret_cast<usize>(::dlopen(full_path.data, RTLD_LAZY)));
    if (_mod == mod_t(0)) {
        log::error("ustd::ffi::Mod[{}].ctor(name={}): cannot load library.", this, name);
        return;
    }
    log::info("ustd::ffi::Mod[{}].ctor(name={}): success", this, name);

    let mod_init_opt = get_fn("mod_init");
    if (mod_init_opt.is_some()) {
        log::info("ustd::ffi::Mod[{}].ctor(name={}): find `mod_init` ok.", this, name);
        let mod_init     = Fn<int()>(mod_init_opt._val);
        mod_init();
        log::info("ustd::ffi::Mod[{}].ctor(name={}): call `mod_init` ok.", this, name);
    }
}


pub Mod::~Mod() noexcept  {
    if (_mod == mod_t(0)) {
        return;
    }

    let mod_exit_opt = get_fn("mod_exit");
    if (mod_exit_opt.is_some()) {
        log::info("ustd::ffi::Mod[{}].dtor(): find `mod_exit` ok.", this);
        let mod_exit = Fn<int()>(mod_exit_opt._val);
        mod_exit();
        log::info("ustd::ffi::Mod[{}].ctor(): call `mod_exit` ok.", this);
    }

    ::dlclose(reinterpret_cast<void*>(usize(_mod)));
    log::info("ustd::ffi::Mod[{}].dtor(): success", this);
}

pub fn Mod::get_fn(str name, bool show_log) const noexcept -> Option<fun_t> {
    if (_mod == mod_t(0)) {
        return Option<fun_t>::None();
    }

    let cname = FixedCStr<256>(name);

    let res  = fun_t(usize(::dlsym(reinterpret_cast<void*>(usize(_mod)), cname)));
    if (res == fun_t(0)) { 
        if (show_log) {
            log::error("ustd::ffi::Mod[{}].[name={}]: cannot find function", this, name);
        }
        return Option<fun_t>::None();
    }

    return Option<fun_t>::Some(res);
}

}

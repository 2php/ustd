#pragma once

#include "ustd/core.h"
#include "ustd/io.h"

namespace ustd::ffi
{

class  Mod;

template<class F>
struct Fn;

enum mod_t : usize { };
enum fun_t : usize { };

class Mod final
{
    mod_t _mod;

public:
    pub ~Mod() noexcept;

    Mod(Mod&& other) noexcept: _mod(other._mod) {
        other._mod = mod_t(0);
    }

    static Option<Mod> load(str path) noexcept {
        mut res = Mod(path);
        if (res._mod == mod_t(0)) {
            return Option<Mod>::None();
        }

        return Option<Mod>::Some(static_cast<Mod&&>(res));
    }

    pub fn get_fn(str name, bool show_log=false) const noexcept->Option<fun_t>;

    fn operator[](str name) const noexcept -> Option<fun_t> {
        return get_fn(name, true);
    }

private:
    pub Mod(str path) noexcept;
};

template<class R, class ...T>
struct Fn<R(T...)> final
{
    friend class Mod;

    fun_t _addr;

    Fn(fun_t addr) : _addr(addr)
    { }

    fn as_fun() const noexcept -> R(*)(T...) {
        let res = reinterpret_cast<R(*)(T...)>(_addr);
        return res;
    }

    fn operator()(T ...u) const -> R {
        let fun = as_fun();
        return fun(u...);
    }

};

}

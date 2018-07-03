#pragma once

#include "ustd/core/mem.h"

namespace ustd
{

template<class F>
class Fn;

template<class R, class ...T>
class Fn<R(T...)>
{
public:
    using fun_t = R(void* obj, T...);
    using mov_t = void(void*, void*);
    using del_t = void(void* obj);

    fun_t*      _fun = nullptr;         //+ 1*8
    del_t*      _del = nullptr;         //+ 1*8
    mov_t*      _mov = nullptr;         //+ 1*8

    union {
        void*   _obj;                   // 01*8
        u8      _buf[13*sizeof(void*)]; //+13*8
    };

    Fn(Fn&& other) noexcept
        : _fun(other._fun), _del(other._del), _mov(other._mov), _obj(other._obj)
    {
        other._fun = nullptr;
        other._del = nullptr;
        other._mov = nullptr;

        if (_mov != nullptr) {
            _mov(_buf, other._buf);
        }
    }

    ~Fn() noexcept {
        if (_del == nullptr) {
            return;
        }

        let ptr = _mov == nullptr ? _obj : _buf;
        (*_del)(ptr);
    }

    template<class F>
    static fn from_fn(F&& f) noexcept -> Fn {
        return Fn(as_fwd<F>(f));
    }

    R operator()(T ...t) const {
        if (_fun == nullptr) {
            ustd::panic("ustd::boxed::FnBox: nullptr!!!");
        }
        let ptr = _mov == nullptr ? _obj : _buf;
        return (*_fun)(ptr, t...);
    }

private:
    template<class F>
    explicit Fn(F&& f) noexcept {
        using U = val_t<F>;
        constexpr static let use_buf = sizeof(U) <= sizeof(_buf);

        // set: _obj
        let ptr = reinterpret_cast<U*>(use_buf ? _buf : (_obj = mnew<U>(1)));
        ustd::ctor(ptr, as_fwd<F>(f));

        // set: _fun
        _fun = [](void* dat, T ...args) -> R {
            mut fun = static_cast<U*>(dat);
            return (*fun)(args...);
        };

        // set: _del
        _del = [](void* raw_ptr) {
            mut obj = reinterpret_cast<U*>(raw_ptr);
            ustd::dtor(obj);

            if (!use_buf) {
                mdel(raw_ptr);
            }
        };

        // set: _mov
        if (!use_buf) {
            return;
        }
        _mov = [](void* dst, void* src) {
            let pdst = reinterpret_cast<U*>(dst);
            let psrc = reinterpret_cast<U*>(src);

            // call: move-ctor
            ustd::ctor(pdst, as_mov(*psrc));
        };
    }
};

}

#pragma once

#include "ustd/core/builtin.h"
#include "ustd/core/mem.h"

namespace ustd
{

template<class T>
class Box final
{
public:
    T*  _ptr = nullptr;

    Box() = default;

    template<typename ...U>
    Box(U&& ...u) {
        _ptr = ustd::mnew<T>(1);
        new(_ptr)T(as_fwd<U>(u)...);
    }

    Box(Box&& other) noexcept: _ptr(other._ptr) {
        other._ptr = nullptr;
    }

    ~Box() {
        if (_ptr == nullptr) {
            return;
        }

        ustd::dtor(*_ptr);
        ustd::mdel(_ptr);
   }

    static fn from_raw(T* ptr) noexcept -> Box {
        return Box(ptr);  // use ptr
    }

    fn operator *() noexcept -> T& {
        return *_ptr;
    }

    fn operator *() const noexcept -> const T& {
        return *_ptr;
    }

    fn forget() noexcept -> void {
        _ptr = nullptr;
    }

private:
    explicit Box(T* ptr) noexcept : _ptr(ptr) {
    }
};

template<class F>
class FnBox;

template<class R, class ...T>
class FnBox<R(T...)>
{
public:
    struct res_t;

    using fun_t = R(res_t*, T...);
    using del_t = void(res_t* obj);

    struct res_t {
        fun_t*  _fun = nullptr;
        del_t*  _del = nullptr;
    };
    res_t* _res = nullptr;

    FnBox(FnBox&& other) noexcept: _res(other._res) {
        other._res = nullptr;
    }

    ~FnBox() noexcept {
        if (_res == nullptr) {
            return;
        }

        _res->_del(_res);
    }

    static fn from_raw(res_t* res) noexcept -> FnBox {
        return FnBox(res);
    }

    template<class F>
    static fn from_fn(F&& f) noexcept -> FnBox {
        return FnBox(as_fwd<F>(f));
    }

    fn forget() noexcept -> void {
        _res = nullptr;
    }

    fn operator()(T ...t) const -> R {
        if (_res == nullptr) {
            ustd::panic("ustd::FnBox<...>: _res = nullptr.");
        }
        return (_res->_fun)(_res, t...);
    }

private:
    explicit FnBox(res_t* res) noexcept: _res(res)
    {}

    template<typename U>
    explicit FnBox(U&& obj) noexcept: _res(nullptr) {
        using obj_st = mut_t<val_t<U>>;

        struct fnbox_res_t {
            using fun_st = R(fnbox_res_t*,   T...);
            using del_st = void(fnbox_res_t* obj);

            fun_st* _fun;
            del_st* _del;
            obj_st  _obj;

            explicit fnbox_res_t(U&& obj): _obj(as_fwd<U>(obj)) {
                _fun = [](fnbox_res_t* self, T ...args) -> R {
                    return (self->_obj)(args...);
                };

                _del = [](fnbox_res_t* self) {
                    ustd::dtor(self);
                    ustd::mdel(self);
                };
            }

            ~fnbox_res_t()
            {}
        };

        // alloc
        mut res = mnew<fnbox_res_t>(1);
        ustd::ctor(res, as_fwd<U>(obj));

        // res -> res
        _res = reinterpret_cast<res_t*>(res);
    }
};

}

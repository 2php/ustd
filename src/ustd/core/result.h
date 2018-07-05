#pragma once

#include "ustd/core/option.h"

namespace ustd
{

// result: val
template<typename T, typename E>
class Result
{
public:
    using ok_t  = T;
    using err_t = E;
    using nil_t = none_t;

    bool        _valid;
    union {
        nil_t   _nil;
        ok_t    _ok;
        err_t   _err;
    };

#pragma region ctor/dtor
    // ctor
    template<typename ...U>
    Result(immut_t<bool, true>, U&& ...u) noexcept: _valid(true), _ok(as_fwd<U>(u)...)
    {}

    // ctor
    template<typename ...U>
    Result(immut_t<bool, false>, U&& ...u) noexcept : _valid(false), _err(as_fwd<U>(u)...)
    {}

    ~Result() {
        if (_valid)  _ok.~T();
        if (!_valid) _err.~E();
    }

    Result(Result&& other) : _valid(other._valid), _nil() {
        if (_valid)  new(&_ok) T(as_mov(other._ok));
        if (!_valid) new(&_err)E(as_mov(other._err));
    }

    Result(const Result& other) _if(trait<T>::$copy && trait<E>::$copy): _valid(other._valid), _nil() {
        if (_valid)  new(&_ok) T(other._ok);
        if (!_valid) new(&_err)E(other._err);
    }

    // ctor: ok
    template<class ...U>
    static fn Ok(U&& ...u) noexcept -> Result {
        return Result(immut_t<bool, true>{}, as_fwd<U>(u)...);
    }

    // ctor: err
    template<class ...U>
    static fn Err(U&& ...u) noexcept -> Result {
        return Result(immut_t<bool, false>{}, as_fwd<U>(u)...);
    }
#pragma endregion

#pragma region property
    // method: is_ok
    fn is_ok() const noexcept -> bool {
        return _valid;
    }

    // method: is_err
    fn is_err() const noexcept -> bool {
        return !_valid;
    }
#pragma endregion

#pragma region ok
    fn ok() & -> Option<T&> {
        if (!_valid) return Option<T&>::None();
        return Option<T&>::Some(_ok);
    }

    fn ok() const& noexcept -> Option<const T&> {
        if (!_valid) return Option<const T&>::None();
        return Option<const T&>::Some(_ok);
    }

    fn ok() && ->Option<T> {
        if (!_valid) return Option<T>::None();
        return Option<T>::Some(as_mov(_ok));
    }
#pragma endregion

#pragma region err
    fn err() & noexcept -> Option<E&> {
        if (_valid) return Option<const E&>::None();
        return Option<ref_t<E>>::Some(_err);
    }

    // method: err[cref]
    fn err() const& noexcept -> Option<const E&> {
        if (_valid) return Option<cref_t<E>>::None();
        return Option<cref_t<E>>::Some(_err);
    }

    // method: err[move], discard success value
    fn err() && -> Option<E> {
        if (_valid) return Option<E>::None();
        return Option<E>::Some(as_mov(_err));
    }
#pragma endregion

#pragma region unwrap

    fn unwrap()& -> T& {
        if (!is_ok()) ustd::panic("ustd::Result<{}>::unwrap()& : assert is_ok()failed.", typeof<T>());
        return _ok;
    }

    fn unwrap() const& -> const T& {
        if (!is_ok()) ustd::panic("ustd::Result<{}>::unwrap() const&: assert is_ok()failed.", typeof<T>());
        return _ok;
    }

    // method: unwrap[move]
    fn unwrap() && -> T {
        if (!is_ok()) ustd::panic("ustd::Result<{}>::unwrap()&& : assert is_ok()failed.", typeof<T>());
        return as_mov(_ok);
    }
#pragma endregion

#pragma region unwrap_or
    // method: unwrap_or[cref]
    template<class U>
    fn unwrap_or(U& def) const& -> U& {
        if (is_ok()) return _ok;
        return def;
    }

    // method: unwrap_or[cref]
    template<class U>
    fn unwrap_or(U& def) & -> U& {
        if (is_ok()) return _ok;
        return def;
    }

    // method: unwrap_or[move]
    template<typename U>
    fn unwrap_or(U&& def) && noexcept -> T {
        if (is_ok()) return as_mov(_ok);
        return as_fwd<U>(def);
    }
#pragma endregion

#pragma region except
    // method: except[move]
    template<typename ...U>
    fn except(const str& fmt, const U& ...args) && -> ok_t {
        if (is_err()) { ustd::panic(fmt, args...); }
        return static_cast<T&&>(_ok);
    }

    // method: except[cref]
    template<typename ...U>
    fn except(const str& fmt, const U& ...args) const& -> cref_t<ok_t> {
        if (is_err()) { ustd::panic(fmt, args...); }
        return _ok;
    }

    // method: except[cref]
    template<typename ...U>
    fn except(const str& fmt, const U& ...args) & -> ref_t<ok_t> {
        if (is_err()) { ustd::panic(fmt, args...); }
        return _ok;
    }
#pragma endregion

#pragma region operator|
    // operator: or[move]
    template<typename R>
    fn operator |(R&& def) && noexcept -> Result<T, E> {
        if (is_ok()) return Result<T, E>::Ok(static_cast<T&&>(_ok));
        return as_fwd<R>(def);
    }

    // operator: or[copy]
    template<typename R>
    fn operator |(R&& def) const& noexcept -> Result<T, E> {
        if (is_ok()) return Result<T, E>::Ok(_ok);
        return as_fwd<R>(def);
    }
#pragma endregion

#pragma region operator&
    // operator: and[move]
    template<typename R, typename U=typename R::ok_t>
    fn operator &(R&& def) && noexcept -> Result<U, E> {
        if (is_err()) return Result<U, E>::Err(as_mov(_err));
        return as_fwd<R>(def);
    }

    // operator: and[copy]
    template<typename R, typename U=typename R::ok_t>
    fn operator &(R&& def) const& noexcept -> Result<U, E> {
        if (is_err()) return Result<U, E>::Err(_err);
        return as_fwd<R>(def);
    }

    // operator: and[copy]
    template<typename R, typename U = typename R::ok_t>
    fn operator &(R&& def) & noexcept -> Result<U, E> {
        if (is_err()) return Result<U, E>::Err(_err);
        return as_fwd<R>(def);
    }
#pragma endregion

#pragma region and_then
    // method: and_then[move]
    template<typename F>
    fn and_then(F&& op) && noexcept {
        using U = decltype(op(as_mov(_ok))._ok);

        if (is_err()) return Result<U, E>::Err(as_mov(_err));
        return op(as_mov(_ok));
    }
    
    // method: and_then[copy]
    template<typename F>
    fn and_then(F&& op) const& noexcept {
        using U = decltype(op(_ok)._ok);

        if (is_err()) return Result<U, E>::Err(_err);
        return op(_ok);
    }

    // method: and_then[copy]
    template<typename F>
    fn and_then(F&& op) & noexcept {
        using U = decltype(op(_ok)._ok);

        if (is_err()) return Result<U, E>::Err(_err);
        return op(_ok);
    }
#pragma endregion

#pragma region or_else
    // method: or_else[move]
    template<typename O>
    fn or_else(O&& op) && {
        using F = decltype(op(as_mov(_ok))._ok);

        if (is_ok()) return Result<T, F>::Ok(as_mov(_ok));
        return op(as_mov(_ok));
    }

    // method: or_else[copy]
    template<typename O>
    fn or_else(O&& op) const& {
        using F = decltype(op(_ok)._ok);

        if (is_ok()) return Result<T, F>::Ok(_ok);
        return op(_ok);
    }

    // method: or_else[copy]
    template<typename O>
    fn or_else(O&& op) & {
        using F = decltype(op(_ok)._ok);

        if (is_ok()) return Result<T, F>::Ok(_ok);
        return op(_ok);
    }
#pragma endregion

#pragma region map
    // method: map[move]
    template<typename F, class U = decltype(declval<ustd::val_t<F>>()(as_mov(_ok)))>
    fn map(F&& op) && -> Result<U, E> {
        if (is_err()) return Result<U, E>::Err(as_mov(_err));
        return Result<U, E>::Ok(op(as_mov(_ok)));
    }

    // method: map[cref]
    template<typename F, class U = decltype(declval<ustd::val_t<F>>()(_ok))>
    fn map(F&& op) const& -> Result<U, E> {
        if (is_err()) Result<U, E>::Err(_err);
        return Result<U, E>::Ok(op(_ok));
    }

    // method: map[cref]
    template<typename F, class U = decltype(declval<ustd::val_t<F>>()(_ok))>
    fn map(F&& op) & -> Result<U, E> {
        if (is_err()) return Result<U, E>::Err(_err);
        return Result<U, E>::Ok(op(_ok));
    }
#pragma endregion
};

// result: ref
template<typename T, typename E>
class Result<T&, E>
{
public:
    using ok_t  = T&;
    using err_t = E;

    bool        _valid;
    union {
        ok_t    _ok;
        err_t   _err;
    };

#pragma region ctor/dtor
    // ctor
    template<typename ...U>
    Result(immut_t<bool, true>, U&& ...u) noexcept : _valid(true), _ok(as_fwd<U>(u)...)
    {}

    // ctor
    template<typename ...U>
    Result(immut_t<bool, false>, U&& ...u) noexcept : _valid(false), _err(as_fwd<U>(u)...)
    {}

    ~Result() {
        if (!_valid) ustd::dtor(&_err);
    }

    Result(Result&& other) : _valid(other._valid), _ok(other._ok) {
        if (!_valid) ustd::ctor(&_err, as_mov(other._err));
    }

    Result(const Result& other) : _valid(other._valid), _ok(other._ok) {
        if (!_valid) ustd::ctor(&_err, as_mov(other._err));
    }

    // ctor: ok
    template<class ...U>
    static fn Ok(U&& ...u) noexcept -> Result {
        return Result(immut_t<bool, true>{}, as_fwd<U>(u)...);
    }

    // ctor: err
    template<class ...U>
    static fn Err(U&& ...u) noexcept -> Result {
        return Result(immut_t<bool, false>{}, as_fwd<U>(u)...);
    }
#pragma endregion

#pragma region property
    // method: is_ok
    fn is_ok() const noexcept -> bool {
        return _valid;
    }

    // method: is_err
    fn is_err() const noexcept -> bool {
        return !_valid;
    }
#pragma endregion

#pragma region ok
    fn ok() & -> Option<T&> {
        if (is_err()) return Option<T&>::None();
        return Option<T&>::Some(_ok);
    }

    fn ok() const& noexcept -> Option<const T&> {
        if (is_err()) return Option<const T&>::None();
        return Option<const T&>::Some(_ok);
    }

#pragma endregion

#pragma region err
    fn err() & noexcept -> Option<E&> {
        if (is_ok()) return Option<const E&>::None();
        return Option<ref_t<E>>::Some(_err);
    }

    // method: err[cref]
    fn err() const& noexcept -> Option<const E&> {
        if (is_ok()) return Option<cref_t<E>>::None();
        return Option<cref_t<E>>::Some(_err);
    }

    // method: err[move], discard success value
    fn err() && -> Option<E> {
        if (is_ok()) return Option<E>::None();
        return Option<E>::Some(as_mov(_err));
    }
#pragma endregion

#pragma region unwrap
    // method: unwrap[cref]
    fn unwrap() -> T& {
        if (!is_ok()) ustd::panic("ustd::Result<{}>::unwrap()& : assert is_ok()failed.", typeof<T>());
        return _ok;
    }

    // method: unwrap[cref]
    fn unwrap() const -> const T& {
        if (!is_ok()) ustd::panic("ustd::Result<{}>::unwrap() const&: assert is_ok()failed.", typeof<T>());
        return _ok;
    }

#pragma endregion

#pragma region map
    template<typename F>
    fn map(F&& op) {
        using U = decltype(op(_ok));

        if (is_err()) return Result<U, E>::Err(_err);
        return Result<U, E>::Ok(op(_ok));
    }

    template<typename F>
    fn map(F&& op) const {
        using U = decltype(op(_ok));

        if (is_err()) Result<U, E>::Err(_err);
        return Result<U, E>::Ok(op(_ok));
    }
#pragma endregion
};

// result: void
template<typename E>
class Result<void, E>
{
public:
    using ok_t  = none_t;
    using err_t = E;

    bool        _valid;

    union {
        ok_t    _ok;
        err_t   _err;
    };

#pragma region ctor/dtor
    // ctor
    template<typename ...U>
    Result(immut_t<bool, true>, U&& ...u) noexcept : _valid(true), _ok(as_fwd<U>(u)...)
    {}

    // ctor
    template<typename ...U>
    Result(immut_t<bool, false>, U&& ...u) noexcept : _valid(false), _err(as_fwd<U>(u)...)
    {}

    ~Result() {
        if (!_valid) _err.~E();
    }

    Result(Result&& other) : _valid(other._valid), _ok(other._ok) {
        if (!_valid) new (&_err)E(as_mov(other._err));
    }

    Result(const Result& other) : _valid(other._valid), _ok(other._ok) {
        if (!_valid) new (&_err)E(other._err);
    }

    // ctor: ok
    template<class ...U>
    static fn Ok(U&& ...u) noexcept -> Result {
        return Result(immut_t<bool, true>{}, as_fwd<U>(u)...);
    }

    // ctor: err
    template<class ...U>
    static fn Err(U&& ...u) noexcept -> Result {
        return Result(immut_t<bool, false>{}, as_fwd<U>(u)...);
    }
#pragma endregion

#pragma region property
    // method: is_ok
    fn is_ok() const noexcept -> bool {
        return _valid;
    }

    // method: is_err
    fn is_err() const noexcept -> bool {
        return !_valid;
    }
#pragma endregion

#pragma region map
    template<typename F>
    fn map(F&& op) const {
        using U = decltype(op());

        if (is_err()) Result<U, E>::Err(_err);
        return Result<U, E>::Ok(op());
    }
#pragma endregion
};
}

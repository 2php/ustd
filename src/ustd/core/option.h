#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

template<class T>
struct Slice;

using str = Slice<const char>;

// option: val
template<typename T>
class Option
{
public:
    using val_t = T;
    using nil_t = void*;

    bool _valid;

    union {
        nil_t _nil;
        val_t _val;
    };

    // ctor: 
    constexpr Option() noexcept: Option(immut_t<bool, false>())
    {}

    // ctor[move]
    Option(Option&& other) noexcept: _valid(other._valid), _nil() {
        if (!_valid) return;
        new(&_val)T(as_mov(other._val));
    }

    // ctor[copy]
    Option(const Option& other) noexcept __enable_if__(trait<T>::$copy): _valid(other._valid), _nil() {
        if (!_valid) return;
        new(&_val)T(other._val);
    }

    ~Option() {
        if (!_valid) return;
        _val.~T();
    }

    // ctor: None
    static fn None() noexcept -> Option {
        return Option(immut_t<bool, false>());
    }

    // ctor: Some
    template<class ...U>
    static fn Some(U&& ...u) noexcept -> Option {
        return Option(immut_t<bool, true>(), as_fwd<U>(u)...);
    }

    // method: is_some
    fn is_none() const noexcept -> bool {
        return _valid == false;
    }

    // method: is_some
    fn is_some() const noexcept -> bool {
        return _valid == true;
    }
#pragma endregion

#pragma region except
    template<typename ...U>
    fn except(const str& fmt, const U& ...u) && -> T {
        if (!is_some()) { ustd::panic(fmt, u...); }
        return as_mov(_val);
    }

    template<typename ...U>
    fn except(const str& fmt, const U& ...u) & -> T& {
        if (!is_some()) { ustd::panic(fmt, u...); }
        return _val;
    }

    template<typename ...U>
    fn except(const str& fmt, const U& ...u) const& -> const T& {
        if (!is_some()) { ustd::panic(fmt, u...); }
        return _val;
    }
#pragma endregion

#pragma region unwrap
    fn unwrap() && noexcept -> T {
        if (!is_some()) ustd::panic("ustd::option::Option<{}>: expect `is_some` failed.", typeof<T>());
        return as_mov(_val);
    }

    fn unwrap() & noexcept -> T& {
        if (!is_some()) ustd::panic("ustd::option::Option<{}>: expect `is_some` failed.", typeof<T>());
        return _val;
    }

    fn unwrap() const& noexcept -> const T& {
        if (!is_some()) ustd::panic("ustd::option::Option<{}>: expect `is_some` failed.", typeof<T>());
        return _val;
    }
#pragma endregion

#pragma region unwrap_or
    fn unwrap_or(T& def) noexcept -> T& {
        if (is_some()) return _val;
        return def;
    }

    fn unwrap_or(const T& def) const noexcept -> const T& {
        if (is_some()) return _val;
        return def;
    }
#pragma endregion

#pragma region map
    template<class F>
    fn map(F&& f) {
        using U = decltype(f(_val));

        if (is_none()) return Option<U>::None();
        return Option<U>::Some(f(_val));
    }

    template<class F>
    fn map(F&& f) const {
        using U = decltype(f(_val));

        if (is_none()) return Option<U>::None();
        return Option<U>::Some(f(_val));
    }


#pragma endregion

private:
    Option(immut_t<bool, false>): _valid(false), _nil()
    {}

    template<class ...U>
    Option(immut_t<bool, true>, U&& ...u) : _valid(true), _val{ as_fwd<U>(u)... }
    {}
};

// option: ref
template<typename T>
class Option<T&>
{
public:
    using val_t = T&;

    bool  _valid;
    val_t _val;

    // ctor: 
    Option() noexcept: Option(immut_t<bool, false>())
    {}

    Option(Option&& other) noexcept : _valid(other._valid), _val(other._val) {
    }

    Option(const Option& other) noexcept: _valid(other._valid), _val(other._val) {
    }

    ~Option() {
    }

    // ctor: None
    static fn None() noexcept -> Option {
        return Option(immut_t<bool, false>());
    }

    // ctor: Some
    static fn Some(T& ref) noexcept -> Option {
        return Option(immut_t<bool, true>(), ref);
    }

    // method: is_some
    fn is_none() const noexcept -> bool {
        return _valid == false;
    }

    // method: is_some
    fn is_some() const noexcept -> bool {
        return _valid == true;
    }

#pragma region and
    template<class U>
    fn operator &(const Option<U>& def) noexcept -> Option<U> {
        if (!_valid) return Option<U>::None();
        return def;
    }

    template<class U>
    fn operator &(const Option<U>& def) const noexcept -> Option<U> {
        if (!_valid) return Option<U>::None();
        return def;
    }
#pragma endregion

#pragma region or
    fn operator |(const Option& def) noexcept -> Option {
        if (is_some()) return *this;
        return def;
    }

    fn operator |(const Option& def) const noexcept -> Option {
        if (is_some()) return *this;
        return def;
    }
#pragma endregion

#pragma region except
    // method[&const]: except
    template<typename ...U>
    fn except(const str& fmt, const U& ...u) -> T& {
        if (!is_some()) { ustd::panic(fmt, u...); }
        return _val;
    }

    // method[&const]: except
    template<typename ...U>
    fn except(const str& fmt, const U& ...u) const -> const T& {
        if (!is_some()) { ustd::panic(fmt, u...); }
        return _val;
    }
#pragma endregion

#pragma region unwrap
    fn unwrap() noexcept -> T& {
        return except("ustd::option::Option<{}>: expect `is_some` failed.", typeof<T>());
    }

    fn unwrap() const noexcept -> const T& {
        return except("ustd::option::Option<{}>: expect `is_some` failed.", typeof<T>());
    }
#pragma endregion

#pragma region and_then
    template<class F, class U = typename decltype(declval<ustd::val_t<F>>()(_val))::val_t>
    fn and_then(F&& f) -> Option<U> {
        if (is_none()) return Option<U>::None();
        return f(_val);
    }

    template<class F, class U = typename decltype(declval<ustd::val_t<F>>(_val))::val_t>
    fn and_then(F&& f) const {
        if (is_none()) return Option<U>::None();
        return f(_val);
    }
#pragma endregion

#pragma region map
    template<class F, class U=decltype(*declptr<ustd::val_t<F>>()(_val))>
    fn map(F&& f) -> Option<U> {
        if (!_valid) return Option<U>::None();
        return Option<U>::Some(f(_val));
    }

    template<class F, class U=decltype(*declptr<ustd::val_t<F>>()(_val))>
    fn map(F&& f) const -> Option<U> {
        if (!_valid) return Option<U>::None();
        return Option<U>::Some(f(_val));
    }
#pragma endregion

private:
    Option(immut_t<bool, false>) : _valid(false), _val(*declptr<T>())
    {}

    Option(immut_t<bool, true>, T& ref) : _valid(true), _val(ref)
    {}
};

template<class T>
fn operator==(const Option<T>& a, const Option<T>& b) noexcept -> bool {
    if (a._valid && b._valid) {
        return a._val == b._val;
    }
    return a._valid == b._valid;
}

template<class T>
fn operator!=(const Option<T>& a, const Option<T>& b) noexcept -> bool {
    if (a._valid && b._valid) {
        return a._val != b._val;
    }
    return a._valid != b._valid;
}

}

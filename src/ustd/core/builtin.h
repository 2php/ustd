#pragma once

#pragma clang diagnostic ignored "-Wc++17-extensions"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic ignored "-Wmicrosoft-union-member-reference"

/* attribute */
#ifdef __INTELLISENSE__
#   define __attribute__(...)
#endif

/* enable_if */
#define _if(...)  __attribute__((enable_if(__VA_ARGS__, "enable_if: failed")))

/* pub */
#ifndef pub
#   define pub __attribute__((visibility("default")))
#endif

/* mut */
#define mut auto

/* let */
#define let const auto

/* fn */
#define fn  auto

/* builtin */
#if defined(__INTELLISENSE__)
#   define ustd_builtin(f) ::f
#else
#   define ustd_builtin(f) __builtin_##f
#endif

fn operator new     (decltype(sizeof(int)), void*) -> void*;
fn operator new[]   (decltype(sizeof(int)), void*) -> void*;
fn operator delete  (void*, void*)                 -> void ;
fn operator delete[](void*, void*)                 -> void ;

namespace ustd
{

#ifdef __INTELLISENSE__
#define __INT8_TYPE__   __int8
#define __INT16_TYPE__  __int16
#define __INT32_TYPE__  __int32
#define __INT64_TYPE__  __int64

#define __UINT8_TYPE__  unsigned __int8
#define __UINT16_TYPE__ unsigned __int16
#define __UINT32_TYPE__ unsigned __int32
#define __UINT64_TYPE__ unsigned __int64
#endif

#pragma region types: primitive types
using i8 = __INT8_TYPE__;     using u8 = __UINT8_TYPE__;
using i16 = __INT16_TYPE__;    using u16 = __UINT16_TYPE__;
using i32 = __INT32_TYPE__;    using u32 = __UINT32_TYPE__;
using i64 = __INT64_TYPE__;    using u64 = __UINT64_TYPE__;

using f32 = float;
using f64 = double;

using byte = i8;
using llong = long long;

using ubyte = u8;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ullong = unsigned long long;

template<class ...U>
using void_t = void;

using cstr_t = const char*;

struct none_t
{
    none_t() noexcept = default;
};

template<typename T>
constexpr fn declptr() noexcept -> T* {
    return  static_cast<T*>(nullptr);
}

template<typename T>
constexpr fn declval() noexcept -> T& {
    return *static_cast<T*>(nullptr);
}

#pragma endregion

#pragma region when
template<bool X> struct when_t;
template<>       struct when_t<true> { using type = void; };
template<bool X> using  when = typename when_t<X>::type;
#pragma endregion

#pragma region $is_same
template<typename T, typename U> struct _is_same { static constexpr let $value = false; };
template<typename T            > struct _is_same<T, T> { static constexpr let $value = true; };

template<class T, class U>
constexpr static let $is_same = _is_same<T, U>::$value;
#pragma endregion

#pragma region $is_base
template<class B, class T>
constexpr static let $is_base = bool(__is_base_of(B, T));
#pragma endregion

#pragma region trival
template<class T>
struct trivial
{
    static constexpr let $copy = __has_trivial_copy(T);
    static constexpr let $dtor = __has_trivial_destructor(T);
    static constexpr let $assign = __has_trivial_assign(T);
};
#pragma endregion

#pragma region trait
template<class T>
struct trait
{
    static constexpr let $void = __is_void(T);
    static constexpr let $enum = __is_enum(T);
    static constexpr let $union = __is_union(T);
    static constexpr let $empty = __is_empty(T);
    static constexpr let $pod = __is_pod(T);
    static constexpr let $class = __is_class(T);
    static constexpr let $struct = __is_class(T) && trivial<T>::$copy;

    static constexpr let $object = __is_object(T);
    static constexpr let $const = __is_const(T);
    static constexpr let $volatile = __is_volatile(T);
    static constexpr let $ptr = __is_pointer(T);
    static constexpr let $ref = __is_lvalue_reference(T);
    static constexpr let $rref = __is_rvalue_reference(T);

    static constexpr let $copy = __has_nothrow_copy(T);
    static constexpr let $assign = __has_nothrow_assign(T);

    static constexpr let $uint = __is_unsigned(T);
    static constexpr let $sint = __is_signed(T);
    static constexpr let $int = __is_integral(T);
    static constexpr let $float = __is_floating_point(T);
    static constexpr let $num = __is_arithmetic(T);

    template<class U>
    static constexpr let $same = __is_same(T, U);

    template<class B>
    static constexpr let $base = __is_base_of(B, T);
};
#pragma endregion

#pragma region if_t

template<bool X, typename T, class F>
struct _if_t;

template<typename T, class F>
struct _if_t<true, T, F> { using type_t = T; };

template<typename T, class F>
struct _if_t<false, T, F> { using type_t = F; };

template<bool X, typename T, typename U>
using  if_t = typename _if_t<X, T, U>::type_t;

#pragma endregion

#pragma region select_t
template<typename ...T>
struct _select_t;

template<class T>
struct _select_t<T>
{
    template<u32 I> using type = typename _if_t<I == 0, T, void>::type_t;
};

template<class T, class ...U>
struct _select_t<T, U...>
{
    template<u32 I>
    using type = if_t<I == 0, T, typename _select_t<U...>::template type<I - 1> >;
};

template<u32 I, typename ...T>
using select_t = typename _select_t<T...>::template type<I>;
#pragma endregion

#pragma region types
template<typename T> struct _val_t { using U = T; };
template<typename T> struct _val_t<T&> { using U = T; };
template<typename T> struct _val_t<T&&> { using U = T; };

template<typename T> struct _ref_t { using U = T & ; };
template<typename T> struct _ref_t<T&> { using U = T & ; };

template<typename T> struct _rref_t { using U = T && ; };
template<typename T> struct _rref_t<T&> { using U = T && ; };
template<typename T> struct _rref_t<T&&> { using U = T && ; };

template<typename T> struct _mut_t { using U = T; };
template<typename T> struct _mut_t<const T> { using U = T; };

template<typename T> struct _const_t { using U = const T; };
template<typename T> struct _const_t<const T> { using U = const T; };

template<typename T> using val_t = typename _val_t<T>::U;
template<typename T> using ref_t = typename _ref_t<T>::U;
template<typename T> using mut_t = typename _mut_t<T>::U;
template<typename T> using const_t = typename _const_t<T>::U;
template<typename T> using rref_t = typename _rref_t<T>::U;
template<typename T> using cref_t = const_t<ref_t<T>>;

template<typename T> constexpr fn as_ref(T&         ref) noexcept -> T& { return ref; }
template<typename T> constexpr fn as_val(T&         ref) noexcept -> T { return ref; }
template<typename T> constexpr fn as_mov(T&         ref) noexcept->T&&      { return static_cast<T&&>(ref); }
template<typename T> constexpr fn as_fwd(val_t<T>&  ref) noexcept->T&&      { return static_cast<T&&>(ref); }
template<typename T> constexpr fn as_fwd(val_t<T>&& ref) noexcept->T&&      { return static_cast<T&&>(ref); }
template<typename T> constexpr fn as_mut(const T&   ref) noexcept -> T& { return const_cast<T&>(ref); }
template<typename T> constexpr fn as_mut(const T*   ptr) noexcept -> T* { return const_cast<T*>(ptr); }
template<typename T> constexpr fn as_const(const T&   ref) noexcept -> const T& { return ref; }
template<typename T> constexpr fn as_const(const T*   ptr) noexcept -> const T* { return ptr; }

#pragma endregion

#pragma region immut_t
template<class T, T ...x>
struct immut_t
{
    fn eq(const immut_t&) const noexcept -> bool {
        return true;
    }

    template<T ...y>
    fn eq(const immut_t<T, y...>&) const noexcept -> bool {
        return false;
    }
};

template<class T, T x>
struct immut_t<T, x>
{
    static constexpr let value = x;

    fn eq(const immut_t&) const noexcept -> bool {
        return true;
    }

    template<T y>
    fn eq(const immut_t<T, y>&) const noexcept -> bool {
        return false;
    }
};

#pragma endregion

#pragma region seq_t
template<u32 N> using seq_t = __make_integer_seq<immut_t, u32, N>;
#pragma endregion

#pragma region idx_t
template<u32 K, class I, bool ...V>
struct _idx_t;

template<u32 K, u32   ...I           > struct _idx_t<K, immut_t<u32, I...>             > { using type = immut_t<u32, I...>; };
template<u32 K, u32   ...I, bool ...V> struct _idx_t<K, immut_t<u32, I...>, true, V...> { using type = typename _idx_t<K + 1, immut_t<u32, I..., K>, V...>::type; };
template<u32 K, u32   ...I, bool ...V> struct _idx_t<K, immut_t<u32, I...>, false, V...> { using type = typename _idx_t<K + 1, immut_t<u32, I...   >, V...>::type; };

template<bool ...V>
using idx_t = typename _idx_t<0, immut_t<u32>, V...>::type;

#pragma endregion

#pragma region int
inline fn to_uint(i8  val) -> u8 { return static_cast<u8> (val); }
inline fn to_uint(i16 val) -> u16 { return static_cast<u16>(val); }
inline fn to_uint(i32 val) -> u32 { return static_cast<u32>(val); }
inline fn to_uint(i64 val) -> u64 { return static_cast<u64>(val); }

inline fn to_uint(u8  val) -> u8 { return val; }
inline fn to_uint(u16 val) -> u16 { return val; }
inline fn to_uint(u32 val) -> u32 { return val; }
inline fn to_uint(u64 val) -> u64 { return val; }

inline fn to_sint(i8  val) -> i8 { return val; }
inline fn to_sint(i16 val) -> i16 { return val; }
inline fn to_sint(i32 val) -> i32 { return val; }
inline fn to_sint(i64 val) -> i64 { return val; }

inline fn to_sint(u8  val) -> i8 { return static_cast<i8>(val); }
inline fn to_sint(u16 val) -> i16 { return static_cast<i16>(val); }
inline fn to_sint(u32 val) -> i32 { return static_cast<i32>(val); }
inline fn to_sint(u64 val) -> i64 { return static_cast<i64>(val); }

#pragma endregion

#pragma region ver_t
template<u32 I> struct ver_t;
template<>      struct ver_t<0> {};
template<u32 I> struct ver_t : ver_t<I - 1> {};
#pragma endregion

#pragma region ctor/dtor
template<class T, class ...U>
fn ctor(T* ptr, U&& ...u) -> T& {
    new(ptr)T(as_fwd<U>(u)...);
    return *ptr;
}

template<class T>
fn dtor(T* obj) -> void {
    if constexpr(!trivial<T>::$dtor) {
        obj->~T();
    }
}
#pragma endregion

#pragma region swap
template<class T>
fn _swap(T& a, T& b) noexcept -> void {
    union U {
        none_t  _;
        T       c;

        U()  noexcept : _() {}
        ~U() noexcept {}
    };
    U u;

    mut& c = u.c;
    new(&c)T(as_mov(a));      // c <== a
    new(&a)T(as_mov(b));      // a <== b
    new(&b)T(as_mov(c));      // a <== b
}

template<class T>
fn swap(T& a, T& b) noexcept -> void {
    _swap(a, b);
}

template<class T>
fn swap(T& a, T&& b) noexcept -> void {
    _swap(a, b);
}
#pragma endregion

#pragma region scope_exit
struct _scope_exit_t
{
    using fn_t = void();
    fn_t* _fn;

    explicit _scope_exit_t(fn_t fun): _fn(fun)
    {}

    ~_scope_exit_t() {
        (*_fn)();
    }
};

inline fn scope_exit(_scope_exit_t::fn_t fun) -> _scope_exit_t {
    return _scope_exit_t(fun);
}
#pragma endregion

}


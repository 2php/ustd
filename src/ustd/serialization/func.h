#pragma once

#include "ustd/serialization/dom.h"

namespace ustd::serialization
{

constexpr inline fn _is_str(str*) noexcept -> bool {
    return true;
}

constexpr inline fn _is_str(StrView*) noexcept -> bool {
    return true;
}

constexpr inline fn _is_str(...) noexcept -> bool {
    return false;
}

template<class T, u32 N>
constexpr inline fn _is_array(FixedList<T,N>*) noexcept -> bool {
    return true;
}

constexpr inline fn _is_array(...) noexcept -> bool {
    return false;
}

template<class T, typename = decltype(T::$property_cnt)>
constexpr inline fn _is_object(const T*) noexcept -> bool {
    return true;
}

constexpr inline fn _is_object(...) noexcept -> bool {
    return false;
}

template<class T>
fn encode(Dom& dom, const T& val) noexcept -> Result<void> {
    // bool
    if constexpr($is_same<T, bool>) {
        dom.set_node(Node::from_bool(val));
    }
    // number
    else if constexpr(trait<T>::$num) {
        dom.set_node(Node::from_num(val));
    }
    // time
    else if constexpr($is_same<T, time::SystemTime>) {
        dom.set_node(Node::from_time(val));
    }
    // enum
    else if constexpr(trait<T>::$enum) {
        let s = to_str(val);
        dom.set_node(Node::from_str(s));
    }
    // str
    else if constexpr(_is_str(declptr<T>())) {
        dom.set_node(Node::from_str(val));
    }
    // list
    else if constexpr(_is_array(declptr<T>())) {
        dom.set_node(Node::from_array());

        let cnt  = val.len;

        mut prev = dom;
        mut node = dom;
        for(mut i = 0u; i < cnt; ++i) {
            node = dom.arr_add_element(prev, Node::from_null());
            encode(node, val[i]);
            prev = node;
        }
    }
    // object
    else if constexpr(_is_object(declptr<T>())) {
        dom.set_node(Node::from_object());

        mut prev_key = dom;

#define ustd_encode_property(idx)                                                   \
        if constexpr(T::$property_cnt > idx) {                                      \
            let kv  = val.get_property(immut_t<u32, idx>());                        \
            mut res = dom.obj_add_key_val(prev_key, kv.key, Node::from_null());     \
            encode(res.$1, kv.val);                                                 \
            prev_key = res.$0;                                                      \
        }

        ustd_encode_property( 0);
        ustd_encode_property( 1);
        ustd_encode_property( 2);
        ustd_encode_property( 3);
        ustd_encode_property( 4);
        ustd_encode_property( 5);
        ustd_encode_property( 6);
        ustd_encode_property( 7);
        ustd_encode_property( 8);
        ustd_encode_property( 9);
        ustd_encode_property(10);
        ustd_encode_property(11);
        ustd_encode_property(12);
        ustd_encode_property(13);
        ustd_encode_property(14);
        ustd_encode_property(15);
        ustd_encode_property(16);
        ustd_encode_property(17);
        ustd_encode_property(18);
        ustd_encode_property(19);
        ustd_encode_property(20);
        ustd_encode_property(21);
        ustd_encode_property(22);
        ustd_encode_property(23);
        ustd_encode_property(24);
        ustd_encode_property(25);
        ustd_encode_property(26);
        ustd_encode_property(27);
        ustd_encode_property(28);
        ustd_encode_property(29);
        ustd_encode_property(30);
        ustd_encode_property(31);
        ustd_encode_property(32);
        ustd_encode_property(33);
        ustd_encode_property(34);
        ustd_encode_property(35);
        ustd_encode_property(36);
        ustd_encode_property(37);
        ustd_encode_property(38);
        ustd_encode_property(39);
#undef ustd_encode_property
    }
    else {
        return Result<void>::Err(Error::UnexpectType);
    }
    return Result<void>::Ok();
}

template<class T>
fn decode(Dom& dom, T& val) noexcept -> Result<void> {
    // bool, num, str, enum, time
    if constexpr($is_same<T, bool> || trait<T>::$num || trait<T>::$enum || $is_same<T, time::SystemTime>) {
        let res = dom.as<T>();
        if (res.is_err()) {
            return Result<void>::Err(res._err);
        }
        val = res._ok;
    }
    if constexpr(_is_array(declptr<T>())) {
        u32 idx = 0;
        for(mut element: dom.into_iter()) {
            decode(element, val[idx++]);
        }
    }
    if constexpr(_is_object(declptr<T>())) {
#define ustd_encode_property(idx)                                                   \
        if constexpr(T::$property_cnt > idx) {                                      \
            let kv          = val.get_property(immut_t<u32, idx>());                \
            mut element_opt = dom[kv.key];                                          \
            if (element_opt.is_err()) {                                             \
                return Result<void>::Err(element_opt._err);                         \
            }                                                                       \
            decode(element_opt._ok, kv.val);                                        \
        }

        ustd_encode_property(0);
        ustd_encode_property(1);
        ustd_encode_property(2);
        ustd_encode_property(3);
        ustd_encode_property(4);
        ustd_encode_property(5);
        ustd_encode_property(6);
        ustd_encode_property(7);
        ustd_encode_property(8);
        ustd_encode_property(9);
        ustd_encode_property(10);
        ustd_encode_property(11);
        ustd_encode_property(12);
        ustd_encode_property(13);
        ustd_encode_property(14);
        ustd_encode_property(15);
        ustd_encode_property(16);
        ustd_encode_property(17);
        ustd_encode_property(18);
        ustd_encode_property(19);
        ustd_encode_property(20);
        ustd_encode_property(21);
        ustd_encode_property(22);
        ustd_encode_property(23);
        ustd_encode_property(24);
        ustd_encode_property(25);
        ustd_encode_property(26);
        ustd_encode_property(27);
        ustd_encode_property(28);
        ustd_encode_property(29);
        ustd_encode_property(30);
        ustd_encode_property(31);
        ustd_encode_property(32);
        ustd_encode_property(33);
        ustd_encode_property(34);
        ustd_encode_property(35);
        ustd_encode_property(36);
        ustd_encode_property(37);
        ustd_encode_property(38);
        ustd_encode_property(39);
#undef ustd_encode_property
    }
    else {
        return Result<void>::Err(Error::UnexpectType);
    }
    return Result<void>::Ok();
}

}


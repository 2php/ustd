#pragma once

#include "ustd/core.h"

namespace ustd::test
{

template<class T, class U>
fn compare(const T& a, const U& b) -> bool {
    if constexpr(trait<T>::$float && trait<U>::$float) {
        let base = f64((ustd::abs(a) + ustd::abs(b)) / 2);
        let diff = f64(ustd::abs(a - b));
        let fpc  = fpclassify(base);
        if (fpc == FP::$zero)   return true;
        if (fpc != FP::$normal) return false;
        return diff / base < 1e-6;
    }
    else {
        return bool(a == b);
    }
}

template<class T, class U>
fn assert_eq(const T& val, const U& expect) -> void {
    if (compare(val, expect)) return;
    ustd::panic("ustd::test::assert_eq(val={}:{}, expect={}:{}): failed", typeof<T>(), val, typeof<U>(), expect);
}

template<class T>
fn assert_eq(const T& val, const T& expect) -> void {
    if (compare(val, expect)) return;
    ustd::panic("ustd::test::assert_eq(val={}:{}, expect={}:{}): failed", typeof<T>(), val, typeof<T>(), expect);
}

template<class T, class U>
fn assert_ne(const T& val, const U& expect) -> void {
    if (!compare(val, expect)) return; 
    ustd::panic("ustd::test::assert_ne(val={}:{}, expect={}:{}): failed", typeof<T>(), val, typeof<U>(), expect);
}

template<class T>
fn assert_ne(const T& val, const T& expect) -> void {
    if (!compare(val, expect)) return;
    ustd::panic("ustd::test::assert_ne(val={}:{}, expect={}:{}): failed", typeof<T>(), val, typeof<T>(), expect);
}

}

namespace ustd
{
using test::assert_eq;
using test::assert_ne;
}

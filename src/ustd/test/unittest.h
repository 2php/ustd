#pragma once

#include "ustd/test/scheduler.h"

#define ustd_test_func ustd_test_make1(_test_func, __LINE__)
#define ustd_test_type ustd_test_make1(_test_type, __LINE__)
#define ustd_test_init ustd_test_make1(_test_init, __LINE__)

#define ustd_test_make1(name, line) ustd_test_make2((name, line))
#define ustd_test_make2(...)        ustd_test_make3 __VA_ARGS__
#define ustd_test_make3(name, line) name##line

#ifdef __INTELLISENSE__
#define unittest(...)   static fn ustd_test_func() -> void
#endif

#ifndef ustd_test
#define ustd_test(...)                                                                  \
    static fn ustd_test_func() -> void;                                                 \
    struct ustd_test_type {};                                                           \
    static __attribute__((constructor)) fn ustd_test_init() -> void {                   \
        ustd::test::scheduler().install<ustd_test_type>(#__VA_ARGS__, &ustd_test_func); \
    }                                                                                   \
    static fn ustd_test_func() -> void
#endif

#define unittest(...) ustd_test(__VA_ARGS__)

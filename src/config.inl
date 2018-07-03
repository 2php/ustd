#ifndef  USTD_CONFIG_INL
#define  USTD_CONFIG_INL

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN10_1607

// windows:ucrt
#if __has_include(<corecrt.h>)
#   include <corecrt.h>
#   include <corecrt_io.h>
#endif

// unix: standard
#if __has_include(<unistd.h>)
#include <unistd.h>
#endif

// dl: shared object
#if __has_include(<dlfcn.h>)
#   include <dlfcn.h>
#endif

// ioctl
#if __has_include(<sys/ioctl.h>)
#   include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <math.h>
#include <sys/stat.h>

#undef stdin    // 0
#undef stdout   // 1
#undef stderr   // 2

#ifdef _UCRT    // windows ucrt
enum {
    STDIN_FILENO    = 0,
    STDOUT_FILENO   = 1,
    STDERR_FILENO   = 2
};
#endif

#if defined(_MSC_VER)
#   define pub __declspec(dllexport)
#elif defined(__clang__)
#   define pub __attribute__((visibility("default")))
#endif

#define USTD_IMPL
#include "ustd.h"
using ustd::u16;  using ustd::i16;
using ustd::u32;  using ustd::i32;
using ustd::u64;  using ustd::i64;
using ustd::f32;  using ustd::f64;

using ustd::isize;
using ustd::usize;

#endif

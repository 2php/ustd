#pragma once

#if defined(__CUDACC__) && !defined(__INTELLISENSE__)
#   define USTD_CUDA_KERNEL
#endif

#ifndef USTD_CUDA_KERNEL
#   include "ustd/core.h"
#   include "ustd/cuda/api.h"
#   include "ustd/cuda/array.h"
#   include "ustd/cuda/tex.h"
#endif

#ifdef USTD_CUDA_KERNEL
#   include "ustd/cuda/kernel.h"
#endif

#ifdef __INTELLISENSE__
#   define  __device__  static
#   define  __global__  static
#   define cu_run(fun, ...) fun
#endif

#ifndef __INTELLISENSE__
#   define cu_run(fun, ...) fun<<<__VA_ARGS__>>>
#endif

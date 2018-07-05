#pragma once

#include "ustd/core.h"
#include "ustd/math/ndslice.h"

namespace ustd::math
{

template<typename T, u32 N = 1>
class NDArray : public NDSlice<T, N>
{
    using base   = NDSlice<T, N>;
    using dims_t = typename base::dims_t;

public:
    NDArray(const dims_t& dims) noexcept : base(nullptr, dims) {
        u64 dims_arr[N];
        for(mut i = 0u; i < N; ++i) {
            dims_arr[i] = dims[i];
        }
        base::_data = ustd::mnew<T>(dims_arr);
    }

    NDArray(const u32(&dims)[N]) noexcept: NDArray(dims_t(dims))
    {}

    NDArray(NDArray&& other) noexcept : base{ other } {
        other._data = nullptr;
    }

    ~NDArray() noexcept {
        if (base::_data == nullptr) return;
        ustd::mdel(base::_data);
    }
};

template<typename T>
class NDArray<T, 0>: public NDSlice<T, 0>
{
    using base = NDSlice<T, 0>;

public:
    NDArray(const T& val=T(0)) noexcept : base(val)
    {}
};

}


#pragma once

#include "ustd/math/ndarray.h"
#include "ustd/cuda/api.h"

namespace ustd::cuda
{

template<typename T, u32 N = 1>
using NDSlice = math::NDSlice<T, N>;

template<typename T, u32 N = 1>
class NDArray : public NDSlice<T, N>
{
    using base = NDSlice<T, N>;

    using dims_t = typename base::dims_t;

public:
    explicit NDArray(dims_t dims): base(nullptr, dims) {
        let cnt = base::count();
        let ptr = cuda::dnew<T>(cnt);
        if (ptr.is_ok()) {
            base::_data = ptr._ok;
        }
    }

    explicit NDArray(const u32(&dims)[N]): NDArray(dims_t(dims))
    {}

    explicit NDArray(math::NDSlice<T, N> src): NDArray(src.dims) {
        let cnt = base::count();
        cuda::mcpy(base::_data, src._data, cnt);
    }

    ~NDArray() {
        if (base::_data == nullptr) {
            return;
        }
        cuda::ddel(base::_data);
    }

    NDArray(NDArray&& other) : base(other) {
        other._data = nullptr;
    }
};

template<class T> using Arr1d = NDArray<T, 1>;
template<class T> using Arr2d = NDArray<T, 2>;
template<class T> using Arr3d = NDArray<T, 3>;

template<typename T, u32 N>
fn operator <<=(cuda::NDArray<T,N>& dst, math::NDArray<T,N>& src) -> cuda::NDArray<T, N>& {
    let cnt = dst.count();
    cuda::mcpy(dst._data, src._data, cnt);
    return dst;
}

template<typename T, u32 N>
fn operator <<=(math::NDArray<T, N>& dst, cuda::NDArray<T, N>& src) -> math::NDArray<T, N>& {
    let cnt = dst.count();
    cuda::mcpy(dst._data, src._data, cnt);
    return dst;
}

}

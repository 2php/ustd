#pragma once

#include "ustd/math/ndarray.h"
#include "ustd/cuda/api.h"

namespace ustd::cuda
{

template<class T, u32 N>
using NDSlice = math::NDSlice<T, N>;

template<class T, u32 N> 
class NDArray;

template<class T, u32 N>
fn operator <<=(cuda::NDArray<T, N>& dst, math::NDArray<T, N>& src) -> cuda::NDArray<T, N>&;

template<class T, u32 N>
fn operator <<=(math::NDArray<T, N>& dst, cuda::NDArray<T, N>& src)->math::NDArray<T, N>&;

template<class T, u32 N = 1>
class NDArray : public NDSlice<T, N>
{
public:
    using base      = NDSlice<T, N>;
    using dims_t    = typename base::dims_t;

    NDArray(NDArray&& other) : base(other) {
        other._data = nullptr;
    }

    ~NDArray() {
        if (base::_data == nullptr) {
            return;
        }
        cuda::ddel(base::_data);
    }

    static fn with_dims(const dims_t& dims) -> NDArray {
        return NDArray(dims);
    }

    static fn from_array(math::NDArray<T, N>& src) {
        mut res = with_dims(src.dims());
        res <<= src;
        return as_mov(res);
    }

    fn resize(const dims_t& dims) -> NDArray& {
        mut tmp = NDArray(dims);
        ustd::swap(*this, tmp);
        return *this;
    }

    fn clear() -> void {
        resize({ 0 });
    }
protected:
    explicit NDArray(dims_t dims) : base(nullptr, dims) {
        let cnt = base::count();

        if (cnt != 0) {
            base::_data = cuda::dnew<T>(dims);
        }
    }
};

template<class T> using Arr1d = NDArray<T, 1>;
template<class T> using Arr2d = NDArray<T, 2>;
template<class T> using Arr3d = NDArray<T, 3>;

template<typename T, u32 N>
fn operator <<=(cuda::NDArray<T,N>& dst, math::NDArray<T,N>& src) -> cuda::NDArray<T, N>& {
    let dims = dst._dims;
    cuda::mcpy(dst._data, src._data, dims);
    return dst;
}

template<typename T, u32 N>
fn operator <<=(math::NDArray<T, N>& dst, cuda::NDArray<T, N>& src) -> math::NDArray<T, N>& {
    let dims = dst._dims;
    cuda::mcpy(dst._data, src._data, dims);
    return dst;
}

}

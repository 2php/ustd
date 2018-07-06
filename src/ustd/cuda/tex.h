#pragma once

#include "ustd/cuda/api.h"
#include "ustd/cuda/ndarray.h"

namespace ustd::cuda
{

template<class T, u32 N>
class Tex
{
public:
    using dims_t = vec<u32, N>;

    tex_t  _tex  = tex_t(0);
    arr_t  _arr  = arr_t(0);
    dims_t _dims;

    explicit Tex(dims_t dims, TexAddress border_mode = TexAddress::Clamp, TexFilter filter_mode = TexFilter::Point) : _dims(dims) {
        _arr = cuda::anew<T>(_dims);
        _tex = cuda::tnew<T>(_arr, border_mode, filter_mode);
    }

    explicit Tex(const u32(&dim)[N], TexAddress border_mode, TexFilter filter_mode = TexFilter::Point)
        : Tex(dim_t(dim), border_mode, filter_mode)
    {}

    explicit Tex(math::NDSlice<T,N> src): Tex(src.dims) {
        this->set(src);
    }

    ~Tex() {
        if (_arr == arr_t(0)) {
            return;
        }

        cuda::tdel<T>(_tex);
        cuda::adel<T>(_arr);
    }

    fn set(NDSlice<T,N> v) noexcept -> void {
        cuda::acpy(_arr, v._data, v._dims);
    }

    fn get(NDSlice<T,N> v) noexcept -> void {
        cuda::acpy(v._data, _arr, v._dims);
    }
};

template<class T> using Tex1d = cuda::Tex<T, 1>;
template<class T> using Tex2d = cuda::Tex<T, 2>;
template<class T> using Tex3d = cuda::Tex<T, 3>;

}
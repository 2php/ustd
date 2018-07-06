#pragma once

#include "ustd/core.h"
#include "ustd/math/ndslice.h"

namespace ustd::math
{

template<typename T, u32 N = 1>
class NDArray
    : public NDSlice<T, N>
{
public:
    using base   = NDSlice<T, N>;
    using dims_t = typename base::dims_t;

    NDArray(NDArray&& other) noexcept : base(other) {
        other._data = nullptr;
    }

    ~NDArray() noexcept {
        if (base::_data == nullptr) {
            return;
        }
        ustd::mdel(base::_data);
    }

    static fn with_dims(const dims_t& dims) noexcept -> NDArray {
        return NDArray(dims);
    }

    fn resize(const dims_t& dims) -> NDArray& {
        mut tmp = NDArray(dims);
        ustd::swap(*this, tmp);
        return *this;
    }

    fn clear() -> void {
        resize({ 0 });
        return *this;
    }

protected:
    NDArray(const dims_t& dims) noexcept : base(nullptr, dims) {
        let cnt = base::count();
        if (cnt != 0) {
            base::_data = ustd::mnew<T>(dims);
        }
    }
};


}


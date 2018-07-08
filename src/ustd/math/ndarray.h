#pragma once

#include "ustd/core.h"
#include "ustd/math/ndslice.h"

namespace ustd::math
{

template<typename T, u32 N = 1>
class NDArray: public NDSlice<T, N>
{
public:
    using base   = NDSlice<T, N>;
    using u32xN  = typename base::u32xN;

    NDArray(NDArray&& other) noexcept : base(other) {
        other._data = nullptr;
    }

    ~NDArray() noexcept {
        if (base::_data == nullptr) {
            return;
        }
        ustd::mdel(base::_data);
    }

    static fn with_dims(const u32xN& dims) noexcept -> NDArray {
        return NDArray(dims);
    }

    fn resize(const u32xN& dims) -> NDArray& {
        mut tmp = NDArray(dims);
        ustd::swap(*this, tmp);
        return *this;
    }

    fn clear() -> void {
        resize({ 0 });
        return *this;
    }

protected:
    explicit NDArray(const u32xN& dims) noexcept : base(nullptr, dims) {
        let cnt = base::count();
        if (cnt != 0) {
            base::_data = ustd::mnew<T>(dims);
        }
    }
};


}


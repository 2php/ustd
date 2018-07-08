
#include "ustd/math/types.h"

namespace ustd::math
{

template<typename T, u32 N>
struct vline_t: ndvec_t<T,N>
{
    using vec_t = vline_t;

    T _step[N];

    template<class ...U>
    explicit vline_t(const U& ...u): _step{u...}
    {}

    // property[r]: dims
    fn dims(u32) const noexcept -> u32 {
        return u32(0);
    }

    template<typename ...I, class=when<sizeof...(I)==N> >
    fn operator()(I ...idxs) const -> T {
        return at(seq_t<N>{}, idxs...);
    }

    template<u32 ...K, typename ...I>
    fn at(immut_t<u32, K...>, I ...idxs) const -> T {
        return ustd::sum((T(_step[K])*idxs)...);
    }
};

template<typename T, typename ...U>
constexpr fn vline(const T& t, const U& ...u) {
    return vline_t<T, 1+sizeof...(U)>(t, u... );
}

}

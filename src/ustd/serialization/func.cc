#include "config.inl"

namespace ustd::serialization
{


struct Study
{
    ustd_property_begin;
    typedef str ustd_property(name);
    typedef u32 ustd_property(age);
    ustd_property_end;
};

unittest(encode) {
    let study = Study{"lumpy", 30};
    mut tree  = Tree::with_capacity(20);
    encode(tree, study);

    log::info("study = {}", tree);
}

}
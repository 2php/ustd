#include "config.inl"

namespace ustd
{

struct TCopy
{
    i32 _id;

    TCopy(i32           id)    : _id(id)        {}
    TCopy(const TCopy&  other) : _id(other._id) {}
};

struct TMove
{
    i32 _id;

    TMove(i32       id)     : _id(id) {}
    TMove(TMove&&   other)  : _id(other._id) { --other._id; }
};

unittest(Option)
{
    // copy
    if (true) {
        mut val = TCopy{ 1 };
        mut opt = Option<TCopy>::Some(val);
        assert_eq(val._id,    1);
        assert_eq(opt._val._id, 1);
    }

    // move
    if (true) {
        mut val = TMove{ 1 };
        mut opt = Option<TMove>::Some(as_mov(val));
        assert_eq(val._id,      0);
        assert_eq(opt._val._id, 1);
    }
}

}


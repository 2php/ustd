#include "config.inl"

namespace ustd
{

unittest(Deque) {

    // test push/pop
    mut q = List<i32>::with_capacity(32).as_deque();

    q.push_front(-1);
    q.push_front(-2);
    q.push_front(-3);

    q.push_back(+1);
    q.push_back(+2);
    q.push_back(+3);

    assert_eq(q.pop_front(), Option<i32>::Some(-3));
    assert_eq(q.pop_front(), Option<i32>::Some(-2));
    assert_eq(q.pop_front(), Option<i32>::Some(-1));

    assert_eq(q.pop_back(), Option<i32>::Some(+3));
    assert_eq(q.pop_back(), Option<i32>::Some(+2));
    assert_eq(q.pop_back(), Option<i32>::Some(+1));

    assert_eq(q.pop_front(), Option<i32>::None());
    assert_eq(q.pop_back(),  Option<i32>::None());
}

unittest(Heap){
    mut h = List<i32>::with_capacity(32).as_heap();

    for(mut i = 0; i < 10; ++i) {
        let k = i * 13 % 10;
        h.push(k);
    }

    for(mut i = 0; i < 10; ++i){
        let v = h.pop();
        assert_eq(v, Option<i32>::Some(i));
    }
}

}

#pragma once

#include "ustd/core/slice.h"
#include "ustd/core/mem.h"
#include "ustd/core/ops.h"

namespace ustd
{

template<typename T> class List;

using String = List<char>;

template<typename T, typename List> class Deque;
template<typename T, typename List> class Queue;
template<typename T, typename List> class Stack;
template<typename T, typename List> class Heap;
template<typename T, typename List> class RingBuffer;

template<typename T>
class List : public Slice<T>
{
public:
    using base   = Slice<T>;

#pragma region ctor/dtor
    // ctor: default
    List() noexcept: base{}
    {}

    // ctor[move]:
    List(List&& other) noexcept : base(other) {
        other._data = nullptr;
        other._size = 0;
    }

    // dtor
    ~List() noexcept {
        if (base::_data == nullptr) return;

        if constexpr (!trivial<T>::$dtor) {
            for(mut i = 0u; i < base::_size; ++i) {
                mut& t = base::_data[i];
                t.~T();
            }
        }

        mdel(base::_data);
    }

    // ctor: with_capacity
    static fn with_capacity(u32 capacity) noexcept -> List {
        let data = mnew<T>(capacity);
        return List{ data, 0u, capacity };
   }
#pragma endregion

#pragma region methods
    fn reserve(u32 new_capacity) noexcept {
        let old_capacity = base::_capacity;
        if (new_capacity <= old_capacity) {
            return;
        }

        let old_data = base::_data;
        let new_data = mnew<T>(new_capacity);
        mmov(old_data, new_data, base::_size);

        mdel(old_data);
        base::_data     = new_data;
        base::_capacity = new_capacity;
    }

    fn grow(u32 count) noexcept {
        let old_capacity = base::_capacity;
        let min_capacity = old_capacity + count;
        let max_capacity = (old_capacity + old_capacity / 4 + 63) & (~63u);
        let new_capacity = ustd::max(min_capacity, max_capacity);
        reserve(new_capacity);
    }
#pragma endregion

#pragma region push/pop
    // method: push
    template<class ...U>
    fn push(U&& ...u) noexcept -> Option<List&> {
        if (base::_size + 1 > base::_capacity) {
            grow(1);
        }
        base::_push(as_fwd<U>(u)...);
        return Option<List&>::Some(*this);
    }

    // method: push
    template<class ...U>
    fn pushn(u32 n, U&& ...u) noexcept -> Option<List&> {
        if (base::_size + n > base::_capacity) {
            grow(n);
        }

        base::_pushn(n, as_fwd<U>(u)...);
        return Option<List&>::Some(*this);
    }

    // method: push_slice
    template<class U>
    fn push_slice(Slice<U> v) noexcept -> Option<List&> {
        if (base::_size + v._size > base::_capacity) {
            grow(v._size);
        }
        base::_push_slice(v);
        return Option<List&>::Some(*this);
    }

    // method: pop
    fn pop() noexcept -> Option<T> {
        return base::_pop();
    }
#pragma endregion

#pragma region as_...
    fn as_deque() && ->Deque<T, List> {
        return Deque<T, List>::from_list(as_mov(*this));
    }

    fn as_qeque() && ->Queue<T, List> {
        return Queue<T, List>::from_list(as_mov(*this));
    }

    fn as_stack() && ->Stack<T, List> {
        return Stack<T, List>::from_list(as_mov(*this));
    }

    fn as_heap() && -> Heap<T, List> {
        return Heap<T, List>::from_list(as_mov(*this));
    }

    fn as_ringbuffer() && ->RingBuffer<T, List> {
        return RingBuffer<T, List>::from_list(as_mov(*this));
    }
#pragma endregion

protected:
    // ctor:
    List(T* data, u32 length, u32 capacity) noexcept
        : base(data, length, capacity)
    {}
};

template<typename T, u32 N>
class FixedList: public List<T>
{
public:
    using base = List<T>;

    union
    {
        u8  _nul;
        T   _buf[N];
    };

#pragma region ctor/dtor
    // ctor:
    FixedList() noexcept : base(_buf, 0, N)
    {}

    // dtor:
    ~FixedList() noexcept {
        base::clear();
        base::_data = nullptr;
    }

    // ctor[move]
    FixedList(FixedList&& other) noexcept: base(_buf, other._size, N) {
        other._size = 0;

        let dst = _buf;
        let src = other._buf;
        let cnt = base::_size;
        ustd::mmov(dst, src, cnt);
    }
#pragma endregion

#pragma region as_...
    fn as_deque() && ->Deque<T, FixedList> {
        return Deque<T, FixedList>::from_list(as_mov(*this));
    }

    fn as_qeque() && ->Queue<T, FixedList> {
        return Queue<T, FixedList>::from_list(as_mov(*this));
    }

    fn as_stack() && ->Stack<T, FixedList> {
        return Stack<T, FixedList>::from_list(as_mov(*this));
    }

    fn as_heap() && ->Heap<T, FixedList> {
        return Heap<T, FixedList>::from_list(as_mov(*this));
    }

    fn as_ringbuffer() && -> RingBuffer<T, FixedList> {
        return RingBuffer<T, FixedList>::from_list(as_mov(*this));
    }
#pragma endregion
};

template<class T, class List = List<T> >
class Deque {
public:
    List    _list;
    u32     _head;
    u32     _tail;

    enum class Opt {
        None,
        OverWrite
    };

    Deque(Deque&& other) noexcept
        : _list(as_mov(other._list))
        , _head(other._head)
        , _tail(other._tail)
    {}

    ~Deque() {
        clear();
    }

    template<class ...U>
    static fn from_list(U&& ...u)noexcept -> Deque {
        return Deque(as_fwd<U>(u)...);
    }

#pragma region property
    fn capacity() const noexcept  -> u32 {
        return _list.capacity();
    }

    fn is_empty() const noexcept -> bool {
        return _list._size == 0;
    }

    fn is_full() const noexcept -> bool {
        return _list._size == _list._capacity;
    }
#pragma endregion

#pragma region method
    fn clear() noexcept -> void {
        if (is_empty()) return;

        _list._size = 0;

        if (!trivial<T>::$dtor) {
            let tail = _tail;
            mut idx = _head;
            while (true) {
                ustd::dtor(&_list[idx]);
                if (idx == tail) break;
                idx = (idx + 1) % _list._capacity;
            }
        }
    }

    fn front() noexcept -> Option<T&> {
        if (is_empty()) return Option<T&>::None();
        return Option<T&>(_list[_head]);
    }

    fn front() const noexcept -> Option<const T&> {
        if (is_empty()) return Option<const T&>::None();
        return Option<const T&>(_list[_head]);
    }

    fn back() noexcept -> Option<T&> {
        if (is_empty()) return Option<T&>::None();
        return Option<T&>(_list[_tail]);
    }

    fn back() const noexcept -> Option<const T&> {
        if (is_empty()) return Option<const T&>::None();
        return Option<const T&>(_list[_tail]);
    }

    template<typename ...U>
    fn push_front(U&& ...u) noexcept -> Option<Deque&> {
        return _push_front<Opt::None>(as_fwd<U>(u)...);
    }

    template<typename ...U>
    fn push_back(U&& ...u) noexcept -> Option<Deque&> {
        return _push_back<Opt::None>(as_fwd<U>(u)...);
    }

    fn pop_front() noexcept -> Option<T> {
        return _pop_front();
    }

    fn pop_back() noexcept -> Option<T> {
        return _pop_back();
    }
#pragma endregion

protected:
    // | ...xxxxxxxx...|       | xxx.......xxxxx|
    // | ...^......^...|       | ..^.......^... |
    // |    |      |   |       |   |       |    |
    // |  head    tail |       |  tail   head   |
    template<class ...U>
    Deque(U&& ...u) noexcept : _list(as_fwd<U>(u)...), _head(0), _tail(_list._capacity - 1)
    {}

#pragma region push/pop
    template<Opt opt, typename ...U>
    fn _push_front(U&& ...val) noexcept -> Option<Deque&> {
        if (is_full()) {
            if constexpr(opt == Opt::OverWrite) {
                _pop_back();
            }
            else {
                return Option<Deque&>::None();
            }
        }

        ++_list._size;
        _head = (_head - 1 + _list._capacity) % _list._capacity;

        ustd::ctor(&_list[_head], as_fwd<U>(val)...);
        return Option<Deque&>::Some(*this);
    }

    fn _pop_front() noexcept -> Option<T> {
        if (is_empty()) {
            return Option<T>::None();
        }

        mut& res = _list[_head];

        --_list._size;
        _head = (_head + 1) % _list._capacity;

        return Option<T>::Some(as_mov(res));
    }

    template<Opt opt, typename ...U>
    fn _push_back(U&& ...val) noexcept -> Option<Deque&> {
        if (is_full()) {
            if constexpr (opt == Opt::OverWrite) {
                _pop_front();
            }
            else {
                return Option<Deque&>::None();
            }
        }

        ++_list._size;
        _tail = (_tail + 1) % _list._capacity;
        ustd::ctor(&_list[_tail], as_fwd<U>(val)...);
        return Option<Deque&>::Some(*this);
    }

    fn _pop_back() noexcept -> Option<T> {
        if (is_empty()) {
            return Option<T>::None();
        }

        mut res = _list[_tail];

        --_list._size;
        _tail = (_tail - 1 + _list._capacity) % _list._capacity;

        return Option<T>::Some(as_mov(res));
    }
#pragma endregion

};

template<class T, class List = List<T> >
class Queue: Deque<T, List>
{
public:
    using base = Deque<T, List>;

    Queue(Queue&& other) noexcept : base(static_cast<base&&>(other))
    {}

    template<class ...U>
    static fn from_list(U&& ...u) noexcept -> Queue {
        return Queue(as_fwd<U>(u)...);
    }

    fn is_empty() const noexcept -> bool {
        return base::is_empty();
    }

    fn is_full() const noexcept -> bool {
        return base::is_full();
    }

    fn capacity() const noexcept  -> u32 {
        return base::get_capacity();
    }

    template<typename ...U>
    fn push(U&& ...u) noexcept -> Option<Queue&> {
        return base::push_back(as_fwd<U>(u)...) & Option<Queue&>::Some(*this);
    }

    fn pop() noexcept -> Option<T> {
        return base::pop_front();
    }

protected:
    template<class ...Us>
    Queue(Us&& ...args) noexcept: base(as_fwd<Us>(args)...)
    {}
};

template<class T, class List = List<T> >
class Stack: Deque<T, List>
{
public:
    using base = Deque<T, List>;

    Stack(Stack&& other) noexcept : base(as_mov(other))
    {}

    template<class ...U>
    static fn from_list(U&& ...u) noexcept -> Stack {
        return Stack(base::from_list(as_fwd<U>(u)...));
    }

    fn is_empty() const noexcept -> bool {
        return base::is_empty();
    }

    fn is_full() const noexcept -> bool {
        return base::is_full();
    }

    fn push(const T& val) noexcept -> Option<Stack&> {
        return base::push_back(val) & Option<Stack&>::Some(*this);
    }

    fn pop() noexcept -> Option<T> {
        return base::pop_back();
    }

protected:
    template<class ...U>
    Stack(U&& ...u) noexcept: base(as_fwd<U>(u)...) {
    }
};

template<class T, class List = List<T> >
class RingBuffer : Deque<T, List>
{
public:
    using base = Deque<T, List>;

    RingBuffer(RingBuffer&& other) noexcept : base(as_mov(other))
    {}

    template<class ...U>
    static fn from_list(U&& ...u) noexcept -> RingBuffer {
        return RingBuffer(as_fwd<U>(u)...);
    }

    // method: check if empty
    fn is_empty() const noexcept -> bool {
        return base::is_empty();
    }

    // method: check if full
    fn is_full() const noexcept -> bool {
        return base::is_full();
    }

    // property[r]: capacity
    fn capacity() const noexcept  -> u32 {
        return base::capacity();
    }

    fn push(const T& val) noexcept -> void {
        base::_push_back<base::Opt::OverWrite>(val);
    }

    fn pop() noexcept -> Option<T> {
        return base::pop_front();
    }

protected:
    template<class ...Us>
    RingBuffer(Us&& ...args) noexcept : base(as_fwd<Us>(args)...)
    {}
};

// |      0      |
// |    /   \    |
// |   1     2   |
// |  /\     /\  |
// | 3  4   5  6 |
template<class T, class List = List<T> >
class Heap
{
public:
    List _list;

    Heap(Heap&& other) noexcept : _list(as_mov(other._list))
    {}

    template<class ...U>
    static fn from_list(U&& ...u) noexcept -> Heap {
        return Heap(as_fwd<U>(u)...);
    }

    fn is_empty() const noexcept -> bool {
        return _list.is_empty();
    }

    fn is_full() const noexcept -> bool {
        return _list.is_full();
    }

    fn top() noexcept -> Option<T&> {
        if (is_empty()) {
            return Option<T&>::None();
        }
        return Option<T&>::Some(_list[0]);
    }

    fn top() const noexcept -> Option<const T&> {
        if (is_empty()) {
            return Option<const T&>::None();
        }
        return Option<const T&>(_list[0]);
    }

    template<class U>
    fn push(U&& u) noexcept -> Option<Heap&> {
         mut res = _list.push(as_fwd<U>(u));
         if (res.is_none()) {
             return Option<Heap&>::None();
         }

         this->shift_up(_list._size - 1);
         return Option<Heap&>::Some(*this);
    }

    fn pop() noexcept -> Option<T> {
        if (is_empty()) {
            return Option<T>::None();
        }

        ustd::swap(_list[0], _list[_list._size - 1]);
        mut res = _list.pop();
        shift_down(0);

        return as_mov(res);
    }

private:
    template<class ...U>
    Heap(U&& ...u) noexcept : _list(as_fwd<U>(u)...) {
    }

    fn shift_up(u32 inode) noexcept {
        while (inode > 0) {
            let iroot = (inode - 1) / 2;
            if (_list[iroot] <= _list[inode]) break;

            ustd::swap(_list[inode], _list[iroot]);
            inode = iroot;
        }
    }

    fn shift_down(u32 inode) noexcept {
        let cnt = _list._size;

        while (inode * 2 + 1 < cnt) {
            let ileft  = inode * 2 + 1;
            let iright = inode * 2 + 2;
            let inext  = iright < cnt ? (_list[ileft] < _list[iright] ? ileft : iright) : ileft;

            if (_list[inode] <= _list[inext]) break;

            ustd::swap(_list[inode], _list[inext]);
            inode = inext;
        }
    }
};

}

#pragma once

#include "ustd/core.h"
#include "ustd/time.h"

namespace ustd::serialization
{

struct Node;
struct Dom;

using ustd::begin;
using ustd::end;

enum class Error
{
    Success,
    Invalid,
    OutOfRange,
    KeyNotFound,
    ParseFailed,
    UnexpectType,
    UnexpectSize,
    UnexceptEnumName,
};

pub fn to_str(Type type) noexcept->str;

enum class Type : u16
{
    $null,
    $bool,

    $i8,  $u8,
    $i16, $u16,
    $i32, $u32,
    $i64, $u64,
    $f32, $f64,

    $num,
    $str,
    $key,
    $time,
    $array,
    $object
};

pub fn to_str(Type type) noexcept->str;

template<class T>
using Result = ustd::Result<T, Error>;

struct Node
{
public:
    using Time = time::Time;

    Type _type = Type::$null;   // 2 byte
    u16  _size = 0;             // 2 byte
    u32  _next = 0;             // 4 byte

    union                       // 8 byte
    {
        void*       _ok;
        bool        _bool;
        i8          _i8;
        u8          _u8;
        i16         _i16;
        u16         _u16;
        i32         _i32;
        u32         _u32;
        i64         _i64;
        u64         _u64;
        f32         _f32;
        f64         _f64;

        const char* _str;
        const char* _key;
        const char* _num;

        Node*       _arr;
        Node*       _obj;
        u64         _time;
    };

public:
    Node() noexcept = default;

    static fn from_null()         noexcept -> Node { Node res; res._type = Type::$null;  return res; }

    static fn from_bool(bool val) noexcept -> Node { Node res; res._type = Type::$bool;  res._bool = val; return res; }

    static fn from_num(u8    val) noexcept -> Node { Node res; res._type = Type::$u8;  res._u8  = val; return res; }
    static fn from_num(i8    val) noexcept -> Node { Node res; res._type = Type::$u8;  res._i8  = val; return res; }

    static fn from_num(u16   val) noexcept -> Node { Node res; res._type = Type::$u16; res._u16 = val; return res; }
    static fn from_num(i16   val) noexcept -> Node { Node res; res._type = Type::$u16; res._i16 = val; return res; }

    static fn from_num(u32   val) noexcept -> Node { Node res; res._type = Type::$u32; res._u32 = val; return res; }
    static fn from_num(i32   val) noexcept -> Node { Node res; res._type = Type::$u32; res._i32 = val; return res; }

    static fn from_num(u64   val) noexcept -> Node { Node res; res._type = Type::$u64; res._u64 = val; return res; }
    static fn from_num(i64   val) noexcept -> Node { Node res; res._type = Type::$u64; res._i64 = val; return res; }

    static fn from_num(f32   val) noexcept -> Node { Node res; res._type = Type::$f32; res._f32 = val; return res; }
    static fn from_num(f64   val) noexcept -> Node { Node res; res._type = Type::$f64; res._f64 = val; return res; }

    static fn from_str(str   val) noexcept -> Node { Node res; res._type = Type::$str; res._str = val._data; res._size = u16(val._size); return res; }
    static fn from_key(str   val) noexcept -> Node { Node res; res._type = Type::$key; res._key = val._data; res._size = u16(val._size); return res; }
    static fn from_num(str   val) noexcept -> Node { Node res; res._type = Type::$num; res._num = val._data; res._size = u16(val._size); return res; }

    static fn from_time(Time val) noexcept -> Node { Node res; res._type = Type::$time; res._time = val.total_nanos(); return res; }

    static fn from_array()        noexcept -> Node { Node res; res._type = Type::$array;  return res; }
    static fn from_object()       noexcept -> Node { Node res; res._type = Type::$object; return res; }

    template<class T>
    fn as() const noexcept -> Result<T> {
        if constexpr($is_same<T, void>) { 
            if (_type == Type::$null) {
                return Result<T>::Ok();
            }
        }
        else if constexpr($is_same<T, bool>) {
            if (_type == Type::$bool) {
                return Result<T>::Ok(_bool);
            }
        }
        else if constexpr(trait<T>::$num) {
            return as_num<T>();
        }
        else if constexpr(trait<T>::$enum) {
            if (_type == Type::$str) {
                return as_enum<T>();
            }
        }
        else if constexpr($is_same<T, str>) {
            if (_type == Type::$str || _type == Type::$key || _type == Type::$num) {
                return Result<T>::Ok(_str, _size);
            }
        }

        return Result<T>::Err(Error::UnexpectType);
    }

private:
    template<class T>
    static constexpr fn get_num_type() noexcept -> Type {
        if      constexpr($is_same<T, i8>)   return Type::$i8 ;
        else if constexpr($is_same<T, u8>)   return Type::$u8 ;
        else if constexpr($is_same<T, i16>)  return Type::$i16;
        else if constexpr($is_same<T, u16>)  return Type::$u16;
        else if constexpr($is_same<T, i32>)  return Type::$i32;
        else if constexpr($is_same<T, u32>)  return Type::$u32;
        else if constexpr($is_same<T, i64>)  return Type::$i64;
        else if constexpr($is_same<T, u64>)  return Type::$u64;
        else if constexpr($is_same<T, f32>)  return Type::$f32;
        else if constexpr($is_same<T, f64>)  return Type::$f64;
        return Type::$f64;
    }

    template<class T>
    fn parse_num() const noexcept -> Result<T> {
        if (_type != Type::$num) {
            return Result<T>::Err(Error::UnexpectType);
        }

        let num_str = str{ _num, u32(_size) };
        let num_opt = str_parse<T>(num_str);
        if (num_opt.is_none()){
            return Result<T>::Err(Error::ParseFailed);
        }
        return Result<T>::Ok(num_opt._val);
    }

    template<class T>
    fn as_num() const noexcept -> Result<T> {
        static constexpr let type = get_num_type<T>();

        if      constexpr($is_same<T, i8>)  { if (_type == type ) return  Result<T>::Ok(_i8) ; }
        else if constexpr($is_same<T, u8>)  { if (_type == type ) return  Result<T>::Ok(_u8) ; }
        else if constexpr($is_same<T, i16>) { if (_type == type ) return  Result<T>::Ok(_i16); }
        else if constexpr($is_same<T, u16>) { if (_type == type ) return  Result<T>::Ok(_u16); }
        else if constexpr($is_same<T, i32>) { if (_type == type ) return  Result<T>::Ok(_i32); }
        else if constexpr($is_same<T, u32>) { if (_type == type ) return  Result<T>::Ok(_u32); }
        else if constexpr($is_same<T, i64>) { if (_type == type ) return  Result<T>::Ok(_i64); }
        else if constexpr($is_same<T, u64>) { if (_type == type ) return  Result<T>::Ok(_u64); }
        else if constexpr($is_same<T, f32>) { if (_type == type ) return  Result<T>::Ok(_f32); }
        else if constexpr($is_same<T, f64>) { if (_type == type ) return  Result<T>::Ok(_f64); }

        if (_type == Type::$num) {
            return parse_num<T>();
        }

        return Result<T>::Err(Error::UnexpectType);
    }

    template<class T>
    fn as_enum() const noexcept -> Result<T> {
        let str_val = as<str>();
        for (mut i = 0; i < 1024u; ++i) {
            let enum_val = T(i);
            let enum_name = to_str(enum_val);
            if (enum_name == str()) break;
            if (enum_name == res) {
                return Result<T>::Ok(enum_val);
            }
        }

        return Result<T>::Err(Error::UnexceptEnumName);
    }
};

struct Dom
{
    Slice<Node>& _nodes;
    u32          _index;

    Dom(Slice<Node>& nodes, u32 index) noexcept
        : _nodes(nodes), _index(index)
    {}

    Dom(const Dom& other) noexcept
        : _nodes(other._nodes), _index(other._index)
    {}

    fn operator=(const Dom& other) noexcept -> Dom& {
        _index = other._index;
        return *this;
    }

#pragma region properties
    /* property: node */
    __declspec(property(get = get_node, put = set_node)) Node node;
    fn get_node() const     noexcept -> const Node& { return _nodes[_index]; }
    fn get_node()           noexcept -> Node&       { return _nodes[_index]; }
    fn set_node(Node val)   noexcept -> void        { _nodes[_index] = val;  }

    /* property: type */
    __declspec(property(get = get_type)) Type type;
    fn get_type() const noexcept -> Type { return node._type; }

    /* property: len */
    __declspec(property(get = get_len)) u32 len;
    fn get_len() const noexcept -> u32 { return node._size; }

    /* property: key */
    __declspec(property(get = get_key)) Option<str> key;
    fn get_key() const noexcept -> Option<str> { 
        let key_node = _nodes[_index - 1]; 
        if (key_node._type != Type::$key) return Option<str>::None();
        return Option<str>::Some(key_node._str, key_node._size);
    }

    template<class T>
    fn as() const noexcept -> Result<T> {
        return node.as<T>();
    }
#pragma endregion

#pragma region iterator
    struct Iter
    {
        using type_t = Dom;

        Slice<Node>&    _nodes;
        u32             _root_idx;
        u32             _element_idx;

        Iter(Slice<Node>& nodes, u32 root_idx)
            : _nodes(nodes), _root_idx(root_idx), _element_idx(root_idx)
        {}

        Iter(const Iter& other)
            : _nodes(other._nodes), _root_idx(other._root_idx), _element_idx(other._element_idx)
        {}

        pub fn next() noexcept->Option<Dom>;
    }; 

    fn into_iter() const noexcept -> Iter {
        return Iter(_nodes, _index);
    }
#pragma endregion

#pragma region access
    pub fn operator[](u32 idx)  noexcept -> Result<Dom>;
    pub fn operator[](str name) noexcept -> Result<Dom>;
#pragma endregion

#pragma region methods
    pub fn arr_add_element(Dom& prev, Node element)       noexcept -> Dom;
    pub fn obj_add_key_val(Dom& prev, Node key, Node val) noexcept -> Tuple<Dom, Dom>;
    pub fn obj_add_key_val(Dom& prev, str  key, Node val) noexcept -> Tuple<Dom, Dom>;
#pragma endregion

protected:
    pub fn _parse_json(str text) noexcept -> ustd::Result<void, str>;
};

pub fn trait_sfmt(Formatter& fmt, const Dom& dom) noexcept -> void;

class Tree: public Dom
{
public:
    using base = Dom;

    List<Node>  _vec;

    Tree(Tree&& other) noexcept
        : Dom(*this), _vec(as_mov(other._vec))
    {
        mut tmp = Dom(_vec, other._index);
        ustd::swap(static_cast<Dom&>(*this), tmp);
    }

    static fn with_capacity(u32 capacity) noexcept -> Tree {
        return Tree(capacity);
    }

    static fn from_json(str text) noexcept -> ustd::Result<Tree, str> {
        mut tree = Tree(text.len / 16 + 256);

        let res = tree._parse_json(text);
        return res.map([&]() { return as_mov(tree); });
    }

protected:
    Tree(u32 capacity) noexcept
        : Dom{ _vec, 0 }
        , _vec{ List<Node>::with_capacity(capacity) }
    {
        _vec.push(Node::from_null());
    }
};
}


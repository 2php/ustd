#include "config.inl"

namespace ustd::serialization
{

pub fn to_str(Type type) noexcept -> str {
    switch (type) {
        case Type::$null:       return "none";
        case Type::$bool:       return "bool";
        case Type::$i8:         return "i8";
        case Type::$u8:         return "u8";
        case Type::$i16:        return "i16";
        case Type::$u16:        return "u16";
        case Type::$i32:        return "i32";
        case Type::$u32:        return "u32";
        case Type::$i64:        return "i64";
        case Type::$u64:        return "u64";
        case Type::$f32:        return "f32";
        case Type::$f64:        return "f64";
        case Type::$num:        return "number";
        case Type::$str:        return "str";
        case Type::$key:        return "key";
        case Type::$time:       return "time";
        case Type::$array:      return "array";
        case Type::$object:     return "object";
    }
    return "unknown";
};

pub fn Dom::Iter::next() noexcept -> Option<Dom> {
    if (_element_idx == _root_idx) {
        let root_type = _nodes[_root_idx]._type;
        if (root_type == Type::$array) {
            _element_idx = _root_idx + 1;
        }
        else if (root_type == Type::$object) {
            _element_idx = _root_idx + 2;
        }
        else {
            return Option<Dom>::None();
        }
        return Option<Dom>::Some(_nodes, _element_idx);
    }

    mut& element = _nodes[_element_idx];
    if (element._next == 0) {
        return Option<Dom>::None();
    }
    _element_idx += element._next;

    return Option<Dom>::Some(_nodes, _element_idx);
}

#pragma region DOM

pub fn Dom::operator[](u32 idx) noexcept -> Result<Dom> {
    if (Type::$array != node()._type) return Result<Dom>::Err(Error::UnexpectType);
    if (idx >= node()._size) return Result<Dom>::Err(Error::OutOfRange);

    mut val = *this;
    val._index += 1;
    for (mut i = 0u; i < idx; ++i) {
        val._index += val.node()._next;
    }

    return Result<Dom>::Ok(val);
}

pub fn Dom::operator[](str name) noexcept -> Result<Dom> {
    if (Type::$object != node()._type) {
        return Result<Dom>::Err(Error::UnexpectType);
    }

    let obj_cnt = node()._size;
    mut val_dom = Dom(_nodes, _index + 2);

    for (mut i = 0u; i < obj_cnt; ++i) {
        let key_opt = val_dom.key();
        if (key_opt.is_none()) {
            return Result<Dom>::Err(Error::UnexpectType);
        }
        let key_name = key_opt._val;

        if (key_name == name) {
            return Result<Dom>::Ok(val_dom);
        }

        val_dom._index += val_dom.node()._next;
    }

    return Result<Dom>::Err(Error::KeyNotFound);
}

pub fn Dom::arr_add_element(Dom& prev, Node value) noexcept -> Dom {
    let prev_idx = prev._index;
    let node_idx = _nodes._size;

    _nodes[_index]._size += 1;
    _nodes.push(value);

    if (prev_idx != _index) {
        _nodes[prev_idx]._next = node_idx - prev_idx;
    }

    return {_nodes, node_idx};
}

pub fn Dom::obj_add_key_val(Dom& prev_key, Node key, Node val) noexcept -> Tuple<Dom, Dom> {
    let prev_key_idx = prev_key._index;
    let prev_val_idx = prev_key_idx + 1;

    let key_idx = _nodes._size;
    let val_idx = key_idx + 1;

    _nodes[_index]._size += 1;
    _nodes.push(key);
    _nodes.push(val);

    if (prev_key_idx != _index) {
        _nodes[prev_key_idx]._next = key_idx - prev_key_idx;
        _nodes[prev_val_idx]._next = val_idx - prev_val_idx;
    }

    return { Dom(_nodes, key_idx), Dom(_nodes, val_idx) };
}

pub fn Dom::obj_add_key_val(Dom& prev, str skey, Node val) noexcept -> Tuple<Dom, Dom> {
    let key = Node::from_str(skey);
    return obj_add_key_val(prev, key, val);
}

#pragma endregion

fn trait_sfmt_json(Formatter& fmt, const Dom& dom) noexcept -> void;
fn trait_sfmt_xml (Formatter& fmt, const Dom& dom) noexcept -> void;

pub fn trait_sfmt(Formatter& fmt, const Dom& dom) noexcept -> void {
    let style = fmt._style;
    let spec  = style.spec();

    if (spec == str()) {
        trait_sfmt_json(fmt, dom);
    }
    else if (spec == str("json")) {
        trait_sfmt_json(fmt, dom);
    }
    else if (spec == str("xml")) {
        trait_sfmt_xml(fmt, dom);
    }
}

}


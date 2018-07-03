#include "config.inl"

namespace ustd::serialization
{

struct JsonParser: Dom
{
    using res_t = ustd::Result<void, str>;
    str _text;

    JsonParser(Dom& dom, str text) noexcept: Dom(dom), _text(text)
    { }

    fn parse() noexcept -> res_t {
        mut res  = parse_any();
        return res;
    }

private:
    static fn is_blank(char c) noexcept -> bool {
        return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
    }

    fn peek_char() noexcept -> char {
        // .....#####
        //      ^
        //     pos
        let cnt = u32(_text.len);
        mut pos = 0u;

        while (true) {
            let c = _text[pos];

            if (!is_blank(c)) {
                _text._data += pos;
                _text._size -= pos;
                return c;
            }

            if (++pos >= cnt) break;
        }

        return char(0);
    }

    fn next_char() noexcept -> char {
        let c = peek_char();
        _text._data += 1;
        _text._size -= 1;
        return c;
    }

    template<u32 N>
    fn parse_match(const char(&s)[N]) noexcept -> bool {
        constexpr let len = N - 1u;

        if (len >= _text._size) {
            return false;
        }

        for (mut i = 1u; i < len; ++i) {
            if (s[i] != _text._data[i]) return false;
        }

        _text._data += len;
        _text._size -= len;
        return true;
    }

    fn parse_number() noexcept -> res_t {
        // 0123456789,....
        // ^         ^    ^
        // b         p    e

        let b = _text._data;
        let e = b + _text._size;
        mut p = b + 1;

        while (p < e) {
            let c = *p;
            if (c == ',' || c == '}' || c == ']') break;
            ++p;
        }

        if (p == e) {
            return res_t::Err(_text);
        }

        _text._data = p;
        _text._size -= (p - b);

        let val = str(b, u32(p - b));
        set_node(Node::from_num(val));
        return res_t::Ok();
    }

    fn parse_str() noexcept -> res_t {
        // "abcdefg"......
        // ^        ^    ^
        // b        p    e

        let b = _text._data;
        let e = b + _text._size;
        mut p = b + 1;

        while (p < e) {
            let c = *p;

            if (c == '\\') {
                p += 2;
            }
            else if (c == '"') {
                break;
            }
            else {
                ++p;
            }
        }
        ++p;

        if (p >= e) {
            return res_t::Err(_text);
        }

        _text._data = p;
        _text._size -= (p - b);

        let val = str(b + 1, u32(p - b - 2));
        set_node(Node::from_str(val));

        return res_t::Ok();
    }

    fn parse_key() noexcept -> res_t {
        // "abcdefg"...:....
        // ^        ^  ^   ^
        // b        p  x   e

        let c = peek_char();
        if (c != '"') {
            return res_t::Err();
        }

        let b = _text._data;
        let e = b + _text._size;
        mut p = b + 1;

        while (p < e) {
            let x = *p;

            if (x == '"')  break;
            else if (x == '\\') p += 2;
            else                ++p;
        }
        ++p;

        if (p >= e) {
            return res_t::Err();
        }

        _text._data = p;
        _text._size -= (p - b);

        let val = str{ b + 1, u32(p - b - 2) };
        set_node(Node::from_key(val));
        return res_t::Ok();
    }

    fn parse_array() noexcept -> res_t {
        set_node(Node::from_array());

        next_char();    //< jump '['

        // check next_char
        if (true) {
            let c = peek_char();
            if (c == char(0)) {
                return res_t::Err(_text);
            }
            if (c == ']') {
                next_char();
                return res_t::Ok();
            }
        }

        mut prev = Dom(*this);

        while (true) {
            // add val
            mut val        = arr_add_element(prev, Node::from_null());
            mut val_parser = JsonParser(val, _text);
            let res         = val_parser.parse();
            if (res.is_err()) {
                return res;
            }

            prev = val;

            let c = val_parser.next_char();
            _text = val_parser._text;
            if (c == ',') {
                continue;
            }
            else if (c == ']') {
                return res_t::Ok();
            }
            else {
                return res_t::Err(_text);
            }
        }
    }

    fn parse_object() noexcept -> res_t {
        set_node(Node::from_object());
        next_char();    // jump {

        if(true) {
            let c = peek_char();
            if (c == char(0)) {
                return res_t::Err(_text);
            }
            if (c == '}') {
                next_char();    // jump }
                return res_t::Ok();
            }
        }

        mut prev_key = Dom(*this);

        while (true) {
            mut kv          = obj_add_key_val(prev_key, Node::from_null(), Node::from_null());

            // parse key
            mut key_parser = JsonParser(kv.$0, _text);
            mut key_res    = key_parser.parse_key();
            if (key_res.is_err()) {
                return key_res;
            }
            prev_key = key_parser;

            // parse ':'
            let char_colon = key_parser.next_char();
            if (char_colon != ':') {
                return res_t::Err(key_parser._text);
            }

            // parse val
            mut val_parser = JsonParser(kv.$1, key_parser._text);
            mut val_res    = val_parser.parse();
            if (val_res.is_err()) {
                return val_res;
            }

            // parse ',' '}'
            let char_comma = val_parser.next_char();
            _text = val_parser._text;

            if (char_comma == ',') {
                continue;
            }
            else if (char_comma == '}') {
                return res_t::Ok();
            }
            else {
                return res_t::Err(_text);
            }
        }
    }

    fn parse_any() noexcept -> res_t {
        let ch = peek_char();

        switch (ch) {
            case 'n':
                if (parse_match("null")) {
                    set_node(Node::from_null());
                    return res_t::Ok();
                }
                break;

            case 't':
                if (parse_match("true")) {
                    set_node(Node::from_bool(true));
                    return res_t::Ok();
                }
                break;

            case 'f':
                if (parse_match("false")) {
                    set_node(Node::from_bool(false));
                    return res_t::Ok();
                }
                break;

            case '[':
                return parse_array();

            case '{':
                return parse_object();

            case '"':
                return parse_str();

            case '+': case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
                return parse_number();

            default:
                break;
        }

        return res_t::Err(_text);
    }
};

class JsonFormater
{
public:
    JsonFormater(Formatter& fmt)
        : _fmt(fmt)
    {}

    fn do_fmt(const Dom& dom) -> void {
        let node = dom.node;

        switch (node._type) {
            case Type::$null:  _fmt("null");       break;
            case Type::$bool:  _fmt(node._bool);   break;
            case Type::$u8:    _fmt(node._u8);     break;
            case Type::$i8:    _fmt(node._i8);     break;
            case Type::$u16:   _fmt(node._u16);    break;
            case Type::$i16:   _fmt(node._i16);    break;
            case Type::$u32:   _fmt(node._u32);    break;
            case Type::$i32:   _fmt(node._i32);    break;
            case Type::$u64:   _fmt(node._u64);    break;
            case Type::$i64:   _fmt(node._i64);    break;
            case Type::$f32:   _fmt(node._f32);    break;
            case Type::$f64:   _fmt(node._f64);    break;
            case Type::$num:   _fmt(str(node._str, node._size)); break;

            case Type::$key:
            case Type::$str:
                _fmt.push_str("\"");
                _fmt.push_str(str(node._str, node._size));
                _fmt.push_str("\"");
                break;

            case Type::$time: {
                let time = time::SystemTime::from_nanos(node._time);
                _fmt(time);
                break;
            }

            case Type::$array: {
                _fmt.push_str("[");

                _fmt._indent += 1;

                // loop array elements
                let cnt = node._size;
                mut val = Dom(dom._nodes, dom._index + 1);
                for (mut idx = 0u; idx < cnt; ++idx, val._index += val.node._next) {
                    do_fmt(val);
                    if (idx + 1 != cnt) {
                        _fmt.push_str(", ");
                    }
                }

                _fmt._indent -= 1;
                _fmt.push_char(']');
                break;
            }

            case Type::$object: {
                _fmt.push_str("{\n");

                _fmt.indent(+1);

                // loop object elements
                let cnt = node._size;
                mut key = Dom{ dom._nodes, dom._index + 1 };
                mut val = Dom{ dom._nodes, dom._index + 2 };
                for (mut idx = 0u; idx < cnt; ++idx, key._index += key.node._next, val._index += val.node._next) {
                    if (idx!=0) _fmt.indent();

                    _fmt.push_str("\"");
                    _fmt.push_str(str(key.node._key, key.node._size));
                    _fmt.push_str("\": ");

                    do_fmt(val);

                    _fmt.push_str(idx + 1 != cnt ? str(",\n") : str("\n"));
                }

                _fmt.indent(-1);
                _fmt.push_char('}');
                break;
            }
        }
    }

private:
    Formatter& _fmt;
};

fn trait_sfmt_json(Formatter& fmt, const Dom& dom) noexcept -> void {
    mut impl = JsonFormater(fmt);
    impl.do_fmt(dom);
}

// wraper
pub fn Dom::_parse_json(str text) noexcept -> ustd::Result<void,str> {
    _nodes.clear();
    _nodes.push(Node::from_null());

    mut root = JsonParser(*this, text);
    let res = root.parse();
    return res;
}

}

namespace ustd::serialization
{

unittest(json) {
    let text = str(R"({
    "x": "TA",
    "y": [{
            "k0": "v0",
            "k1": [1, 2, 3, 4]
        }, {
            "ka": "v1",
            "kb": [5, 6, 7, 8]
        }]
})");

    let dom_opt = Tree::from_json(text);
    assert_eq(dom_opt.is_ok(), true);

    let& dom = dom_opt.unwrap();
    log::info("dom[json] = {:json}", dom);
}

}

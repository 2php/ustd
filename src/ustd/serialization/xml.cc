#include "config.inl"

namespace ustd::serialization
{

fn to_str(Type type) noexcept->str;

class XmlFormater
{
public:
    XmlFormater(Formatter& fmt)
        : _fmt(fmt)
    {}

    fn head(const Dom& dom) -> void {
        let node = dom.node();

        _fmt.push_str("<xml? version=\"1.0\" encoding=\"utf-8\" ?>\n");
        sformat(_fmt._outbuf, "<root size={}>", node._size);
    }

    fn tail() -> void {
        _fmt.push_str("</root>\n");
    }

    fn do_fmt(const Dom& dom) -> void {
        let node = dom.node();
        let type = dom.type();

        switch (type) {
            case Type::$null:   _fmt("");           break;
            case Type::$bool:   _fmt(node._bool);   break;
            case Type::$u8:     _fmt(node._u8);    break;
            case Type::$i8:     _fmt(node._i8);    break;
            case Type::$u16:    _fmt(node._u16);    break;
            case Type::$i16:    _fmt(node._i16);    break;
            case Type::$u32:    _fmt(node._u32);    break;
            case Type::$i32:    _fmt(node._i32);    break;
            case Type::$u64:    _fmt(node._u64);    break;
            case Type::$i64:    _fmt(node._i64);    break;
            case Type::$f32:    _fmt(node._f32);    break;
            case Type::$f64:    _fmt(node._f64);    break;

            case Type::$num: {
                _fmt.push_str(str(node._str, node._size));
                break;
            }

            case Type::$time: {
                let time = time::SystemTime::from_nanos(node._time);
                _fmt(time);
                break;
            }

            case Type::$key:
            case Type::$str: {
                _fmt.push_str("\"");
                _fmt.push_str(str(node._str, node._size));
                _fmt.push_str("\"");
                break;
            }

            case Type::$array: {
                _fmt.push_str("\n");

                _fmt._indent += 1;

                // loop array elements
                let cnt = node._size;
                mut val = Dom{ dom._nodes, dom._index + 1 };

                for (mut idx = 0u; idx < cnt; ++idx, val._index += val.node()._next) {
                    _fmt.indent();

                    if (val.type() == Type::$array || val.type() == Type::$object) {
                        sformat(_fmt._outbuf, "<item type=\"{}\", size={}>", to_str(val.type()), val.len());
                    }
                    else {
                        sformat(_fmt._outbuf, "<item type=\"{}\">", to_str(val.type()));
                    }
                    do_fmt(val);

                    if (val.type() == Type::$array || val.type() == Type::$object) {
                        _fmt.indent();
                    }
                    _fmt.push_str("</item>\n");
                }

                _fmt._indent -= 1;
                break;
            }

            case Type::$object: {
                _fmt.push_str("\n");

                _fmt._indent += 1;

                // loop object elements
                let cnt = node._size;
                mut key = Dom{ dom._nodes, dom._index + 1 };
                mut val = Dom{ dom._nodes, dom._index + 2 };

                for (u32 idx = 0u; idx < cnt; ++idx, key._index += key.next(), val._index += val.next()) {
                    _fmt.indent();

                    sformat(_fmt._outbuf, "<{} type=\"{}\">", str{ key.node()._str, key.node()._size }, to_str(val.type()));
                    do_fmt(val);

                    if (val.type() == Type::$array || val.type() == Type::$object) {
                        _fmt.indent();
                    }
                    sformat(_fmt._outbuf, "</{}>\n", str{ key.node()._str, key.node()._size });
                }

                _fmt._indent -= 1;
                break;
            }
        }
    }

private:
    Formatter& _fmt;
};

pub fn trait_sfmt_xml(Formatter& fmt, const Dom& dom) noexcept -> void {
    mut impl = XmlFormater(fmt);
    impl.head(dom);
    impl.do_fmt(dom);
    impl.tail();
}

}


namespace ustd::serialization
{

unittest(xml) {
    let text = str(R"({
    "x": "TA",
    "y": [{
            "k0": "v0",
            "k1": [1, 2, 3, 4]
        }, {
            "k0": "v1",
            "k1": [1, 2, 3, 4]
        }]
})");

    let dom = Tree::from_json(text).unwrap();
    log::info("dom[xml] = {:xml}", dom);
}

}

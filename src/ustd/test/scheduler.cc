#include "config.inl"

namespace ustd::test
{

using namespace ustd::io;

enum State
{
    Ignore,
    Running,
    Success,
    Error,
};

static fn match(str name, Slice<str> patterns) noexcept -> bool {
    if (patterns.len() == 0) {
        return true;
    }

    int cnts = 0;

    for (let& pattern : patterns.into_iter()) {
        switch (pattern[0]) {
        case '+':
            if (name.starts_with(pattern.slice(1, -1))) {
                ++cnts;
            }
            break;

        case '-':
            if (name.starts_with(pattern.slice(1, -1))) {
                return false;
            }
            else {
                ++cnts;
            }
            break;

        default:
            break;
        }
    }

    return cnts != 0;
}

static fn on(str name, State stat) -> void {
    using namespace io;

    static const SGR title_sgrs[] = { SGR::FG_YEL, SGR::FG_CYN, SGR::FG_GRN, SGR::FG_RED };
    static const str            signs[]  = { "~~", ">>", "<<", "!!" };
    log::logger().log_msg(title_sgrs[stat], signs[stat], io::SGR::FG_MAG, name);
}

struct TestInfo
{
    Type type;
    str  name;
    void(*func)();
};

static fn get_tests() noexcept -> Slice<TestInfo>& {
    static FixedList<TestInfo, 1024> res;
    return res;
}

pub fn Pipeline::install1(Type type, str name, void(*func)()) -> void {
    let info  = TestInfo{ type, name, func };
    get_tests().push(info);
}

pub fn Pipeline::invoke(Slice<str> pattern) noexcept -> u32 {
    let& tests = get_tests();

    mut cnt = 0u;

    for(mut test: tests.into_iter()) {
        let  name = snformat<256>("{}::{}", test.type.mod(), test.name);

        if (!match(name, pattern)) {
            on(name, Ignore);
        }
        else {
            on(name, Running);
            try {
                test.func();
                on(name, Success);
                ++cnt;
            }
            catch (...) {
                on(name, Error);
            }
        }
    }
    return cnt;
}

}


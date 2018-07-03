#include "ustd.h"

using namespace ustd;
using namespace ustd::ffi;

int main(int argc, const char* argv[]) {
    mut opts = FixedList<str, 32>();
    mut libs = FixedList<Mod, 16>();

    for (int i = 1; i < argc; ++i) {
        let s = cstr(argv[i]);

        switch (s[0]) {
        case '@': {
            let path = fs::FixedPath(s.slice(1, -1));
            libs.push(Mod::load(path).unwrap());
            break;
        }
        case '+':
        case '-':
            opts.push(s);
        }
    }
    test::Pipeline::instance().invoke(opts);

    return 0;
}

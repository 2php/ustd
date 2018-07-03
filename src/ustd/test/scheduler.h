#pragma once

namespace ustd::test
{

class Pipeline
{
public:
    pub fn invoke(Slice<str> pattern) noexcept -> u32;

    static fn instance() noexcept -> Pipeline& {
        static mut res = Pipeline();
        return res;
    }

    template<typename T>
    void install(str name, void(*func)()) {
        static let type = typeof<T>();
        install1(type, name, func);
    }

private:
    Pipeline()           = default;
    ~Pipeline()          = default;
    Pipeline(Pipeline&&) = default;

    pub fn install1(Type type, str name, void(*func)()) -> void;
};

inline fn scheduler() noexcept -> Pipeline& {
    return Pipeline::instance();
}

}


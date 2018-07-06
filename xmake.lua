
-- modes: debug and release 
add_rules("mode.debug", "mode.release")

-- compiler: clang
set_tools("cxx", "b:/LLVM/bin/clang++.exe")
add_cxflags("-std=c++17", "-fms-extensions")
add_includedirs("src/")

target("ustd")
    set_kind("shared")

    add_files("src/**.cc") 
    del_files("src/**/main.cc")
    del_files("src/**_unix.cc")
    del_files("src/**_pthread.cc")


target("ustd.test")
    set_kind("binary")
    add_deps("ustd")

    add_files("src/ustd/test/main.cc")

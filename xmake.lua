
-- modes: debug and release 
add_rules("mode.debug", "mode.release")

-- compiler: clang
add_cxflags("-std=c++17", "-fms-extensions")
add_includedirs("src/")

target("ustd")
    set_kind("shared")
    add_files("src/**.cc") 
    del_files("src/**/main.cc")


target("ustd.test")
    set_kind("binary")
    add_deps("ustd")
    add_files("src/ustd/test/main.cc")

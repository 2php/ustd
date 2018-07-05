
-- add modes: debug and release 
add_rules("mode.debug", "mode.release")

-- add target
target(".")

    -- set kind
    set_kind("shared")

    -- set language
    add_cxflags("-std=c++17", "-fms-extensions")

    -- set files
    add_files("src/**.cc") 
    del_files("src/**_win32.cc")

    -- set includes
    add_includedirs("src/")


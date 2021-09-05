-- project
set_project("yutil")
-- add build modes
add_rules("mode.release", "mode.debug")

set_warnings("all")
add_defines("Y_EXPORTS", "_UNICODE")


-- for the windows platform (msvc)
if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

-- include project sources
includes("include")


target("yutil")
    -- make as a static/shared library
    set_kind("$(kind)")
    add_headerfiles("include/yutil_build.h")
    add_headerfiles("include/yutil.h")
    add_headerfiles("include/(yutil/*.h)")
    add_files("src/*.c")

target("test")
    -- make as a binary
    set_kind("binary")
    -- add the dependent target
    add_deps("yutil")
    add_headerfiles("test/*.h")
    -- add the source files
    add_files("test/test.c")
    add_files("test/*.c")
    add_deps("yutil")

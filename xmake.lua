-- project
set_project("yutil")
-- add build modes
add_rules("mode.release", "mode.debug")

set_warnings("all")
add_defines("Y_EXPORTS", "_UNICODE")

-- for the windows platform (msvc)
if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
else
    add_cxflags("-fPIC")
end

if is_mode("release") then
    set_symbols("none")
end

target("yutil")
    -- make as a static/shared library
    set_kind("$(kind)")

        -- export all symbols for windows/dll
    if is_plat("windows") and is_kind("shared") then
        if is_mode("release") then
            set_optimize("fastest")
        end
        add_rules("utils.symbols.export_all")
    end
    
    add_headerfiles("include/yutil.h")
    add_headerfiles("include/(yutil/*.h)")
    add_files("src/*.c")



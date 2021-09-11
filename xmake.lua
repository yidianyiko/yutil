-- project
set_project("yutil")

set_warnings("all")

-- set language: c99
stdc = "c99"
set_languages(stdc)

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")

-- add build modes
add_rules("mode.release", "mode.debug")

add_defines("_UNICODE")

-- for the windows platform (msvc)
if is_plat("windows") then
    add_defines("Y_EXPORTS", "_CRT_SECURE_NO_WARNINGS")
else
    add_defines("_GNU_SOURCE=1")
end

if is_mode("release") then
    set_symbols("none")
end
-- include project sources
includes("include")
includes("test")

-- 'xmake f --optionname=test'
option("test")
    set_default(true)
    set_showmenu(true)
    set_category("test")
    set_description("Enable or disable test option")
option_end()


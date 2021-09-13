--xmake build test
target("yutil_test")
    set_default(false)

    -- add the dependent target
    add_deps("yutil")

    if is_plat("windows") then
        add_defines("_CRT_SECURE_NO_WARNINGS")
    else
        add_cxflags("-Wno-error=unused-variable", "-Wno-error=unused-but-set-variable")
    end
    
    -- make as a binary
    set_kind("binary")

    add_headerfiles("*.h")
    add_files("*.c")
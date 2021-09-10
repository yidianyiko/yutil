--xmake build test
target("test")
    set_default(false)

    -- add the dependent target
    add_deps("yutil")
    
    -- make as a binary
    set_kind("binary")

    add_headerfiles("*.h")
    add_files("*.c")
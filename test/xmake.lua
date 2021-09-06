target("test")
    -- add the dependent target
    add_packages("yutil")
    -- make as a binary
    set_kind("binary")

    add_headerfiles("../**.h")
    -- add the source files
    add_files("../**.c")
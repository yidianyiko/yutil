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
    
    -- install importfiles for pkg-config
    add_rules("utils.install.pkgconfig_importfiles")

    -- add include directories
    add_includedirs("../include", {public = true})

    -- add the header files for installing
    add_headerfiles("yutil.h")
    add_headerfiles("yutil/*.h")

    -- add the common source files
    add_files("../src/*.c")
add_rules("mode.debug", "mode.releasedbg", "mode.release")

target("persian-calendar")
    set_kind("binary")
    add_files("*.c")

    add_ldflags("/ENTRY:_WinMainCRTStartup", {force = true})

    add_ldflags("/SUBSYSTEM:WINDOWS", {force = true})

    add_syslinks("kernel32", "gdi32", "user32", "shell32", "Shlwapi")

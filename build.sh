i686-w64-mingw32-gcc main.cc -o persian-calendar.exe \
    -fno-rtti -fno-exceptions -Oz -s \
    -lkernel32 -luser32 -lshell32 -lgdi32 -lshlwapi -ladvapi32 \
    -nostartfiles -Wl,-e,_WinMainCRTStartup

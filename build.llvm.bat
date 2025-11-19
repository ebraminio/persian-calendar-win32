@echo off
REM https://github.com/mstorsjo/llvm-mingw
C:\llvm-mingw\bin\i686-w64-mingw32-gcc persian-calendar.cc -o persian-calendar.llvm.exe ^
    -Weverything -Wall -Wextra -Wpedantic -Werror -Wno-c++98-compat-pedantic ^
    -fno-exceptions -fno-rtti -Oz -s -lkernel32 -luser32 -lshell32 -lgdi32 -lshlwapi -ladvapi32 ^
    -nostdlib -nodefaultlibs -nostartfiles -Wl,-e,start -Wl,-subsystem,windows ^
    -Wl,/entry:start -Wl,/subsystem:windows ^
    -Wl,/merge:.rdata=.text -Wl,/merge:.data=.text ^
    -Wl,/fixed

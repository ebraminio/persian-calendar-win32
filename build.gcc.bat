@echo off
C:\mingw32\bin\i686-w64-mingw32-gcc persian-calendar.cc -o persian-calendar.gcc.exe ^
    -Wall -Wextra -Wpedantic -Werror -fno-exceptions -Oz -s ^
    -lkernel32 -luser32 -lshell32 -lgdi32 -lshlwapi -ladvapi32 ^
    -nostdlib -nodefaultlibs -nostartfiles -Wl,-e,start -Wl,-subsystem,windows

clang --target=i686-w64-mingw32 --sysroot=$(brew --prefix mingw-w64)/toolchain-i686 \
    persian-calendar.cc -o persian-calendar.exe \
    -Wall -Wextra -Wpedantic -Werror -Weffc++ -fno-exceptions -Oz -s \
    -lkernel32 -luser32 -lshell32 -lgdi32 -lshlwapi -ladvapi32 \
    -nostdlib -nodefaultlibs -nostartfiles -fuse-ld=lld -Wl,-e,start -Wl,-subsystem,windows \
    && wine persian-calendar.exe

# https://github.com/mstorsjo/llvm-mingw
i686-w64-mingw32-gcc persian-calendar.cc -o persian-calendar.exe -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unsafe-buffer-usage -Wno-old-style-cast -Weverything -Wall -Wextra -Wpedantic -Werror -fno-exceptions -Oz -s -lkernel32 -luser32 -lshell32 -lgdi32 -lshlwapi -ladvapi32 -nostdlib -nodefaultlibs -nostartfiles -Wl,-e,start -Wl,-subsystem,windows

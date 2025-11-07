#!/bin/bash
# x86_64-w64-mingw32-gcc
i686-w64-mingw32-gcc main.cc -o persian-calendar.exe \
  -fno-rtti -fno-exceptions \
  -Oz -s -ffunction-sections -fdata-sections \
  -Wl,--gc-sections -Wl,--strip-all \
  -nostartfiles -fno-unwind-tables -fno-asynchronous-unwind-tables \
  -fno-stack-protector -fno-ident -fno-builtin \
  -lkernel32 -luser32 -lshell32 -lgdi32 -lshlwapi -ladvapi32 \
  -Wl,-e,_WinMainCRTStartup \
  -Wl,-subsystem,windows \
  -nostdlib -nodefaultlibs \
  -fmerge-all-constants -fno-plt && wine persian-calendar.exe

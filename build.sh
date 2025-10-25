#!/bin/bash
i686-w64-mingw32-gcc main.c -o persian-calendar.exe \
  -Os -s -ffunction-sections -fdata-sections \
  -Wl,--gc-sections -Wl,--strip-all -Wl,-Bstatic \
  -nostartfiles -fno-unwind-tables -fno-asynchronous-unwind-tables \
  -fno-stack-protector -fno-ident -fno-builtin \
  -lkernel32 -luser32 -lshell32 -lgdi32 \
  -Wl,-e,_WinMainCRTStartup \
  -Wl,-subsystem,windows \
  -fmerge-all-constants -fno-plt && wine persian-calendar.exe
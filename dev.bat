@echo off
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
taskkill /IM persian-calendar.exe /FI "STATUS eq RUNNING" ^
    && cl test.cc && test.exe ^
    && build.msvc.bat && build.llvm.bat && dir persian-calendar*.exe && persian-calendar.exe && build.gcc.bat
REM dumpbin /DISASM persian-calendar.exe

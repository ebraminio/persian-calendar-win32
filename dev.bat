REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
@taskkill /IM persian-calendar.exe /FI "STATUS eq RUNNING" ^
    && cl test.cc && test.exe ^
    && .\build.bat && dir persian-calendar.exe && persian-calendar.exe
REM dumpbin /DISASM persian-calendar.exe

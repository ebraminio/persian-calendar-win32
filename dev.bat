REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x8
@taskkill /IM persian-calendar.exe /FI "STATUS eq RUNNING" && .\build.bat && dir persian-calendar.exe && persian-calendar.exe
cl test.cc && test.exe
REM dumpbin /DISASM persian-calendar.exe

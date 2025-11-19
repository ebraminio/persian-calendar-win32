@cl persian-calendar.cc /utf-8 /O1 /Wall /GS- /wd4710 /wd4711 /wd5045 /WX ^
    /link /ENTRY:start /NODEFAULTLIB /SUBSYSTEM:WINDOWS,5.01 /INCREMENTAL:NO ^
    kernel32.lib user32.lib shell32.lib gdi32.lib shlwapi.lib advapi32.lib ^
    /FIXED /MERGE:.rdata=.text /MERGE:.data=.text /ignore:4254

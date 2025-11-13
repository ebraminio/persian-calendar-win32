cl main.cc /Fepersian-calendar.exe ^
    /utf-8 /MT /Os /O1 /GL /Gy /GS- /EHs-c- /GR- ^
    /volatile:iso /Zl /GF- /Gm- /fp:fast /d2finline- ^
    /link /RELEASE /DEBUG:NONE /MERGE:.rdata=.text /NODEFAULTLIB ^
    /ENTRY:"_start" /SUBSYSTEM:WINDOWS,5.01 /OPT:REF /OPT:ICF ^
    /INCREMENTAL:NO /LTCG:OFF kernel32.lib user32.lib shell32.lib gdi32.lib shlwapi.lib advapi32.lib

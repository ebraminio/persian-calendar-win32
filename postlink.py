#!/usr/bin/env python3
import pefile
from pathlib import Path

exe_path = str(Path("persian-calendar.exe").resolve())
pe = pefile.PE(exe_path)
pe.FILE_HEADER.TimeDateStamp = 0
opt = pe.OPTIONAL_HEADER
# 4.0 is Windows 4, the actual support is from Windows XP
opt.MajorOperatingSystemVersion = 4
opt.MinorOperatingSystemVersion = 0
opt.MajorSubsystemVersion = 4
opt.MinorSubsystemVersion = 0
pe.write(exe_path)

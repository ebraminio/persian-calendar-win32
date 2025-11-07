Installation
------------

* Find `persian-calendar.exe` from release section of GitHub
* Press Windows+R
* Open `shell:startup`
* Drop the persian-calendar.exe on the folder.

Build
-----

* Windows with MSVC

Find and 'Developer Command Prompt for VS 2022' in start menu, cd into the cloned folder and run build.bat.

* MingW in Linux/macOS
<<<<<<< HEAD
`./build-mingw.sh && wine persian-calendar.exe`
=======
`./build.sh && wine persian-calendar.exe`
>>>>>>> abc6110 (Make msvc build the canonical build)

Same .bat file should be usable in Windows also if `i686-w64-mingw32-gcc` can be found on PATH, download it from https://github.com/niXman/mingw-builds-binaries/ for example.

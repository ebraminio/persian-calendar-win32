The most simple way to display Persian calendar as a tray icon in your Windows machine.

ساده‌ترین برنامهٔ نمایش تقویم فارسی به‌عنوان یک آیکون در نوار اعلان ویندوز.

It looks something like this

<img width="372" height="231" alt="image" src="https://github.com/user-attachments/assets/e15b9ce4-16f1-4a78-9309-8e431eb523a9" />

and it has a support from Windows XP to Windows 11's HiDPI and dark mode for the tray menu. 

Installation
------------

* Find a `persian-calendar.exe` from the latest version of https://github.com/persian-calendar/persian-calendar-win32/releases
* Press Win+E (brings the Explorer) then Ctrl+L (opens the location bar), then type `shell:startup` and enter
* Drop the `persian-calendar.exe` on the folder

Build
-----

* Windows with MSVC

Find and 'Developer Command Prompt for VS 2022' in start menu, cd into the cloned folder and run build.bat.

* MingW in Linux/macOS

`./build.sh`

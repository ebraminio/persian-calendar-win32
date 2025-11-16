#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <wchar.h>
#include <shellscalingapi.h>
#include <stdint.h>
#include "persian-calendar.h"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

// A logger with CRT that works with wine also, so let's have it around
// static void log(const char *s) {
//     DWORD written;
//     WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), s, (DWORD)lstrlenA(s), &written, NULL);
// }

static HICON create_text_icon(HDC hdc, const wchar_t *text, bool black_background)
{
    const int size = 128;
    HBITMAP hbmColor = CreateCompatibleBitmap(hdc, size, size);
    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, NULL);

    HDC memDC = CreateCompatibleDC(hdc);
    HGDIOBJ oldBmp = SelectObject(memDC, hbmColor);
    RECT rc = {0, 0, size, size};

    HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memDC, &rc, bgBrush);
    DeleteObject(bgBrush);

    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));

    HFONT hFont = CreateFontW(
        -size + 4, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Calibri");
    HFONT oldFont = (HFONT)SelectObject(memDC, hFont);

    DrawTextW(memDC, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(memDC, hbmMask);

    if (!black_background)
        FillRect(memDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    SetTextColor(memDC, RGB(0, 0, 0));
    DrawTextW(memDC, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(memDC, oldFont);
    DeleteObject(hFont);

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = hbmColor;
    iconInfo.hbmMask = hbmMask;

    HICON hIcon = CreateIconIndirect(&iconInfo);

    DeleteObject(hbmColor);
    DeleteObject(hbmMask);
    return hIcon;
}

static bool local_digits = true;
static bool black_background = true;
static void apply_local_digits(wchar_t *buf)
{
    if (local_digits)
        for (unsigned i = 0; buf[i]; ++i)
            buf[i] += L'۰' - L'0';
}

static HMENU menu = 0;
const unsigned menu_id_start = 1000;
static unsigned local_digits_id = 0;
static unsigned black_background_id = 0;
static unsigned exit_id = 0;
inline void create_menu(wchar_t *date)
{
    HMENU old_menu = menu;
    menu = CreatePopupMenu();
    unsigned id = menu_id_start;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFOW);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = MFS_DISABLED;
        item.wID = id;
        item.dwTypeData = date;
        InsertMenuItemW(menu, id, TRUE, &item);
    }
    ++id;
    InsertMenuW(menu, id++, MF_SEPARATOR, TRUE, NULL);
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFOW);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = local_digits ? MFS_CHECKED : 0;
        item.wID = id;
        item.dwTypeData = const_cast<wchar_t *>(L"اعداد فارسی");
        InsertMenuItemW(menu, id, TRUE, &item);
        local_digits_id = id;
    }
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFOW);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = black_background ? MFS_CHECKED : 0;
        item.wID = id;
        item.dwTypeData = const_cast<wchar_t *>(L"پیش‌زمینهٔ سیاه آیکون");
        InsertMenuItemW(menu, id, TRUE, &item);
        black_background_id = id;
    }
    ++id;
    InsertMenuW(menu, id++, MF_SEPARATOR, TRUE, NULL);
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFOW);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;
        item.dwTypeData = const_cast<wchar_t *>(L"خروج");
        InsertMenuItemW(menu, id, TRUE, &item);
        exit_id = id;
    }
    ++id;
    if (old_menu)
        DestroyMenu(old_menu);
}

inline uint32_t today_jdn()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return gregorian_to_jdn(st.wYear, st.wMonth, st.wDay);
}

const wchar_t *months[] = {
    L"فروردین",
    L"اردیبهشت",
    L"خرداد",
    L"تیر",
    L"مرداد",
    L"شهریور",
    L"مهر",
    L"آبان",
    L"آذر",
    L"دی",
    L"بهمن",
    L"اسفند",
};
const wchar_t *weekdays[] = {
    L"شنبه",
    L"یکشنبه",
    L"دوشنبه",
    L"سه‌شنبه",
    L"چهارشنبه",
    L"پنجشنبه",
    L"جمعه",
};
void update(HWND hwnd, NOTIFYICONDATAW *notify_icon_data)
{
    uint32_t jdn = today_jdn();
    persian_date_t date = jdn_to_persian(jdn);

    wchar_t day[10];
    wnsprintfW(day, sizeof(day), L"%d", date.day);
    apply_local_digits(day);

    wchar_t month[10];
    wnsprintfW(month, sizeof(month), L"%d", date.month);
    apply_local_digits(month);

    wchar_t year[10];
    wnsprintfW(year, sizeof(year), L"%d", date.year);
    apply_local_digits(year);

    wnsprintfW(notify_icon_data->szTip, sizeof(notify_icon_data->szTip),
               L"%ls، %ls %ls/%ls %ls",
               weekdays[(jdn + 3) % 7], day, months[date.month - 1], month, year);

    // szTip allocated string is both used for the tooltip and first item of the menu
    create_menu(notify_icon_data->szTip);

    HDC hdc = GetDC(hwnd);
    HICON icon = create_text_icon(hdc, day, black_background);
    ReleaseDC(hwnd, hdc);
    if (notify_icon_data->hIcon)
        DestroyIcon(notify_icon_data->hIcon);
    notify_icon_data->hIcon = icon;
    Shell_NotifyIconW(NIM_MODIFY, notify_icon_data);
}

#define appId L"PersianCalendarWin32"
struct Registry
{
    Registry()
    {
        LONG status = RegCreateKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\" appId,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE | KEY_READ,
            NULL,
            &key,
            NULL);
        if (status != ERROR_SUCCESS)
            key = NULL;
    }

    void init_global_variables() const
    {
        if (!key)
            return;
        DWORD value = 0;
        DWORD size = sizeof(DWORD);
        DWORD type = 0;

        if (RegQueryValueExW(key, local_digits_key, NULL, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD)
            local_digits = !!value;

        if (RegQueryValueExW(key, black_background_key, NULL, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD)
            black_background = !!value;
    }

    void set_local_digits(bool value) const
    {
        set_bool(local_digits_key, value);
    }

    void set_black_background(bool value) const
    {
        set_bool(black_background_key, value);
    }

    ~Registry()
    {
        if (key)
            RegCloseKey(key);
    }

private:
    HKEY key;

    void set_bool(wchar_t *name, bool value) const
    {
        if (!key)
            return;
        DWORD dword = value ? 1 : 0;
        RegSetValueExW(
            key,
            name,
            0,
            REG_DWORD,
            reinterpret_cast<const BYTE *>(&dword),
            sizeof(DWORD));
    }

    constexpr static wchar_t *local_digits_key = const_cast<wchar_t *>(L"LocalDigits");
    constexpr static wchar_t *black_background_key = const_cast<wchar_t *>(L"BlackBackground");
};

NOTIFYICONDATAW notify_icon_data = {};
#define ID_TIMER 1
#define ID_NOTIFY_ICON_CLICK (WM_USER + 1)
static LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_TIMER:
        update(hwnd, &notify_icon_data);
        break;
    case ID_NOTIFY_ICON_CLICK:
        if (lparam == WM_LBUTTONUP || lparam == WM_RBUTTONUP)
        {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hwnd);
            BOOL cmd = TrackPopupMenuEx(menu, TPM_RIGHTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_LAYOUTRTL,
                                        p.x, p.y, hwnd, 0);
            SendMessageW(hwnd, WM_COMMAND, (WPARAM)cmd, 0);
        }
        break;
    case WM_COMMAND:
        if (wparam >= menu_id_start)
        {
            MENUITEMINFOW item;
            item.cbSize = sizeof(MENUITEMINFOW);
            item.fMask = MIIM_ID | MIIM_DATA;
            if (GetMenuItemInfoW(menu, wparam, FALSE, &item))
            {
                if (item.wID == local_digits_id)
                {
                    local_digits = !local_digits;
                    update(hwnd, &notify_icon_data);
                    Registry().set_local_digits(local_digits);
                }
                else if (item.wID == black_background_id)
                {
                    black_background = !black_background;
                    update(hwnd, &notify_icon_data);
                    Registry().set_black_background(black_background);
                }
                else if (item.wID == exit_id)
                    PostQuitMessage(0);
            }
            return 0;
        }
        break;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

WNDCLASSEXW wc = {};
extern "C" void _start()
{
    HANDLE mutex = CreateMutexW(0, 0, const_cast<wchar_t *>(appId));
    if (!mutex || GetLastError() == ERROR_ALREADY_EXISTS)
        return;
    HMODULE module = GetModuleHandleW(0);

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hInstance = module;
    wc.lpfnWndProc = window_procedure;
    wc.lpszClassName = appId;
    if (!RegisterClassExW(&wc))
        ExitProcess(1);

    HWND hwnd = CreateWindowExW(0, appId, 0, 0, 0, 0, 0, 0, 0, 0, module, 0);
    if (!hwnd)
        ExitProcess(1);

    {
        HMODULE user32 = LoadLibraryW(L"user32.dll");
        if (user32)
        {
            typedef BOOL(WINAPI * func_t)(DPI_AWARENESS_CONTEXT);
            func_t func = (func_t)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
            if (func)
                func(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            FreeLibrary(user32);
        }
    }
    {
        HMODULE uxtheme = LoadLibraryW(L"uxtheme.dll");
        if (uxtheme)
        {
            typedef INT(WINAPI * func_t)(INT); // SetPreferredAppMode's signature, is in 135 of uxtheme
            func_t func = (func_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(135));
            if (func)
                func(/*Allow dark*/ 1);
            FreeLibrary(uxtheme);
        }
    }

    Registry().init_global_variables();
    notify_icon_data.cbSize = sizeof(NOTIFYICONDATAW);
    notify_icon_data.hWnd = hwnd;
    notify_icon_data.uCallbackMessage = ID_NOTIFY_ICON_CLICK;
    notify_icon_data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    notify_icon_data.uID = 0;
    Shell_NotifyIconW(NIM_ADD, &notify_icon_data);
    update(hwnd, &notify_icon_data);
    SetTimer(hwnd, ID_TIMER, 60000, 0);

    MSG msg;
    while (GetMessageW(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Shell_NotifyIconW(NIM_DELETE, &notify_icon_data);
    DestroyIcon(notify_icon_data.hIcon);

    UnregisterClassW(appId, module);
    ExitProcess(0);
}

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <wchar.h>
#include "persian-calendar.h"

// static void log(const char *s) {
//     DWORD written;
//     WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), s, (DWORD)lstrlenA(s), &written, NULL);
// }

extern "C" void* memset(void* s, int c, size_t sz) {
    char *p = (char *)s;
    char x = c & 0xFF;
    while (sz--) *p++ = x;
    return s;
}

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

static int get_today_fixed()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    int y = st.wYear;
    int m = st.wMonth;
    int d = st.wDay;
    if (m < 3)
    {
        y--;
        m += 12;
    }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + d - 1 - 305;
}

static bool local_digits = true;
static bool black_background = true;
static void apply_local_digits(wchar_t *buf)
{
    if (local_digits)
        for (unsigned i = 0; buf[i]; ++i)
            buf[i] = buf[i] - L'0' + L'۰';
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

static HMENU hmenu = 0;
const wchar_t rlm = 0x200F;
const int menu_id_start = 1000;
static wchar_t *local_digits_key = const_cast<LPWSTR>(L"LocalDigits");
static wchar_t *black_background_key = const_cast<LPWSTR>(L"BlackBackground");
static int local_digits_id = 0;
static int black_background_id = 0;
static int exit_id = 0;
static void create_menu(PersianDate date)
{
    HMENU prevhmenu = hmenu;
    hmenu = CreatePopupMenu();
    int id = menu_id_start;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;

        wchar_t day[10];
        wnsprintfW(day, sizeof(day), L"%d", date.day);
        apply_local_digits(day);

        wchar_t year[10];
        wnsprintfW(year, sizeof(year), L"%d", date.year);
        apply_local_digits(year);

        wchar_t buf[255];
        wnsprintfW(buf, sizeof(buf), L"%lc%ls %ls %ls", rlm,
                   day, months[date.month - 1], year);

        item.dwTypeData = buf;
        item.fState |= MFS_DISABLED;
        InsertMenuItemW(hmenu, id, TRUE, &item);
        ++id;
    }
    InsertMenuW(hmenu, id++, MF_SEPARATOR, TRUE, const_cast<LPWSTR>(L""));
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;
        item.dwTypeData = const_cast<LPWSTR>(L"اعداد فارسی");
        if (local_digits)
            item.fState |= MFS_CHECKED;
        InsertMenuItemW(hmenu, id, TRUE, &item);
        local_digits_id = id;
        ++id;
    }
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;
        item.dwTypeData = const_cast<LPWSTR>(L"پیش‌زمینهٔ سیاه آیکون");
        if (black_background)
            item.fState |= MFS_CHECKED;
        InsertMenuItemW(hmenu, id, TRUE, &item);
        black_background_id = id;
        ++id;
    }
    InsertMenuW(hmenu, id++, MF_SEPARATOR, TRUE, const_cast<LPWSTR>(L""));
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;
        item.dwTypeData = const_cast<LPWSTR>(L"خروج");
        InsertMenuItemW(hmenu, id, TRUE, &item);
        exit_id = id;
        ++id;
    }
    if (prevhmenu)
        DestroyMenu(prevhmenu);
}

static void update(HWND hwnd, NOTIFYICONDATA *nid)
{
    PersianDate date = persian_fast_from_fixed(get_today_fixed());
    create_menu(date);
    HDC hdc = GetDC(hwnd);

    wchar_t day[10];
    wnsprintfW(day, sizeof(day), L"%d", date.day);
    apply_local_digits(day);

    HICON icon = create_text_icon(hdc, day, black_background);
    ReleaseDC(hwnd, hdc);
    if (nid->hIcon)
        DestroyIcon(nid->hIcon);
    nid->hIcon = icon;
    Shell_NotifyIcon(NIM_MODIFY, nid);
}

template <typename Action>
static int with_registry(Action &&action)
{
    const wchar_t *subKey = L"Software\\PersianCalendarWin32";
    HKEY hKey;
    LONG status = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        subKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE | KEY_READ,
        NULL,
        &hKey,
        NULL);

    if (status != ERROR_SUCCESS)
        return 1;

    action(hKey);

    RegCloseKey(hKey);
    return 0;
}

static void store_bool_in_registry(HKEY hKey, LPCWSTR key, bool value)
{
    DWORD dword = value;
    RegSetValueExW(
        hKey,
        key,
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE *>(&dword),
        sizeof(DWORD));
}

static void init_global_variable(HKEY hKey)
{
    DWORD value = 0;
    DWORD size = sizeof(DWORD);
    DWORD type = 0;

    if (RegQueryValueExW(hKey, local_digits_key, NULL, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD)
        local_digits = value;

    if (RegQueryValueExW(hKey, black_background_key, NULL, &type, reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS && type == REG_DWORD)
        black_background = value;
}

NOTIFYICONDATA nid = {};
#define ID_TIMER 1
#define ID_NOTIFY_ICON_CLICK (WM_USER + 1)
LPCWSTR app = const_cast<LPWSTR>(L"Persian Calendar");
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
        update(hwnd, &nid);
        break;
    case ID_NOTIFY_ICON_CLICK:
        if (lparam == WM_LBUTTONUP || lparam == WM_RBUTTONUP)
        {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hwnd);
            WORD cmd = TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY | TPM_LAYOUTRTL,
                                      p.x, p.y, 0, hwnd, 0);
            SendMessage(hwnd, WM_COMMAND, cmd, 0);
        }
        break;
    case WM_COMMAND:
        if (wparam >= menu_id_start)
        {
            MENUITEMINFO item;
            item.cbSize = sizeof(MENUITEMINFO);
            item.fMask = MIIM_ID | MIIM_DATA;
            if (GetMenuItemInfo(hmenu, wparam, FALSE, &item))
            {
                if (item.wID == local_digits_id)
                {
                    local_digits = !local_digits;
                    update(hwnd, &nid);
                    with_registry([](HKEY hKey)
                                  { store_bool_in_registry(hKey, local_digits_key, local_digits); });
                }
                else if (item.wID == black_background_id)
                {
                    black_background = !black_background;
                    update(hwnd, &nid);
                    with_registry([](HKEY hKey)
                                  { store_bool_in_registry(hKey, black_background_key, black_background); });
                }
                else if (item.wID == exit_id)
                    PostQuitMessage(0);
            }
            return 0;
        }
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

BOOL EnableDarkMode(HWND hwnd, BOOL enable)
{
    typedef BOOL (WINAPI *AllowDarkModeForWindowProc)(HWND, BOOL);
    HMODULE hUxtheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxtheme) return FALSE;

    auto fn = (AllowDarkModeForWindowProc)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135)); // undocumented
    if (!fn) return FALSE;

    return fn(hwnd, enable);
}

#ifdef _WIN64
extern "C" void _WinMainCRTStartup()
#else
extern "C" void WinMainCRTStartup()
#endif
{
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(0);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = app;
    if (!RegisterClassExW(&wc))
        ExitProcess(1);

    HWND hwnd = CreateWindowExW(0, app, 0, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(0), 0);
    if (!hwnd)
        ExitProcess(1);

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    EnableDarkMode(hwnd, TRUE);

    with_registry([](HKEY hKey)
                  { init_global_variable(hKey); });
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uCallbackMessage = ID_NOTIFY_ICON_CLICK;
    nid.uFlags = NIF_MESSAGE | NIF_ICON;
    nid.uID = 0;
    Shell_NotifyIcon(NIM_ADD, &nid);
    update(hwnd, &nid);
    SetTimer(hwnd, ID_TIMER, 60000, 0);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);
    DestroyIcon(nid.hIcon);

    UnregisterClassW(app, GetModuleHandle(0));
    ExitProcess(0);
}

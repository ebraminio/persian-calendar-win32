#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <shellapi.h>
#include <wchar.h>
#include "persian-calendar.h"

HICON CreateTextIcon(HDC hdc, const wchar_t *text)
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

static int getTodayFixed()
{
    SYSTEMTIME st;
    FILETIME ft;
    char buffer[256];
    GetLocalTime(&st);
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    LONGLONG fileTimeValue = uli.QuadPart;
    return (int)(fileTimeValue / (10000000LL * 60 * 60 * 24)) + 584389;
}

static bool local_digits = true;
void apply_local_digits(wchar_t *buf)
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
const wchar_t lrm = 0x200F;
const int menu_id_start = 1000;
static int local_digits_id = 0;
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
        swprintf(day, sizeof(day), L"%d", date.day);
        apply_local_digits(day);

        wchar_t year[10];
        swprintf(year, sizeof(year), L"%d", date.year);
        apply_local_digits(year);

        wchar_t buf[255];
        swprintf(buf, sizeof(buf), L"%lc%ls %ls %ls", lrm,
                 day, months[date.month - 1], year);

        item.dwTypeData = buf;
        item.fState |= MFS_DISABLED;
        InsertMenuItemW(hmenu, id, TRUE, &item);
        ++id;
    }
    InsertMenu(hmenu, id++, MF_SEPARATOR, TRUE, "");
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;
        item.dwTypeData = L"اعداد فارسی";
        if (local_digits)
            item.fState |= MFS_CHECKED;
        InsertMenuItemW(hmenu, id, TRUE, &item);
        local_digits_id = id;
        ++id;
    }
    InsertMenu(hmenu, id++, MF_SEPARATOR, TRUE, "");
    ++id;
    {
        MENUITEMINFOW item = {};
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
        item.fType = 0;
        item.fState = 0;
        item.wID = id;
        item.dwTypeData = L"خروج";
        InsertMenuItemW(hmenu, id, TRUE, &item);
        exit_id = id;
        ++id;
    }
    if (prevhmenu)
        DestroyMenu(prevhmenu);
}

static void update(HWND hwnd, NOTIFYICONDATA *nid)
{
    PersianDate date = persian_fast_from_fixed(getTodayFixed());
    create_menu(date);
    HDC hdc = GetDC(hwnd);

    wchar_t day[10];
    swprintf(day, sizeof(day), L"%d", date.day);
    apply_local_digits(day);

    HICON icon = CreateTextIcon(hdc, day);
    ReleaseDC(hwnd, hdc);
    if (nid->hIcon)
        DestroyIcon(nid->hIcon);
    nid->hIcon = icon;
    Shell_NotifyIcon(NIM_MODIFY, nid);
}

NOTIFYICONDATA nid = {};
#define ID_TIMER 1
#define ID_NOTIFY_ICON_CLICK (WM_USER + 1)
const char *app = "Persian Calendar";
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
            WORD cmd = TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
                                      p.x, p.y, 0, hwnd, 0);
            SendMessage(hwnd, WM_COMMAND, cmd, 0);
        }
        break;
    case WM_COMMAND:
        if (wparam >= menu_id_start)
        {
            MENUITEMINFO item = {
                .cbSize = sizeof(MENUITEMINFO),
                .fMask = MIIM_ID | MIIM_DATA,
            };
            if (GetMenuItemInfo(hmenu, wparam, FALSE, &item))
            {
                if (item.wID == local_digits_id)
                {
                    local_digits = !local_digits;
                    update(hwnd, &nid);
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

#ifdef _WIN64
void _WinMainCRTStartup()
#else
void WinMainCRTStartup()
#endif
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(0);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = app;
    if (!RegisterClassEx(&wc))
        ExitProcess(1);

    HWND hwnd = CreateWindowEx(0, app, 0, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(0), 0);
    if (!hwnd)
        ExitProcess(1);

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

    UnregisterClass(app, GetModuleHandle(0));
    ExitProcess(0);
}

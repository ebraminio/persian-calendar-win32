#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <shellapi.h>
#include "persian-calendar.h"

HICON CreateTextIcon(HDC hdc, const wchar_t *text)
{
    const int size = 128;
    HBITMAP hbmColor = CreateCompatibleBitmap(hdc, size, size);
    HBITMAP hbmMask = CreateCompatibleBitmap(hdc, size, size);
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

    ICONINFO iconInfo = {0};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = hbmColor;
    iconInfo.hbmMask = hbmMask;

    HICON hIcon = CreateIconIndirect(&iconInfo);

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
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

const wchar_t *days[] = {
    L"۰", L"۱", L"۲", L"۳", L"۴", L"۵", L"۶", L"۷", L"۸", L"۹", L"۱۰", L"۱۱",
    L"۱۲", L"۱۳", L"۱۴", L"۱۵", L"۱۶", L"۱۷", L"۱۸", L"۱۹", L"۲۰", L"۲۱",
    L"۲۲", L"۲۳", L"۲۴", L"۲۵", L"۲۶", L"۲۷", L"۲۸", L"۲۹", L"۳۰", L"۳۱",
};

static void update(HWND hwnd, NOTIFYICONDATA *nid)
{
    PersianDate date = persian_fast_from_fixed(getTodayFixed());
    HDC hdc = GetDC(hwnd);
    HICON icon = CreateTextIcon(hdc, days[date.day]);
    ReleaseDC(hwnd, hdc);

    if (nid->hIcon) DestroyIcon(nid->hIcon);
    nid->hIcon = icon;
    Shell_NotifyIcon(NIM_MODIFY, nid);
}

NOTIFYICONDATA nid = {0};
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
            DestroyWindow(hwnd);
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
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(0);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = app;
    if (!RegisterClassEx(&wc)) ExitProcess(1);

    HWND hwnd = CreateWindowEx(0, app, 0, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(0), 0);
    if (!hwnd) ExitProcess(1);

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

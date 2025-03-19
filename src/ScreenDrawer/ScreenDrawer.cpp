#include "ScreenDrawer.hpp"
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>
#include <strsafe.h>

#define WM_CUSTOM_DRAW (WM_USER + 1)
#define WM_TRAYICON (WM_USER + 2)
#define IDM_EXIT 1000
#define IDM_TOPMOST 1001
#define IDM_NOTOPMOST 1002
#define IDM_FIRST 1003
#define IDM_LAST 1004

ScreenDrawer::ScreenDrawer(HINSTANCE hInstance) : hInstance(hInstance)
{
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

ScreenDrawer::~ScreenDrawer()
{
    try
    {
        if (windowsHandle)
        {
            DestroyWindow(windowsHandle);
        }
        Gdiplus::GdiplusShutdown(gdiplusToken);
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in destructor: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception in destructor" << std::endl;
    }
}

void ScreenDrawer::createWindowF(const char *CLASS_NAME)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    this->screenHeight = screenHeight;
    this->screenWidth = screenWidth;
    std::thread sdWindowThread(&ScreenDrawer::_createWindow, this, CLASS_NAME, screenWidth, screenHeight);
    sdWindowThread.detach();
}

void ScreenDrawer::createWindow(const char *CLASS_NAME, int width, int height)
{
    this->screenHeight = height;
    this->screenWidth = width;
    std::thread sdWindowThread(&ScreenDrawer::_createWindow, this, CLASS_NAME, width, height);
    sdWindowThread.detach();
}

void ScreenDrawer::_createWindow(const char *CLASS_NAME, int width, int height)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
     // Convert to std::string
     std::string str = CLASS_NAME;
    
     // Convert std::string to char*
     this->CLASS_NAME = &str[0];
    if (!RegisterClass(&wc))
    {
        throw std::runtime_error("Failed to register window class");
    }

    windowsHandle = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        CLASS_NAME,
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,
        NULL,
        hInstance,
        this
    );

    if (!windowsHandle)
    {
        throw std::runtime_error("Failed to create window");
    }

    CreateLayeredWindow(windowsHandle, width, height, drawFunction);

    ShowWindow(windowsHandle, SW_SHOWDEFAULT);
    UpdateWindow(windowsHandle);

    InitTrayIcon();

    isRunnnig = true;
    MSG msg = {};
    while (isRunnnig)
    {
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // Add Sleep to yield CPU time
            // Sleep(100);
        }
    }
}

LRESULT CALLBACK ScreenDrawer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ScreenDrawer *pThis = reinterpret_cast<ScreenDrawer *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg)
    {
    case WM_CREATE:
    {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        pThis = reinterpret_cast<ScreenDrawer *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (pThis && pThis->drawFunction)
        {
            Gdiplus::Graphics graphics(hdc);
            pThis->drawFunction(&graphics, pThis->screenWidth, pThis->screenHeight);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_TIMER:
    case WM_CUSTOM_DRAW:
    {
        if (pThis)
        {
            pThis->CreateLayeredWindow(hwnd, pThis->screenWidth, pThis->screenHeight, pThis->drawFunction);
            SetWindowPos(hwnd, pThis->zPosition, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        return 0;
    }

    case WM_TRAYICON:
    {
        if (LOWORD(lParam) == WM_RBUTTONDOWN && pThis)
        {
            pThis->ShowTrayMenu(hwnd);
        }
        return 0;
    }

    case WM_COMMAND:
    {
        if (pThis && pThis->menuCallbacks.find(LOWORD(wParam)) != pThis->menuCallbacks.end())
        {
            pThis->menuCallbacks[LOWORD(wParam)]();
        }
        if (LOWORD(wParam) == IDM_EXIT)
        {
            pThis->isRunnnig = false;
            PostQuitMessage(0);
            DestroyWindow(hwnd);
        }
        return 0;
    }

    case WM_DESTROY:
    {
        pThis->isRunnnig = false;
        PostQuitMessage(0);
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void ScreenDrawer::CreateLayeredWindow(HWND hwnd, int width, int height, void (*drawFunction)(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight))
{
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, width, height);
    HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

    Gdiplus::Graphics graphics(hdcMem);
    if (drawFunction)
    {
        drawFunction(&graphics, width, height);
    }

    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptSrc = {0, 0};
    SIZE sizeWnd = {width, height};
    POINT ptDst = {0, 0};

    UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
}

void ScreenDrawer::updateScreen()
{
    if (windowsHandle)
    {
        PostMessage(windowsHandle, WM_CUSTOM_DRAW, 0, 0);
    }
}

void ScreenDrawer::setDrawFunction(void (*drawFunction)(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight))
{
    this->drawFunction = drawFunction;
}

void ScreenDrawer::setZOrder(HWND hwndInsertAfter)
{
    zPosition = hwndInsertAfter;
    if (windowsHandle)
    {
        SetWindowPos(windowsHandle, zPosition, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}

void ScreenDrawer::showNotificationBar()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void ScreenDrawer::hideNotificationBar()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ScreenDrawer::InitTrayIcon()
{
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = windowsHandle;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), CLASS_NAME);
}

void ScreenDrawer::ShowTrayMenu(HWND hwnd)
{
    hMenu = CreatePopupMenu();
    for (auto &item : menuItems)
    {
        AppendMenu(hMenu, MF_STRING, item.first,item.second );
    }
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_TOPMOST, "Top Most");
    AppendMenu(hMenu, MF_STRING, IDM_NOTOPMOST, "Not Top Most");
    AppendMenu(hMenu, MF_STRING, IDM_FIRST, "First");
    AppendMenu(hMenu, MF_STRING, IDM_LAST, "Last");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_EXIT, "Exit");
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

bool ScreenDrawer::isWindowRunning()
{
    return isRunnnig;
}

void ScreenDrawer::destroyWindow()
{
    if (windowsHandle)
    {
        DestroyWindow(windowsHandle);
        isRunnnig = false;
    }
}

void ScreenDrawer::addMenuItem(UINT uIDNewItem, const char *lpNewItem, std::function<void()> callback)
{
    
         menuItems[uIDNewItem] = lpNewItem;
        menuCallbacks[uIDNewItem] = callback;
    
}
void ScreenDrawer::addMenuSeparator()
{
    if (hMenu)
    {
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    }
}
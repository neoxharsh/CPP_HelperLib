#include <windows.h>
#include <gdiplus.h>
#include <random>
#include <time.h>
#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

HWND hwndLayered; // Global variable to store the handle of the layered window

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Use GDI+ for drawing
        Graphics graphics(hdc);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);

        // Clear the background with transparency
        graphics.Clear(Color(0, 0, 0, 0));

        // Draw a transparent red rectangle
        Color transparentRed(128, rand() % 255, rand() % 255, rand() % 255); // 128 is the alpha value (0-255), where 0 is fully transparent and 255 is fully opaque
        SolidBrush brush(transparentRed);
        graphics.FillRectangle(&brush, 50, 50, 200, 200);

        EndPaint(hwnd, &ps);
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void CreateLayeredWindow(HWND hwnd) {
    // Get the window DC
    HDC hdc = GetDC(hwnd);

    // Create a memory DC and compatible bitmap
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, 300, 300); // Adjust size as needed
    HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

    // Use GDI+ to draw on the memory DC
    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    // Clear the background with transparency
    graphics.Clear(Color(0, 0, 0, 0));

    // Draw a transparent red rectangle
    Color transparentRed(128,rand() % 255, rand() % 255, rand() % 255); // 128 is the alpha value (0-255), where 0 is fully transparent and 255 is fully opaque
    SolidBrush brush(transparentRed);
    graphics.FillRectangle(&brush, 50, 50, 200, 200);

    // Use UpdateLayeredWindow to set the layered attributes
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255; // Adjust alpha blending level (0-255)
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { 300, 300 }; // Size of the window
    POINT ptDst = { 0, 0 };

    UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    // Clean up
    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
}

void ChangeWindowZOrder(HWND hwnd, HWND hwndInsertAfter) {
    // Change the window's z-order
    SetWindowPos(hwnd, hwndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    srand(time(0));
    const char CLASS_NAME[] = "LayeredWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // No background brush to make the window transparent

    RegisterClass(&wc);

    hwndLayered = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, // Extended styles
        CLASS_NAME,  // Window class name
        "Layered Window Example",  // Window title
        WS_POPUP,  // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300,  // Initial position and size
        NULL,  // Parent window
        NULL,  // Menu handle
        hInstance,  // Instance handle
        NULL  // Additional application data
    );

    if (hwndLayered == NULL) {
        return 0;
    }

    // Create the layered window
    CreateLayeredWindow(hwndLayered);

    ShowWindow(hwndLayered, nCmdShow);
    UpdateWindow(hwndLayered);

    // Example of changing z-order after a delay
    Sleep(100); // Sleep for 2 seconds
    HWND hwndOther = FindWindow(NULL, "Untitled - Notepad"); // Example of finding another window
    if (hwndOther) {
        ChangeWindowZOrder(hwndLayered, hwndOther); // Move layered window behind Notepad window
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);

    return 0;
}

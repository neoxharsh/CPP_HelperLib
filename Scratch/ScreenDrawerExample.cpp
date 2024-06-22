#include <windows.h>
#include <gdiplus.h>
#include <thread>
#include <ctime>
#include <iostream> // For user input
#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

class LayeredWindow {
public:
    LayeredWindow(HINSTANCE hInstance);
    ~LayeredWindow();

    void Show();
    void UpdateColor();
    void RunMessageLoop();
    void StopMessageLoop();
    void ChangeZOrder(HWND hwndInsertAfter);

private:
    HWND hwnd;
    COLORREF currentColor;
    HINSTANCE hInstance;
    std::thread messageLoopThread;
    ULONG_PTR gdiplusToken;

    void CreateLayeredContent();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

LayeredWindow::LayeredWindow(HINSTANCE hInstance) : hInstance(hInstance), currentColor(RGB(255, 0, 0)) {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    const char CLASS_NAME[] = "LayeredWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // No background brush to make the window transparent

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, // Extended styles
        CLASS_NAME,  // Window class name
        "Layered Window Example",  // Window title
        WS_POPUP,  // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300,  // Initial position and size
        NULL,  // Parent window
        NULL,  // Menu handle
        hInstance,  // Instance handle
        this  // Additional application data (pointer to this instance)
    );

    if (hwnd == NULL) {
        throw std::runtime_error("Failed to create window");
    }

    CreateLayeredContent();
}

LayeredWindow::~LayeredWindow() {
    GdiplusShutdown(gdiplusToken);
}

void LayeredWindow::Show() {
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    SetTimer(hwnd, 1, 1000, NULL); // Update color every second

    messageLoopThread = std::thread(&LayeredWindow::RunMessageLoop, this);
    
}


void LayeredWindow::UpdateColor() {
    currentColor = RGB(rand() % 256, rand() % 256, rand() % 256);
    CreateLayeredContent();
}

void LayeredWindow::RunMessageLoop() {
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(hwnd, 1); // Kill the timer when the message loop exits
}

void LayeredWindow::StopMessageLoop() {
    PostThreadMessage(GetThreadId(messageLoopThread.native_handle()), WM_QUIT, 0, 0);
    messageLoopThread.join();
}

void LayeredWindow::ChangeZOrder(HWND hwndInsertAfter) {
    SetWindowPos(hwnd, hwndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void LayeredWindow::CreateLayeredContent() {
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, 300, 300);
    HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    Color gdiColor(GetRValue(currentColor), GetGValue(currentColor), GetBValue(currentColor));
    SolidBrush brush(gdiColor);
    graphics.FillRectangle(&brush, 50, 50, 200, 200);

    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { 300, 300 };
    POINT ptDst = { 0, 0 };

    UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK LayeredWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LayeredWindow* pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (LayeredWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (LayeredWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        } break;

        case WM_TIMER: {
            pThis->UpdateColor();
        } break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation

    HINSTANCE hInstance = GetModuleHandle(NULL);

    try {
        LayeredWindow window(hInstance);
        window.Show();

        // Main thread can do other tasks here
        // Example of changing z-order after a delay
        Sleep(2000); // Sleep for 2 seconds
        HWND hwndOther = FindWindow(NULL, "Untitled - Notepad"); // Example of finding another window
        if (hwndOther) {
            window.ChangeZOrder(hwndOther); // Move layered window behind Notepad window
        }

        // Send a custom message after 5 seconds
        Sleep(5000); // Sleep for 5 seconds
        PostMessage(window.hwnd, WM_CUSTOM_UPDATE, 0, 0); // Post the custom message to the window

        // Wait for user input before exiting
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get(); // Wait for user to press Enter

        // Stop the message loop and clean up
        window.StopMessageLoop();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

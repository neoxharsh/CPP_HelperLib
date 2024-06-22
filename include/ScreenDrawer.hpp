#ifndef SCREENDRAWER_HPP
#define SCREENDRAWER_HPP

#include <windows.h>
#include <gdiplus.h>
#include <thread>

#define WINDOW_HANDLE_FIRST        ((HWND)0)
#define WINDOW_HANDLE_LAST         ((HWND)1)
#define WINDOW_HANDLE_TOPMOST      ((HWND)-1)
#define WINDOW_HANDLE_NOTOPMOST    ((HWND)-2)


class ScreenDrawer
{
public:
    ScreenDrawer(HINSTANCE hInstance);
    ~ScreenDrawer();
    
    void createWindowF(const char *CLASS_NAME);
    void createWindow(const char *CLASS_NAME, int width, int height);
    void setDrawFunction(void (*drawFunction)(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight));
    void setZOrder(HWND hwndInsertAfter);
    void updateScreen();
    void showNotificationBar();
    void hideNotificationBar();
    bool isWindowRunning();
    void destroyWindow();

private:
    void _createWindow(const char *CLASS_NAME, int width, int height);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CreateLayeredWindow(HWND hwnd, int width, int height, void (*drawFunction)(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight));
    void InitTrayIcon();
    void ShowTrayMenu(HWND hwnd);

    HINSTANCE hInstance;
    HWND windowsHandle = NULL;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    bool isRunnnig = false;
    int screenWidth = 0;
    int screenHeight = 0;
    void (*drawFunction)(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight) = nullptr;
    HWND zPosition = HWND_TOP;
    NOTIFYICONDATA nid = {};
};

#endif // SCREENDRAWER_HPP

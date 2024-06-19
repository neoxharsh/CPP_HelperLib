#include "KeyboardListner.hpp"
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

KeyboardListener::KeyboardListener()
{
}
KeyboardListener::~KeyboardListener()
{
}

void KeyboardListener::setHotKey(UINT modKey, UINT key)
{
    KeyboardListener::modKey = modKey;
    KeyboardListener::key = key;
}

void KeyboardListener::_callBackThread()
{
    if (!RegisterHotKey(NULL, 1, KeyboardListener::modKey, KeyboardListener::key))
    {
        std::cerr << "Failed to register hotkey" << std::endl;
        return;
    }
    else
    {
        std::cout << "Hotkey Registerd!" << std::endl;
    }
    MSG msg = {0};
    KeyboardListener::keepRunning = true;
    while (KeyboardListener::keepRunning)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_HOTKEY && msg.wParam == 1)
            {
                std::cout << "Hotkey pressed!" << std::endl;
                customCallbackFunction();
            }
        }
    }
}

void KeyboardListener::setCallbackFunction(void (*func)())
{
    customCallbackFunction = func;
}

void KeyboardListener::startHotKeyListener()
{
    KeyboardListener::callBackThread = std::thread(&KeyboardListener::_callBackThread, this);
}

void KeyboardListener::stopHotKeyListener()
{
    KeyboardListener::keepRunning = false;
    if (KeyboardListener::callBackThread.joinable())
    {
        KeyboardListener::callBackThread.join();
    }
}

std::string GetActiveWindowText()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL)
    {
        return "";
    }

    // Get the length of the text
    int length = GetWindowTextLength(hwnd);
    if (length == 0)
    {
        return "";
    }

    // Get the window text
    char *buffer = new char[length + 1];
    GetWindowText(hwnd, buffer, length + 1);
    std::string windowTitle(buffer);
    delete[] buffer;

    // Try to get the selected text from the active window
    HWND focused = GetFocus();
    if (focused == NULL)
    {
        focused = GetForegroundWindow();
    }
    Sleep(250);
    // Send a Ctrl+C key press to copy the selected text to the clipboard
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event('C', 0, 0, 0);
    Sleep(50); // Wait for the clipboard to update
    keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

    // Get the clipboard data
    if (!OpenClipboard(NULL))
    {
        return "";
    }
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL)
    {
        CloseClipboard();
        return "";
    }

    char *clipboardText = static_cast<char *>(GlobalLock(hData));
    if (clipboardText == NULL)
    {
        CloseClipboard();
        return "";
    }

    std::string selectedText(clipboardText);

    GlobalUnlock(hData);
    CloseClipboard();

    return selectedText;
}
#ifndef KEYBOARD_LISTENER_H
#define KEYBOARD_LISTENER_H
#include <windows.h>
#include <iostream>
#include <string>
#include <atomic>
#include <thread>

class KeyboardListener{
    public:
        std::atomic<bool> isHotkeyCallbackThreadRunning;
        void (*customCallbackFunction)();
        KeyboardListener();
        ~KeyboardListener();
        void setHotKey(UINT modKey, UINT key);
        void setCallbackFunction(void (*func)());
        void startHotKeyListener();
        void stopHotKeyListener();
    private:
        std::atomic<bool> keepRunning;
        UINT modKey, key;
        void _callBackThread();
        std::thread callBackThread;
};

std::string GetActiveWindowText();
#endif
#include <KeyboardListner.hpp>

extern "C"{
    __declspec(dllexport) KeyboardListener* KeyboardListener_new() {return new KeyboardListener();};
    __declspec(dllexport) void KeyboardListener_setHotKey(KeyboardListener* kl) {kl->setHotKey(MOD_SHIFT | MOD_WIN, 0x45);} 
    __declspec(dllexport) void KeyboardListener_startHotKeyListener(KeyboardListener* kl) {kl->startHotKeyListener();} 
    __declspec(dllexport) void KeyboardListener_setCallbackFunction(KeyboardListener* kl, void (*func)()) {kl->setCallbackFunction(func);} 
}
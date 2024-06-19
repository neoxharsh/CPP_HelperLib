#include <windows.h>
#include <iostream>
#include <string>
#include "KeyboardListner.hpp"

KeyboardListener kbl;

void customCallback(){
    std::cout <<"Hello" << std::endl;
}
int main() {

    kbl.setHotKey(MOD_SHIFT | MOD_WIN, 0x45);
    kbl.setCallbackFunction(customCallback);
    kbl.startHotKeyListener();

    std::cout << "Press Enter to stop the hotkey listener..." << std::endl;
    std::cin.get();
    kbl.stopHotKeyListener();
    // // Register a hotkey (Ctrl + Shift + A)
    // if (!RegisterHotKey(NULL, 1, MOD_SHIFT | MOD_WIN , 0x45)) {
    //     std::cerr << "Failed to register hotkey" << std::endl;
    //     return 1;
    // }

    // std::cout << "Hotkey registered, waiting for it to be pressed..." << std::endl;

    // // Message loop to listen for the hotkey
    // MSG msg = {0};
    // while (GetMessage(&msg, NULL, 0, 0) != 0) {
    //     if (msg.message == WM_HOTKEY) {
    //         std::cout << "Hotkey pressed!" << std::endl;
    //         std::string selectedText = GetActiveWindowText();
    //         std::cout << "Selected text: " << selectedText << std::endl;
    //         // Do something with the selected text
    //     }
    // }

    // // Unregister the hotkey before exiting
    // UnregisterHotKey(NULL, 1);
    return 0;
}
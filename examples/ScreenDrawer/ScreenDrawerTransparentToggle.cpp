#include "ScreenDrawer.hpp"
#include <iostream>
#include <random>
#include <ctime>

// Define the rectangle properties
int rectWidth = 100;
int rectHeight = 100;
int rectX = 0;
int rectY = 0;
int rectSpeedX = 5;
int rectSpeedY = 5;
ScreenDrawer* screenDrawer = nullptr;
void drawFunction(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight)
{
    graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics->Clear(Gdiplus::Color(50, 0, 0, 0)); // Clear the background with transparency
    
    Gdiplus::SolidBrush sb(Gdiplus::Color(0, 0, 0));
    Gdiplus::Font font(L"Arial", 32);
    std::time_t currentTime = std::time(nullptr);
    std::tm *localTime = std::localtime(&currentTime);
    std::wstring timeString = std::to_wstring(localTime->tm_hour) + L":" + std::to_wstring(localTime->tm_min) + L":" + std::to_wstring(localTime->tm_sec);

    graphics->DrawString(timeString.c_str(), -1, &font, Gdiplus::PointF(screenWidth - 200, 0), &sb);
}

void printHello(){
    std::cout << "Hello" << std::endl;
    screenDrawer->setWindowTransparent(false);
}

int main()
{
   
    screenDrawer = new ScreenDrawer("ScreenDrawerExampleSimple",10, drawFunction);
    screenDrawer->addMenuItem(1, "Hello", printHello);
    int i = 0;
    while (true)
    {
        std::cout << i << std::endl;
        i++;
        Sleep(30000);
        screenDrawer->setWindowTransparent(true);
    }
    

    return 0;
}

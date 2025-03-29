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

void drawFunction(Gdiplus::Graphics *graphics, int screenWidth, int screenHeight)
{
    graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // graphics->Clear(Gdiplus::Color(0, 0, 0, 0)); // Clear the background with transparency
   
    Gdiplus::SolidBrush sb(Gdiplus::Color(0, 0, 0));
    Gdiplus::Font font(L"Arial", 32);
    std::time_t currentTime = std::time(nullptr);
    std::tm *localTime = std::localtime(&currentTime);
    std::wstring timeString = std::to_wstring(localTime->tm_hour) + L":" + std::to_wstring(localTime->tm_min) + L":" + std::to_wstring(localTime->tm_sec);

    graphics->DrawString(timeString.c_str(), -1, &font, Gdiplus::PointF(screenWidth - 200, 0), &sb);

    

    // Update the rectangle position
    rectX += rectSpeedX;
    rectY += rectSpeedY;

    // Check if the rectangle hits the screen boundaries
    if (rectX < 0 || rectX + rectWidth > screenWidth) {
        rectSpeedX *= -1; // Reverse the horizontal direction
    }
    if (rectY < 0 || rectY + rectHeight > screenHeight) {
        rectSpeedY *= -1; // Reverse the vertical direction
    }

    // Draw the rectangle
    Gdiplus::SolidBrush rectBrush(Gdiplus::Color(255, 0, 0));
    Gdiplus::Rect rect(rectX, rectY, rectWidth, rectHeight);
    graphics->FillRectangle(&rectBrush, rect);
}

void printHello(){
    std::cout << "Hello" << std::endl;
    // screenDrawer->setZOrder(WINDOW_HANDLE_NOTOPMOST);
    
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    try
    {
        ScreenDrawer screenDrawer("ScreenDrawerExample",10, drawFunction);
        screenDrawer.addMenuItem(1, "Hello", printHello);
        screenDrawer.joinMainThread();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }



    return 0;
}

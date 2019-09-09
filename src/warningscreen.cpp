#include "warningscreen.h"
#include "menuscreenmanager.h"
#include <iostream>

WarningScreen::WarningScreen(const std::string & warningText, const MenuItemPtr & item, bool isError) : warningText(warningText), isError(isError)
{
    items.push_back(item);
    if(isError)
        std::cerr << warningText << std::endl;
}
WarningScreen::~WarningScreen()
{

}
void WarningScreen::Init()
{
    textures.emplace_back(fontBold, (isError) ? "ERROR!" : "WARNING!", 48, renderer, 640, 80, true, 0xFF, 0 ,0);
    textures.emplace_back(font, warningText, 28, renderer, 640, 360, true);
}

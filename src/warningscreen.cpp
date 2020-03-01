/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "languagemanager.h"
#include "menuscreenmanager.h"
#include "warningscreen.h"
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
    textures.push_back(FontManager::CreateTexture(TextItemType::WARNING_HEADER_TEXT, (isError) ? LanguageManager::GetString("error_title") : LanguageManager::GetString("warning_title"), 48, 640, 80, true));
    textures.push_back(FontManager::CreateTexture(TextItemType::DEFAULT_TEXT, warningText, 28, 640, 360, true));
}

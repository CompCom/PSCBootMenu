/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "guihelper.h"
#include "json.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

void from_json(const json& j, Font& f) {
    j.at("fontPath").get_to(f.fontPath);
    j.at("r").get_to(f.r);
    j.at("g").get_to(f.g);
    j.at("b").get_to(f.b);
}

FontManager::FontManager()
{
#ifdef __arm__
    fonts[DEFAULT_TEXT] = { "/usr/share/fonts/ttf/LiberationMono-Regular.ttf", 0xFF, 0xFF, 0xFF };
#else
    fonts[DEFAULT_TEXT] = { "./root/usr/share/fonts/ttf/LiberationMono-Regular.ttf", 0xFF, 0xFF, 0xFF };
#endif // __arm__
}

FontManager::~FontManager()
{

}

Texture FontManager::CreateTexture(TextItemType itemType, const std::string & text, int fontSize, int xPos, int yPos, bool center)
{
    const Font & font = GetInstance().GetFont(itemType);
    return Texture(font.fontPath, text, fontSize, GuiHelper::GetInstance().GetRenderer(), xPos, yPos, center, font.r, font.g, font.b);
}

FontManager& FontManager::GetInstance()
{
    static FontManager _instance;
    return _instance;
}

const Font & FontManager::GetFont(TextItemType itemType)
{
    auto font = GetInstance().fonts.find(itemType);
    if(font != GetInstance().fonts.end())
        return font->second;
    else
    {
        std::cerr << "Could not find font for text ID #" << (int)itemType << " returning default text." << std::endl;
        return GetInstance().fonts[TextItemType::DEFAULT_TEXT];
    }
}

bool FindFontFilePath(Font & font)
{
    GuiHelper & guiHelper = GuiHelper::GetInstance();
    if(fs::exists(font.fontPath))
        return true;
    else if(fs::exists(guiHelper.GetThemeFolder()/"Font"/font.fontPath))
    {
        font.fontPath = guiHelper.GetThemeFolder()/"Font"/font.fontPath;
        return true;
    }
    else if(fs::exists(guiHelper.GetTextureFolder()/"Font"/font.fontPath))
    {
        font.fontPath = guiHelper.GetTextureFolder()/"Font"/font.fontPath;
        return true;
    }
#ifdef __arm__
    else if(fs::exists("/usr/share/fonts/ttf/"+font.fontPath))
    {
        font.fontPath = "/usr/share/fonts/ttf/"+font.fontPath;
        return true;
    }
#else
    else if(fs::exists("./root/usr/share/fonts/ttf/"+font.fontPath))
    {
        font.fontPath = "./root/usr/share/fonts/ttf/"+font.fontPath;
        return true;
    }
#endif
    return false;
}

bool FontManager::LoadFonts()
{
    std::string fontJSONPath = GuiHelper::GetInstance().GetThemeFolder()/"Boot/fonts.json";
    std::ifstream in(fontJSONPath);
    if(in.is_open() == false)
    {
        fontJSONPath = GuiHelper::GetInstance().GetTextureFolder()/"Boot/fonts.json";
        in.open(fontJSONPath);
        if(in.is_open() == false)
            return false;
    }

    json j;
    try
    {
        in >> j;
        in.close();
    }
    catch(...)
    {
        in.close();
        std::cerr << "Could not parse font json." << std::endl;
        return false;
    }

    std::map<std::string, Font> fontList = j.get<std::map<std::string, Font>>();

    for(auto & font : fontList)
    {
        if(FindFontFilePath(font.second) == false)
            continue;

        #define ASSIGN_FONT(TYPE) \
            if(font.first.compare(#TYPE) == 0) \
                fonts[TYPE] = font.second;

        ASSIGN_FONT(DEFAULT_TEXT)
        else ASSIGN_FONT(HEADER_TEXT)
        else ASSIGN_FONT(FOOTER_TEXT)
        else ASSIGN_FONT(BOOT_FOOTER_TEXT)
        else ASSIGN_FONT(GENERIC_MENU_ITEM_TEXT)
        else ASSIGN_FONT(GENERIC_MENU_ITEM_TEXT_ACTIVE)
        else ASSIGN_FONT(KEYBOARD_TEXT)
        else ASSIGN_FONT(KEYBOARD_LETTER)
        else ASSIGN_FONT(KEYBOARD_LETTER_ACTIVE)
        else ASSIGN_FONT(PUSH_BUTTON)
        else ASSIGN_FONT(RANGE_SCROLL_TEXT)
        else ASSIGN_FONT(MESSAGE_BOX_HEADER_TEXT)
        else ASSIGN_FONT(MESSAGE_BOX_TEXT)
        else ASSIGN_FONT(WARNING_HEADER_TEXT)
        else
            std::cerr << "No matching font variable for: " << font.first << std::endl;
    }

    return true;
}

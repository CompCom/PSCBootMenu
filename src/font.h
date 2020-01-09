/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef FONT_H_
#define FONT_H_

#include "framework/texture.h"
#include <map>

enum TextItemType { DEFAULT_TEXT, HEADER_TEXT, FOOTER_TEXT, BOOT_FOOTER_TEXT, GENERIC_MENU_ITEM_TEXT, GENERIC_MENU_ITEM_TEXT_ACTIVE, KEYBOARD_TEXT, KEYBOARD_LETTER, KEYBOARD_LETTER_ACTIVE, PUSH_BUTTON, RANGE_SCROLL_TEXT, MESSAGE_BOX_HEADER_TEXT, MESSAGE_BOX_TEXT, WARNING_HEADER_TEXT };

struct Font
{
    std::string fontPath;
    int r, g, b;
};

class FontManager
{
public:
    ~FontManager();
    static Texture CreateTexture(TextItemType itemType, const std::string & text, int fontSize, int xPos, int yPos, bool center);
    static FontManager& GetInstance();
    static const Font & GetFont(TextItemType itemType);
    bool LoadFonts();
private:
    FontManager();
    FontManager(const FontManager&) = delete;
    FontManager& operator =(const FontManager&) = delete;
    std::map<TextItemType, Font> fonts;
};

#endif // FONT_H_

/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "keyboardbutton.h"

KeyboardButton::KeyboardButton(const std::string & buttonText, int fontSize, int highlightFontSize, const Texture & highlightBGTexture, SDL_Renderer * renderer, int x, int y) : highlightBGTexture(highlightBGTexture)
{
    buttonTexture = FontManager::CreateTexture(KEYBOARD_LETTER, buttonText, fontSize, x, y, true);
    highlightTextTexture = FontManager::CreateTexture(KEYBOARD_LETTER_ACTIVE, buttonText, highlightFontSize, x, y, true);
    this->highlightBGTexture.SetPosition(x, y, true);
}

void KeyboardButton::draw(SDL_Renderer* renderer, bool active)
{
    if(active)
    {
        highlightBGTexture.Draw(renderer);
        highlightTextTexture.Draw(renderer);
    }
    else
        buttonTexture.Draw(renderer);
}
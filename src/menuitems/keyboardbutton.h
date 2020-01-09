/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef KEYBOARDBUTTON_H_
#define KEYBOARDBUTTON_H_

#include "pushbutton.h"

struct KeyboardButton : public PushButton
{
    KeyboardButton(const std::string & buttonText, int fontSize, int highlightFontSize, const Texture & highlightBGTexture, SDL_Renderer * renderer, int x, int y);
    void draw(SDL_Renderer* renderer, bool active) override;
    Texture highlightBGTexture, highlightTextTexture;
};
using KeyboardButtonPtr = std::shared_ptr<KeyboardButton>;

#endif
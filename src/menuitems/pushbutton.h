/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#include "menuitem.h"
#include <functional>

struct PushButton : MenuItem
{
    PushButton();
    PushButton(const std::string & buttonText, int fontSize, int x, int y, bool center = true);
    Texture buttonTexture;
    std::function<void()> onPress;
    void draw(SDL_Renderer* renderer, bool active);
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void setPosition(int x, int y) override;
};
using PushButtonPtr = std::shared_ptr<PushButton>;

#endif

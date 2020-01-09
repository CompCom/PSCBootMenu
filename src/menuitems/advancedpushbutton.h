/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef ADVANCEDPUSHBUTTON_H_
#define ADVANCEDPUSHBUTTON_H_

#include "menuitem.h"
#include <functional>
#include <map>

struct AdvancedPushButton : public MenuItem
{
    AdvancedPushButton(const std::string & buttonText, int fontSize, int x, int y, bool center = true);
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void setPosition(int x, int y) override;
    Texture buttonTexture;
    bool previousState = false;
    std::map<uint8_t, std::function<void()>> onPress;
    std::function<void()> onEnter;
};
using AdvancedPushButtonPtr = std::shared_ptr<AdvancedPushButton>;


#endif
/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef GENERICMENUENTRY_H_
#define GENERICMENUENTRY_H_

#include "menuitem.h"
#include <functional>
#include <map>

struct GenericMenuEntry : public MenuItem
{
    GenericMenuEntry();
    GenericMenuEntry(const Texture & background, const Texture & activeBackground, int yPos);
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void setPosition(int x, int y) override;
    void SetLeft(const std::string & text);
    void SetRight(const std::string & text);
    void SetRight(const Texture & normalTexture, const Texture & activeTexture);
    void SetYPos(int y);
    int yPos;
    Texture activeBackground, background;
    Texture left, leftActive, right, rightActive;
    bool previousState = false;
    std::map<uint8_t, std::function<void()>> onPress;
    std::function<void()> onEnter;
};
using GenericMenuEntryPtr = std::shared_ptr<GenericMenuEntry>;

#endif
/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "framework/texture.h"

class SDL_Context;
class SDL_Renderer;
class Manager;

struct MenuItem
{
    virtual void draw(SDL_Renderer* renderer, bool active) = 0;
    virtual int handleButtonPress(uint8_t gamebutton, uint8_t state) = 0;
    virtual void setPosition(int x, int y);
    virtual void Update();
};
using MenuItemPtr = std::shared_ptr<MenuItem>;

#endif

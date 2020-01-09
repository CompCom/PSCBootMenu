/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef RANGESCROLL_H_
#define RANGESCROLL_H_

#include "menuitem.h"

struct RangeScroll : public MenuItem
{
    RangeScroll(int & currentValue, int min, int max, SDL_Renderer * renderer);
    void createValueTexture();
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void setPosition(int x, int y) override;
    int min, max, x, y;
    int & value;
    SDL_Renderer * renderer;
    Texture valueTexture, upArrow, downArrow;
};
using RangeScrollPtr = std::shared_ptr<RangeScroll>;

#endif
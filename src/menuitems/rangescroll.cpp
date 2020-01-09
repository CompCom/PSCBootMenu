/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "rangescroll.h"

RangeScroll::RangeScroll(int & currentValue, int min, int max, SDL_Renderer * renderer) : min(min), max(max), value(currentValue), renderer(renderer)
{
    upArrow = FontManager::CreateTexture(RANGE_SCROLL_TEXT, u8"\u25B2", 50, 0, 0, false);
    downArrow = upArrow;
}

void RangeScroll::createValueTexture()
{
    valueTexture = FontManager::CreateTexture(RANGE_SCROLL_TEXT, std::to_string(value), 50, x, y, true);
}

void RangeScroll::draw(SDL_Renderer* renderer, bool active)
{
    upArrow.Draw(renderer);
    if(active)
        SDL_SetTextureColorMod(valueTexture.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(valueTexture.texture.get(), 255, 255, 255);
    valueTexture.Draw(renderer);
    downArrow.Draw(renderer, SDL_RendererFlip::SDL_FLIP_VERTICAL);
}

void RangeScroll::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
    createValueTexture();
    upArrow.rect.x = x - upArrow.rect.w/2;
    upArrow.rect.y = y - valueTexture.rect.h - 8;
    downArrow.rect.x = upArrow.rect.x;
    downArrow.rect.y = y + 6;
}

int RangeScroll::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        if(++value > max)
            value = min;
        createValueTexture();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        if(--value < min)
            value = max;
        createValueTexture();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return 1;
    }
    return 0;
}
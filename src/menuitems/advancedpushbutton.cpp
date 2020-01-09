/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "advancedpushbutton.h"
#include "font.h"

AdvancedPushButton::AdvancedPushButton(const std::string & buttonText, int fontSize, int x, int y, bool center)
{
    buttonTexture = FontManager::CreateTexture(TextItemType::PUSH_BUTTON, buttonText, fontSize, x, y, center);
}

void AdvancedPushButton::draw(SDL_Renderer* renderer, bool active)
{
    if(previousState != active)
    {
        if(active)
        {
            SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 0);
            if(onEnter)
                onEnter();
        }
        else
            SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 255);
    }
    buttonTexture.Draw(renderer);
    previousState = active;
}

int AdvancedPushButton::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return 1;
    default:
        if(state == 1 && onPress.count(gamebutton))
            onPress[gamebutton]();
        break;
    }
    return 0;
}

void AdvancedPushButton::setPosition(int x, int y)
{
    buttonTexture.rect.x = x;
    buttonTexture.rect.y = y;
}
/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "pushbutton.h"

PushButton::PushButton()
{}

PushButton::PushButton(const std::string & buttonText, int fontSize, int x, int y, bool center)
{
    buttonTexture = FontManager::CreateTexture(PUSH_BUTTON, buttonText, fontSize, x, y, center);
}

void PushButton::draw(SDL_Renderer* renderer, bool active)
{
    if(active)
        SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 255);
    buttonTexture.Draw(renderer);
}

int PushButton::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_A:
        if(state == 1)
            onPress();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return 1;
    }
    return 0;
}

void PushButton::setPosition(int x, int y)
{
    buttonTexture.rect.x = x;
    buttonTexture.rect.y = y;
}
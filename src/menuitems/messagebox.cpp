/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "messagebox.h"
#include <chrono>

MessageBox::MessageBox(const std::string & heading, const std::string & msg, SDL_Renderer * renderer) : renderer(renderer)
{
    message = FontManager::CreateTexture(TextItemType::MESSAGE_BOX_TEXT, msg, 24, 10, 0, false);
    title = FontManager::CreateTexture(TextItemType::MESSAGE_BOX_HEADER_TEXT, heading, 36, (message.rect.w+20)/2, 0, true);
    title.rect.y = 5;
    message.rect.y = 10 + title.rect.h + 10;
    bg = Texture(renderer, 0, 0, message.rect.w+20, title.rect.h+message.rect.h+30, 0xFF000000, false);
    overlay = Texture(renderer, 0, 0, 1280, 720, 0x80000000, false);
}

void MessageBox::draw(SDL_Renderer* renderer, bool active)
{
    overlay.Draw(renderer);
    bg.Draw(renderer);
    title.Draw(renderer);
    message.Draw(renderer);
}

int MessageBox::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    return 0;
}

void MessageBox::setPosition(int x, int y)
{
    x -= bg.rect.w / 2;
    y -= bg.rect.h / 2;
    bg.rect.x += x - posX;
    bg.rect.y += y - posY;
    title.rect.x += x - posX;
    title.rect.y += y - posY;
    message.rect.x += x - posX;
    message.rect.y += y - posY;
    posX = x;
    posY = y;
}

void MessageBox::Update()
{
    if(backgroundTask.wait_for(std::chrono::seconds(0)) == std::future_status::ready && endFunction)
        endFunction();
}
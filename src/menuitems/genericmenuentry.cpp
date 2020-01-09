/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "font.h"
#include "genericmenuentry.h"
#include "guihelper.h"

GenericMenuEntry::GenericMenuEntry() : GenericMenuEntry(GuiHelper::GetInstance().GetTexture(GuiElement::LIST_NORMAL),
                                                        GuiHelper::GetInstance().GetTexture(GuiElement::LIST_FOCUS),
                                                        0)
{}

GenericMenuEntry::GenericMenuEntry(const Texture & background, const Texture & activeBackground, int yPos) : yPos(yPos), activeBackground(activeBackground), background(background)
{
    this->background.SetPosition(640, yPos, true);
    this->activeBackground.SetPosition(640, yPos, true);
}

void GenericMenuEntry::draw(SDL_Renderer* renderer, bool active)
{
    if(active)
    {
        activeBackground.Draw(renderer);
        leftActive.Draw(renderer);
        rightActive.Draw(renderer);
    }
    else
    {
        background.Draw(renderer);
        left.Draw(renderer);
        right.Draw(renderer);
    }

    if(previousState != active && active)
    {
        if(onEnter)
            onEnter();
    }
    previousState = active;
}

int GenericMenuEntry::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return 1;
    default:
        if(state == 1 && onPress.count(gamebutton))
            onPress[gamebutton]();
        break;
    }
    return 0;
}

void GenericMenuEntry::setPosition(int x, int y)
{
    SetYPos(y);
}

void GenericMenuEntry::SetLeft(const std::string & text)
{
    left = FontManager::CreateTexture(GENERIC_MENU_ITEM_TEXT, text, 20, 0, yPos, true);
    leftActive = FontManager::CreateTexture(GENERIC_MENU_ITEM_TEXT_ACTIVE, text, 20, 0, yPos, true);
    left.rect.x = background.rect.x + 45;
    leftActive.rect.x = left.rect.x;
}

void GenericMenuEntry::SetRight(const std::string & text)
{
    std::string displayText = text;
    if(text.size() > 25)
        displayText = displayText.substr(0, 22) + "...";
    right = FontManager::CreateTexture(GENERIC_MENU_ITEM_TEXT, displayText, 20, 0, yPos, true);
    rightActive = FontManager::CreateTexture(GENERIC_MENU_ITEM_TEXT_ACTIVE, displayText, 20, 0, yPos, true);
    right.rect.x = background.rect.x + background.rect.w - 10 - right.rect.w;
    rightActive.rect.x = background.rect.x + background.rect.w - 10 - rightActive.rect.w;
}

void GenericMenuEntry::SetRight(const Texture & normalTexture, const Texture & activeTexture)
{
    right = normalTexture;
    rightActive = activeTexture;
    right.SetPosition(0, yPos, true);
    rightActive.SetPosition(0, yPos, true);
    right.rect.x = background.rect.x + background.rect.w - 10 - right.rect.w;
    rightActive.rect.x = background.rect.x + background.rect.w - 10 - rightActive.rect.w;
}

void GenericMenuEntry::SetYPos(int y)
{
    yPos = y;
    background.SetPosition(640, y, true);
    activeBackground.SetPosition(640, y, true);
    left.rect.y = yPos - left.rect.h/2;
    leftActive.rect.y = yPos - leftActive.rect.h/2;
    right.rect.y = yPos - right.rect.h/2;
    rightActive.rect.y = yPos - rightActive.rect.h/2;
}
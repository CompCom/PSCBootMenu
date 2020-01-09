/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "itemcollection.h"

void ItemCollection::addItem(MenuItemPtr item)
{
    if(moveItem)
        item->setPosition(x+xOffset,y+yOffset*items.size());
    items.push_back(item);
}

void ItemCollection::draw(SDL_Renderer* renderer, bool active)
{
    for(unsigned int i = 0, count = items.size(); i < count; ++i)
        items[i]->draw(renderer, active && i == currentItem);
}

int ItemCollection::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch (collectionType)
    {
    case NONE:
        return handleButtonPress_default(gamebutton, state);
    case HORIZONTAL:
        return handleButtonPress_horizontal(gamebutton, state);
    case VERTICAL:
        return handleButtonPress_vertical(gamebutton, state);
    }
    return 0;
}

int ItemCollection::handleButtonPress_default(uint8_t gamebutton, uint8_t state)
{
    int result = items[currentItem]->handleButtonPress(gamebutton, state);
    if(result == -1)
    {
        if(currentItem == 0)
            return -1;
        currentItem = currentItem-1;
    }
    else if(result == 1)
    {
        if(currentItem == items.size()-1)
            return 1;
        currentItem = currentItem+1;
    }
    return 0;
}

int ItemCollection::handleButtonPress_horizontal(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            currentItem = 0;
            return -1;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            currentItem = 0;
            return 1;
        default:
            if(items.size())
                currentItem = (currentItem + items[currentItem]->handleButtonPress(gamebutton, state) + items.size()) % items.size();
            break;
    }
    return 0;
}

int ItemCollection::handleButtonPress_vertical(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            return -1;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            return 1;
        default:
            if(items.size())
                currentItem = (currentItem + items[currentItem]->handleButtonPress(gamebutton, state) + items.size()) % items.size();
            break;
    }
    return 0;
}
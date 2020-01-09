/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "imagepushbutton.h"
#include "scrollingcontainer.h"

void ScrollingContainer::AddItem(const std::shared_ptr<ImagePushButton> & item)
{
    item->setPosition(x + (items.size()-currentItem)*offset, y);
    items.push_back(item);
}

void ScrollingContainer::draw(SDL_Renderer* renderer, bool active)
{
    for(unsigned int i = 0, count = items.size(); i < count; ++i)
        items[i]->draw(renderer, (scrolling) ? false : active && i == currentItem);
}

int ScrollingContainer::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            return -1;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            return 1;
        default:
            int result = items[currentItem]->handleButtonPress(gamebutton, state);
            if(state > 0)
            {
                if(result == -1)
                    requestedScroll |= LEFT;
                else if(result == 1)
                    requestedScroll |= RIGHT;
            }
            else
            {
                if(result == -1)
                    requestedScroll &= ~LEFT;
                else if(result == 1)
                    requestedScroll &= ~RIGHT;
            }
            break;
    }
    return 0;
}

void ScrollingContainer::SetCurrentItem(int id)
{
    if(id >= 0 && id < items.size())
    {
        currentItem = id;
        setPositions();
    }
}

void ScrollingContainer::setPositions()
{
    int moveOffset = (scrolling == 0) ? 0 : frameCount/(float)maxFrameCount * offset * scrolling;
    for(int i = 0, count = items.size(); i < count; ++i)
        items[i]->setPosition(x + (i-currentItem)*offset - moveOffset, y);
}

void ScrollingContainer::Update()
{
    if(scrolling == 0 && requestedScroll != 0)
    {
        if(requestedScroll & LEFT)
            scrolling = (currentItem > 0) ? -1 : 0;
        else if(requestedScroll & RIGHT)
            scrolling = (currentItem+1 < items.size()) ? 1 : 0;
    }
    if(scrolling != 0)
    {
        if(frameCount < maxFrameCount)
            ++frameCount;
        else
        {
            currentItem += scrolling;
            scrolling = 0;
            frameCount = 0;
        }
        setPositions();
    }
}
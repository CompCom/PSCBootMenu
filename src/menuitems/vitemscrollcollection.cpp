/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "guihelper.h"
#include "vitemscrollcollection.h"

VItemScrollCollection::VItemScrollCollection()
{
    arrowUp = GuiHelper::GetInstance().GetTexture(ARROW_UP);
    arrowDown = GuiHelper::GetInstance().GetTexture(ARROW_DOWN);
}

void VItemScrollCollection::clearItems()
{
    items.clear();
    topItemId = 0;
    currentItem = 0;
}

void VItemScrollCollection::draw(SDL_Renderer* renderer, bool active)
{
    if(items.size() - topItemId > displayItemCount)
        arrowDown.Draw(renderer);
    if(topItemId != 0)
        arrowUp.Draw(renderer);
    for(int i = topItemId, count = std::min(topItemId+displayItemCount, (int)items.size()); i < count; ++i)
        items[i]->draw(renderer, active && i == currentItem);
}

int VItemScrollCollection::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    if(items.size() == 0)
        return 0;

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

    if(currentItem < topItemId)
    {
        topItemId = currentItem;
        moveItems();
    }
    else if(currentItem >= topItemId+displayItemCount)
    {
        topItemId = currentItem - displayItemCount + 1;
        moveItems();
    }

    return 0;
}

void VItemScrollCollection::moveItems()
{
    for(int i = topItemId, count = 0, max = std::min(topItemId+displayItemCount, (int)items.size()); i < max; ++i, ++count)
        items[i]->setPosition(x+xOffset, y+yOffset*count);
}

void VItemScrollCollection::setArrowPosition(int xPos)
{
    arrowUp.SetPosition(xPos-arrowUp.rect.w/2, y-yOffset/2, false);
    arrowDown.SetPosition(xPos-arrowDown.rect.w/2, y+yOffset*((float)displayItemCount-.75f)-arrowDown.rect.h/2, false);
}
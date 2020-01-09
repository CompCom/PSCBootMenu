/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef VITEMSCROLLCOLLECTION_H_
#define VITEMSCROLLCOLLECTION_H_

#include "itemcollection.h"

struct VItemScrollCollection : public ItemCollection
{
    VItemScrollCollection();
    Texture arrowUp, arrowDown;
    int displayItemCount = 10, topItemId = 0;
    void clearItems();
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void moveItems();
    void setArrowPosition(int xPos);
};
using VItemScrollCollectionPtr = std::shared_ptr<VItemScrollCollection>;

#endif
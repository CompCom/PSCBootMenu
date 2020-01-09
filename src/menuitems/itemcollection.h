/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef ITEMCOLLECTION_H_
#define ITEMCOLLECTION_H_

#include "menuitem.h"

#include <vector>

struct ItemCollection : public MenuItem
{
    enum CollectionType { NONE = 0, VERTICAL, HORIZONTAL } collectionType = NONE;
    bool moveItem = true;
    int x = 50, y = 120, xOffset = 10, yOffset = 20;
    std::vector<MenuItemPtr> items;
    int currentItem = 0;
    virtual void addItem(MenuItemPtr item);
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
private:
    int handleButtonPress_default(uint8_t gamebutton, uint8_t state);
    int handleButtonPress_horizontal(uint8_t gamebutton, uint8_t state);
    int handleButtonPress_vertical(uint8_t gamebutton, uint8_t state);
};
using ItemCollectionPtr = std::shared_ptr<ItemCollection>;

#endif
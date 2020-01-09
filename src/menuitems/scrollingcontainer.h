/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef SCROLLINGCONTAINER_H_
#define SCROLLINGCONTAINER_H_

#include "menuitem.h"
#include <vector>

struct ImagePushButton;

struct ScrollingContainer : public MenuItem
{
    enum ScrollDirection { LEFT = 1, RIGHT = 1 << 1};
    void AddItem(const std::shared_ptr<ImagePushButton> & item);
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void SetCurrentItem(int id);
    void setPositions();
    void Update() override;
    int x, y, offset;
    int currentItem = 0;
    int scrolling = 0;
    int requestedScroll = 0;
    int frameCount = 0, maxFrameCount = 20;
    std::vector<std::shared_ptr<ImagePushButton>> items;
};
using ScrollingContainerPtr = std::shared_ptr<ScrollingContainer>;

#endif
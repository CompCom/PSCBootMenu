/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef MESSAGEBOX_H_
#define MESSAGEBOX_H_

#include "menuitem.h"
#include <functional>
#include <future>

struct MessageBox : public MenuItem
{
    MessageBox(const std::string & heading, const std::string & msg, SDL_Renderer * renderer);
    void draw(SDL_Renderer* renderer, bool active) override;
    int handleButtonPress(uint8_t gamebutton, uint8_t state) override;
    void setPosition(int x, int y) override;
    void Update() override;
    SDL_Renderer * renderer;
    int posX = 0, posY = 0;
    Texture bg, title, message, overlay;
    std::future<void> backgroundTask;
    std::function<void()> endFunction;
};

#endif
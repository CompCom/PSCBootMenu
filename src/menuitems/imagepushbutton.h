/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef IMAGEPUSHBUTTON_H_
#define IMAGEPUSHBUTTON_H_

#include "pushbutton.h"

struct ImagePushButton : public PushButton
{
    ImagePushButton(const Texture & image, const Texture & activeImage, SDL_Renderer * renderer);
    void draw(SDL_Renderer* renderer, bool active) override;
    void setPosition(int x, int y) override;
    Texture activeTexture;
    SDL_Renderer * renderer;
};
using ImagePushButtonPtr = std::shared_ptr<ImagePushButton>;

#endif
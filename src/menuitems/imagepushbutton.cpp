/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "imagepushbutton.h"

ImagePushButton::ImagePushButton(const Texture & image, const Texture & activeImage, SDL_Renderer * renderer) : renderer(renderer)
{
    buttonTexture = image;
    activeTexture = activeImage;
}

void ImagePushButton::setPosition(int x, int y)
{
    buttonTexture.rect.x = x - buttonTexture.rect.w/2;
    buttonTexture.rect.y = y - buttonTexture.rect.h/2;
    activeTexture.rect = buttonTexture.rect;
}

void ImagePushButton::draw(SDL_Renderer* renderer, bool active)
{
    (active) ? activeTexture.Draw(renderer) : buttonTexture.Draw(renderer);
}
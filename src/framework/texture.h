/**
  * Copyright (C) 2017-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <memory>
#include <SDL.h>

struct Texture
{
    std::shared_ptr<SDL_Texture> texture;
    SDL_Rect rect;
    Texture();
    Texture(const std::string & text, int fontSize, SDL_Renderer* renderer, int x = 0, int y = 0, bool centerText = false, const int color = 0xFFFFFFFF);
    Texture(const std::string & font, const std::string & text, int fontSize, SDL_Renderer* renderer, int x = 0, int y = 0, bool centerText = false, const uint8_t r = 0xFF, const uint8_t g = 0xFF, const uint8_t b = 0xFF);
    Texture(const std::string & imageFilePath, SDL_Renderer* renderer, int x = 0, int y = 0, bool centerImg = false);
    Texture(SDL_Renderer* renderer, int rectX, int rectY, int rectW, int rectH, int color, bool centerRect = true);
    void Draw(SDL_Renderer* renderer);
    void Draw(SDL_Renderer* renderer, SDL_RendererFlip flip_enum);
};

struct Sprite
{
    std::shared_ptr<SDL_Texture> texture;
    SDL_Rect sRect, dRect;
    void Draw(SDL_Renderer * renderer);
};

SDL_Texture * LoadTextureImage(SDL_Renderer *renderer, const std::string & file, SDL_Rect & rect);
SDL_Texture * LoadTexturePNG(SDL_Renderer *renderer, const std::string & file, SDL_Rect * rect = nullptr);
SDL_Texture * WriteText(const std::string & text, int fontSize, SDL_Renderer* renderer, int & textureWidth, int & textureHeight, const int color = 0xFFFFFF);
SDL_Texture * CreateRect(SDL_Renderer* renderer, int width, int height, const int color);

#endif

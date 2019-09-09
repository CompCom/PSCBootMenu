/**
  * Copyright (C) 2017-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "texture.h"
#include "font8x8_basic.h"

#include <iostream>
#include <cstring>
#include <png.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define rmask 0xff000000
    #define gmask 0x00ff0000
    #define bmask 0x0000ff00
    #define amask 0x000000ff
#else
    #define rmask 0x000000ff
    #define gmask 0x0000ff00
    #define bmask 0x00ff0000
    #define amask 0xff000000
#endif

Texture::Texture() {}

Texture::Texture(const std::string & text, int fontSize, SDL_Renderer* renderer, int x, int y, bool centerText, const int color)
{
    texture = std::shared_ptr<SDL_Texture>(WriteText(text, fontSize, renderer, rect.w, rect.h, color), SDL_DestroyTexture);
    rect.x = (centerText) ? x-rect.w/2 : x;
    rect.y = (centerText) ? y-rect.h/2 : y;
}

Texture::Texture(const std::string & font, const std::string & text, int fontSize, SDL_Renderer* renderer, int x, int y, bool centerText, const uint8_t r, const uint8_t g, const uint8_t b)
{
    rect.x = x;
    rect.y = y;

    if(text.size() == 0)
        return;

    auto ttf_font = TTF_OpenFont(font.c_str(), fontSize);
    if(ttf_font)
    {
        SDL_Color color = { r, g, b, 0xFF };
        auto surface = TTF_RenderUTF8_Blended_Wrapped(ttf_font, text.c_str(), color, 1152);
        if(surface)
        {
            texture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(renderer, surface), SDL_DestroyTexture);
            SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
            SDL_FreeSurface(surface);

            SDL_QueryTexture(texture.get(), NULL, NULL, &rect.w, &rect.h);
            if(centerText)
            {
                rect.x -= rect.w/2;
                rect.y -= rect.h/2;
            }
        }
        else
            std::cerr << TTF_GetError() << std::endl;
        TTF_CloseFont(ttf_font);
    }
    else
        std::cerr << TTF_GetError() << std::endl;
}

Texture::Texture(const std::string & imageFilePath, SDL_Renderer* renderer, int x, int y, bool centerImg)
{
    texture = std::shared_ptr<SDL_Texture>(LoadTextureImage(renderer, imageFilePath, rect), SDL_DestroyTexture);
    rect.x = (centerImg) ? x-rect.w/2 : x;
    rect.y = (centerImg) ? y-rect.h/2 : y;
}

Texture::Texture(SDL_Renderer* renderer, int rectX, int rectY, int rectW, int rectH, int color, bool centerRect)
{
    texture = std::shared_ptr<SDL_Texture>(CreateRect(renderer, 1, 1, color), SDL_DestroyTexture);
    if(centerRect)
        rect = { rectX-rectW/2, rectY-rectH/2, rectW, rectH };
    else
        rect = { rectX, rectY, rectW, rectH };
}

void Texture::Draw(SDL_Renderer* renderer)
{
    if(texture.get())
      SDL_RenderCopyEx(renderer, texture.get(), NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

void Texture::Draw(SDL_Renderer* renderer, SDL_RendererFlip flip_enum)
{
    if(texture.get())
      SDL_RenderCopyEx(renderer, texture.get(), NULL, &rect, 0, NULL, flip_enum);
}

void Sprite::Draw(SDL_Renderer * renderer)
{
    SDL_RenderCopyEx(renderer, texture.get(), &sRect, &dRect, 0, NULL, SDL_FLIP_NONE);
}

SDL_Texture * LoadTextureImage(SDL_Renderer *renderer, const std::string & file, SDL_Rect & rect)
{
    if(auto texture = IMG_LoadTexture(renderer, file.c_str()))
    {
        SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
        return texture;
    }
    std::cerr << "Cannot open image file: " << file << "\n";
    return nullptr;
}

SDL_Texture * LoadTexturePNG(SDL_Renderer *renderer, const std::string & file, SDL_Rect * rect)
{
    SDL_Texture* texture = nullptr;

    png_image png;
    memset(&png, 0, sizeof(png));
    png.version = PNG_IMAGE_VERSION;
    png.opaque = NULL;

    if(png_image_begin_read_from_file(&png, file.c_str()) == 0)
    {
        std::cerr << "Cannot open png file: " << file << "\n";
        return nullptr;
    }
    //Set PNG Format
    png.format = PNG_FORMAT_RGBA;
    if(png.width == 0 || png.height == 0)
    {
        std::cerr << "Invalid png: " << file << "\n";
        png_image_free(&png);
        return nullptr;
    }
    if(rect)
    {
        rect->h = png.height;
        rect->w = png.width;
    }
    //Create surface
    auto surface = SDL_CreateRGBSurface(0, png.width, png.height, 32, rmask, gmask, bmask, amask);

    if(surface)
    {
        //Read png data to surface
        png_image_finish_read(&png, NULL, surface->pixels, 0, NULL);
        png_image_free(&png);
        //Convert surface to texture
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    else
    {
        std::cerr << "Error creating surface for: " << file << "\n";
        exit(1);
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

SDL_Texture * WriteText(const std::string & text, int fontSize, SDL_Renderer* renderer, int & textureWidth, int & textureHeight, const int color)
{
    if(text.size()==0)
        return nullptr;
    textureWidth = fontSize*text.size();
    textureHeight = fontSize;
    auto surface = SDL_CreateRGBSurface(0, 8*text.size(), 8, 32, rmask, gmask, bmask, amask);
    uint* pixels = reinterpret_cast<uint*>(surface->pixels);
    int pitch = 8*text.size();
    for(unsigned int i = 0; i < text.size(); ++i)
    {
        auto bitmap = font8x8_basic[text[i]];
        int offset = i*8;
        for(int y = 0; y < 8; ++y)
        {
            for(int x = 0; x < 8; ++x)
            {
                pixels[x+offset+(y*pitch)] = (bitmap[y] & 1 << x) ? color : 0;
            }
        }
    }
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture * CreateRect(SDL_Renderer* renderer, int width, int height, const int color)
{
    SDL_Texture * texture = nullptr;
    SDL_Rect rect { 0, 0, width, height};

    auto surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    SDL_FillRect(surface, &rect, color);

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surface);
    return texture;
}

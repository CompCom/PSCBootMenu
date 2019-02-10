/**
  * Copyright (C) 2017-2018 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "sdl_context.h"

#include <iostream>
#include <thread>
#include <SDL.h>

SDL_Context::SDL_Context(std::chrono::milliseconds fpsTime) : fpsTime(fpsTime)
{
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL init failed with error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL) {
        std::cerr << "Cannot create renderer.\n";
        SDL_DestroyWindow(window);
        exit(1);
    }
    nextFrameTime = std::chrono::system_clock::now()+fpsTime;
}

SDL_Context::~SDL_Context()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDL_Context::StartFrame()
{
    SDL_RenderClear(renderer);
}

void SDL_Context::EndFrame()
{
    SDL_RenderPresent(renderer);
    std::this_thread::sleep_until(nextFrameTime);
    nextFrameTime += fpsTime;
}

void SDL_Context::ResetTimer()
{
    nextFrameTime = std::chrono::system_clock::now()+fpsTime;
}

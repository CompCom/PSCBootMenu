/**
  * Copyright (C) 2017-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef SDL_CONTEXT_H_
#define SDL_CONTEXT_H_

#include <string>
#include <memory>
#include <chrono>
#include <vector>

class SDL_Window;
class SDL_Renderer;

struct SDL_Context
{
public:
    SDL_Context(std::chrono::milliseconds fpsTime = std::chrono::milliseconds(33));
    ~SDL_Context();
    void StartFrame();
    void EndFrame();
    void ResetTimer();

    SDL_Window* window;
    SDL_Renderer* renderer;
    std::chrono::milliseconds fpsTime;

private:
    std::chrono::time_point<std::chrono::system_clock> nextFrameTime;
};

#endif

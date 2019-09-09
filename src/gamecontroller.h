/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include <memory>
#include <chrono>
#include <vector>

#include <SDL.h>

struct GameControllerEvent
{
    int button;
    uint8_t state;
};

struct GameButtonState
{
    bool state;
    std::chrono::time_point<std::chrono::system_clock> updateTime;
};

class GameController
{
private:
    std::shared_ptr<SDL_GameController> controller;
    std::vector<GameControllerEvent> & controllerEvents;
    GameButtonState buttonState[15];
    bool mapJoystickToDpad;
    int id;
    void processButtonState(bool newState, int buttonId);
public:
    GameController(int id, std::vector<GameControllerEvent> & controllerEvents);
    ~GameController();
    int GetId() const;
    SDL_GameController* GetController() const;
    void Update();
};

using GameControllerPtr = std::unique_ptr<GameController>;

#endif

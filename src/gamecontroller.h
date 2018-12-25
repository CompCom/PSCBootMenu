/**
  * Copyright (C) 2018 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include <memory>

#include <SDL.h>

struct GameControllerEvent
{
    int button;
    bool state;
};

class GameController
{
private:
    std::shared_ptr<SDL_GameController> controller;
    bool buttonState[15];
    bool mapJoystickToDpad;
    int id;
    void processButtonState(bool newState, int buttonId);
public:
    GameController();
    GameController(int id);
    ~GameController();
    int GetId() const;
    SDL_GameController* GetController() const;
    void Update();
};

#endif

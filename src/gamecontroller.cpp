/**
  * Copyright (C) 2018 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "gamecontroller.h"
#include <string>
#include <list>
#include <iostream>

extern std::list<GameControllerEvent> ControllerEvents;

GameController::GameController() : id(-1)
{}

GameController::GameController(int id)
{
    this->id = id;
    controller = std::shared_ptr<SDL_GameController>(SDL_GameControllerOpen(id), SDL_GameControllerClose);
    if(std::string(SDL_JoystickNameForIndex(id)).find("Sony Interactive Entertainment Controller") != std::string::npos)
        mapJoystickToDpad = true;
    else
        mapJoystickToDpad = false;
    memset(buttonState, 0, sizeof(buttonState));
}

GameController::~GameController()
{
    std::cout << "Removed Game Controller ID: " << id << std::endl;
}

int GameController::GetId() const
{
    return id;
}

SDL_GameController* GameController::GetController() const
{
    return controller.get();
}

void GameController::processButtonState(bool newState, int buttonId)
{
    if(newState != buttonState[buttonId])
    {
        ControllerEvents.push_back({buttonId, newState});
        buttonState[buttonId] = newState;
    }
}

void GameController::Update()
{
    for(int i = 0; i < 15; ++i)
    {
        if(mapJoystickToDpad && i >= SDL_CONTROLLER_BUTTON_DPAD_UP && i <= SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
            continue;

        processButtonState(SDL_GameControllerGetButton(controller.get(), (SDL_GameControllerButton)i), i);
    }

    if(mapJoystickToDpad)
    {
        int x = SDL_GameControllerGetAxis(controller.get(), SDL_CONTROLLER_AXIS_LEFTX);
        processButtonState(x < 0, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        processButtonState(x > 0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        int y = SDL_GameControllerGetAxis(controller.get(), SDL_CONTROLLER_AXIS_LEFTY);
        processButtonState(y < 0, SDL_CONTROLLER_BUTTON_DPAD_UP);
        processButtonState(y > 0, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    }
}

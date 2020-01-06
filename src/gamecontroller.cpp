/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "gamecontroller.h"
#include <iostream>
#include <string>
#include <vector>

GameController::GameController(int id, std::vector<GameControllerEvent> & controllerEvents) : controllerEvents(controllerEvents)
{
    this->id = id;
    controller = std::shared_ptr<SDL_GameController>(SDL_GameControllerOpen(id), SDL_GameControllerClose);
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

uint8_t GameController::GetButtonState(int button) const
{
    if(button >= 0 && button < 15)
        return buttonState[button].state;
    return 0;
}

SDL_GameController* GameController::GetController() const
{
    return controller.get();
}

void GameController::processButtonState(bool newState, int buttonId)
{
    using namespace std::chrono_literals;
    if(newState != buttonState[buttonId].state)
    {
        controllerEvents.push_back({buttonId, newState});
        buttonState[buttonId] = {newState, std::chrono::system_clock::now()};
    }
    else if(buttonState[buttonId].state && (std::chrono::system_clock::now()-buttonState[buttonId].updateTime) > 300ms)
    {
        controllerEvents.push_back({buttonId, 2});
        buttonState[buttonId].updateTime += 50ms;
    }
}

void GameController::Update()
{
    for(int i = 0; i < SDL_CONTROLLER_BUTTON_DPAD_UP; ++i)
    {
        processButtonState(SDL_GameControllerGetButton(controller.get(), (SDL_GameControllerButton)i), i);
    }

    // Handle both analog and dpad input
    constexpr int deadZone = 32767/2;
    const int x = SDL_GameControllerGetAxis(controller.get(), SDL_CONTROLLER_AXIS_LEFTX);
    processButtonState(SDL_GameControllerGetButton(controller.get(), SDL_CONTROLLER_BUTTON_DPAD_LEFT) || x < -deadZone, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    processButtonState(SDL_GameControllerGetButton(controller.get(), SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || x > deadZone, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    const int y = SDL_GameControllerGetAxis(controller.get(), SDL_CONTROLLER_AXIS_LEFTY);
    processButtonState(SDL_GameControllerGetButton(controller.get(), SDL_CONTROLLER_BUTTON_DPAD_UP) || y < -deadZone, SDL_CONTROLLER_BUTTON_DPAD_UP);
    processButtonState(SDL_GameControllerGetButton(controller.get(), SDL_CONTROLLER_BUTTON_DPAD_DOWN) || y > deadZone, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
}

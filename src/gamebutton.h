/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef GAMEBUTTON_H_
#define GAMEBUTTON_H_

#include "guihelper.h"
#include <SDL_gamecontroller.h>

enum GameButtonValue
{
    GAME_BUTTON_X = 0, GAME_BUTTON_O, GAME_BUTTON_TRIANGLE, GAME_BUTTON_SQUARE, GAME_BUTTON_L1, GAME_BUTTON_R1, GAME_BUTTON_START, GAME_BUTTON_END,
#ifdef SEGA
    GAME_BUTTON_SEGA_A = GAME_BUTTON_X, GAME_BUTTON_SEGA_B, GAME_BUTTON_SEGA_C, GAME_BUTTON_SEGA_Y, GAME_BUTTON_SEGA_X, GAME_BUTTON_SEGA_Z, GAME_BUTTON_SEGA_START
#endif
};

struct GameButton
{
    const int SDL_Value;
    const GuiElement GUI_Value;
};

#ifdef SEGA
constexpr GameButton GameButtons[GAME_BUTTON_END] =
{
    {SDL_CONTROLLER_BUTTON_A, GuiElement::A_BUTTON},
    {SDL_CONTROLLER_BUTTON_B, GuiElement::B_BUTTON},
    {SDL_CONTROLLER_BUTTON_Y, GuiElement::C_BUTTON},
    {SDL_CONTROLLER_BUTTON_X, GuiElement::Y_BUTTON},
    {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, GuiElement::X_BUTTON},
    {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, GuiElement::Z_BUTTON},
    {SDL_CONTROLLER_BUTTON_START, GuiElement::START_BUTTON},
};
#else
constexpr GameButton GameButtons[GAME_BUTTON_END] =
{
    {SDL_CONTROLLER_BUTTON_A, GuiElement::X_BUTTON},
    {SDL_CONTROLLER_BUTTON_B, GuiElement::O_BUTTON},
    {SDL_CONTROLLER_BUTTON_Y, GuiElement::TRIANGLE_BUTTON},
    {SDL_CONTROLLER_BUTTON_X, GuiElement::SQUARE_BUTTON},
    {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, GuiElement::L1_BUTTON},
    {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, GuiElement::R1_BUTTON},
    {SDL_CONTROLLER_BUTTON_START, GuiElement::START_BUTTON},
};
#endif

#endif // GAMEBUTTON_H_

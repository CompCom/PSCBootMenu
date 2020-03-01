/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "booltoggle.h"
#include "guihelper.h"

BoolToggle::BoolToggle(const std::string & variable_name, const std::string & value) : GenericMenuEntry()
{
    SetValueFromConfig(value);
    SetLeft(variable_name);
    UpdateSlider(this->value);
    onPress[SDL_CONTROLLER_BUTTON_A] = [this](){ UpdateSlider(!this->value); };
    onPress[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = std::bind(&BoolToggle::UpdateSlider, this, false);
    onPress[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = std::bind(&BoolToggle::UpdateSlider, this, true);
}

std::string BoolToggle::GetConfigValue() const
{
    return (value) ? "\"1\"" : "\"0\"";
}

void BoolToggle::SetValueFromConfig(const std::string & value)
{
    this->value = (value.compare("\"1\"") == 0);
}

void BoolToggle::UpdateSlider(bool newValue)
{
    value = newValue;
    const Texture & sliderTexture = (value) ? GuiHelper::GetInstance().GetTexture(SLIDER_ON) : GuiHelper::GetInstance().GetTexture(SLIDER_OFF);
    SetRight(sliderTexture, sliderTexture);
}

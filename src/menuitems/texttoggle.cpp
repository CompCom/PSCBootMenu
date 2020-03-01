/**
  * Copyright (C) 2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "guihelper.h"
#include "texttoggle.h"
#include <algorithm>

TextToggle::TextToggle() : GenericMenuEntry(), selectedValue(0)
{
    onPress[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = [this](){setSelectedValue((this->selectedValue - 1 + this->values.size()) % this->values.size());};
    onPress[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = [this](){setSelectedValue((this->selectedValue + 1) % this->values.size());};
}

void TextToggle::setSelectedValue(std::string currentValue)
{
    currentValue.erase(std::remove_if(currentValue.begin(), currentValue.end(), [](const char & c){ return c == '"';}), currentValue.end());
    auto iter = std::find(values.begin(), values.end(), currentValue);
    if(iter != values.end())
        selectedValue = std::distance(values.begin(), iter);
    else
        selectedValue = 0;
    setSelectedValue(selectedValue);
}

void TextToggle::setSelectedValue(int newValue)
{
    // Set New Value and Generate Textures for value
    selectedValue = newValue;
    SetRight(values[selectedValue]);

    // Add arrows to texture
    GuiHelper & guiHelper = GuiHelper::GetInstance();
    SetRight(guiHelper.CreateArrowedText(right), guiHelper.CreateArrowedText(rightActive));
}
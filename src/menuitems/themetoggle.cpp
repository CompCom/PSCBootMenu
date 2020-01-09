/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "guihelper.h"
#include "themetoggle.h"
#include <algorithm>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

extern std::string uiThemesFolder;

ThemeToggle::ThemeToggle(const std::string & variable_name, const std::string & currentValue) : GenericMenuEntry()
{
    SetLeft(variable_name);
    for(auto & item : fs::directory_iterator(uiThemesFolder))
    {
        if(fs::is_directory(item.path()))
        {
            values.push_back(item.path().filename().string());
        }
    }
    setSelectedValue(currentValue);
    onPress[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = [this](){setSelectedValue((this->selectedValue - 1 + this->values.size()) % this->values.size());};
    onPress[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = [this](){setSelectedValue((this->selectedValue + 1) % this->values.size());};
}


void ThemeToggle::draw(SDL_Renderer* renderer, bool active)
{
    GenericMenuEntry::draw(renderer, active);
}

void ThemeToggle::setSelectedValue(std::string currentValue)
{
    currentValue.erase(std::remove_if(currentValue.begin(), currentValue.end(), [](const char & c){ return c == '"';}), currentValue.end());
    auto iter = std::find(values.begin(), values.end(), currentValue);
    if(iter != values.end())
        selectedValue = std::distance(values.begin(), iter);
    else
        selectedValue = 0;
    setSelectedValue(selectedValue);
}

void ThemeToggle::setSelectedValue(int newValue)
{
    selectedValue = newValue;
    SetRight(values[selectedValue]);

    Texture arrow = GuiHelper::GetInstance().GetTexture(GuiElement::ARROW_RIGHT);
    const int arrowW = arrow.rect.w;
    const int spacing = 5;
    right.SetPosition(arrowW+spacing, 0, false);
    rightActive.SetPosition(arrowW+spacing, 0, false);

    SDL_Renderer* renderer = GuiHelper::GetInstance().GetRenderer();

    Texture normal;
    normal.CreateRendableTexture(renderer, right.rect.w+(arrowW+spacing)*2, right.rect.h, 0, 0, false);
    normal.SetAsRenderTarget(renderer);
    arrow.SetPosition(arrowW/2, normal.rect.h/2, true);
    arrow.Draw(renderer, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
    right.Draw(renderer);
    arrow.rect.x = right.rect.x+right.rect.w+spacing;
    arrow.Draw(renderer);
    SDL_SetRenderTarget(renderer, NULL);

    Texture active;
    active.CreateRendableTexture(renderer, rightActive.rect.w+(arrowW+spacing)*2, rightActive.rect.h, 0, 0, false);
    active.SetAsRenderTarget(renderer);
    arrow.SetPosition(arrowW/2, active.rect.h/2, true);
    arrow.Draw(renderer, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
    rightActive.Draw(renderer);
    arrow.rect.x = rightActive.rect.x+rightActive.rect.w+spacing;
    arrow.Draw(renderer);
    SDL_SetRenderTarget(renderer, NULL);

    SetRight(normal, active);
}
/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef THEMETOGGLE_H_
#define THEMETOGGLE_H_

#include "genericmenuentry.h"
#include <vector>

struct ThemeToggle : public GenericMenuEntry
{
    ThemeToggle(const std::string & variable_name, const std::string & currentValue);
    void draw(SDL_Renderer* renderer, bool active) override;
    void setSelectedValue(std::string currentValue);
    void setSelectedValue(int newValue);
    unsigned int selectedValue;
    std::vector<std::string> values;
};
using ThemeTogglePtr = std::shared_ptr<ThemeToggle>;

#endif
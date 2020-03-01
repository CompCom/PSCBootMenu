/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "themetoggle.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

extern std::string uiThemesFolder;

ThemeToggle::ThemeToggle(const std::string & variable_name, const std::string & currentValue) : TextToggle()
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
}

std::string ThemeToggle::GetConfigValue() const
{
    return "\"" + values[selectedValue] + "\"";
}

void ThemeToggle::SetValueFromConfig(const std::string & value)
{
    setSelectedValue(value);
}
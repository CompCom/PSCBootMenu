/**
  * Copyright (C) 2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "languagemanager.h"
#include "languagetoggle.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

LanguageToggle::LanguageToggle(const std::string & variable_name, const std::string & currentValue)
{
    SetLeft(variable_name);
    for(auto & item : fs::directory_iterator(LanguageManager::GetLanguageFolder()))
    {
        const auto & path = item.path();
        if(fs::is_regular_file(path) && path.extension().compare(".json") == 0)
        {
            auto filename = path.filename().string();
            values.push_back(filename.substr(0, filename.find_last_of('.')));
        }
    }
    SetValueFromConfig(currentValue);
}

std::string LanguageToggle::GetConfigValue() const
{
    return "\"" + values[selectedValue] + ".json\"";
}

void LanguageToggle::SetValueFromConfig(const std::string & value)
{
    setSelectedValue(value.substr(0, value.find_last_of('.')));
}
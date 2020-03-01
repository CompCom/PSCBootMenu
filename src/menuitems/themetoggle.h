/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef THEMETOGGLE_H_
#define THEMETOGGLE_H_

#include "settingstoggle.h"
#include "texttoggle.h"

struct ThemeToggle : public TextToggle, public SettingsToggle
{
    ThemeToggle(const std::string & variable_name, const std::string & currentValue);
    std::string GetConfigValue() const override;
    void SetValueFromConfig(const std::string & value) override;
};
using ThemeTogglePtr = std::shared_ptr<ThemeToggle>;

#endif
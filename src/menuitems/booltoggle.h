/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef BOOLTOGGLE_H_
#define BOOLTOGGLE_H_

#include "genericmenuentry.h"
#include "settingstoggle.h"

struct BoolToggle : public GenericMenuEntry, public SettingsToggle
{
    BoolToggle(const std::string & variable_name, const std::string & value);
    std::string GetConfigValue() const override;
    void SetValueFromConfig(const std::string & value) override;
    void UpdateSlider(bool newValue);
    bool value;
};
using BoolTogglePtr = std::shared_ptr<BoolToggle>;

#endif
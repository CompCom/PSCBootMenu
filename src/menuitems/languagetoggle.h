/**
  * Copyright (C) 2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef LANGUAGETOGGLE_H_
#define LANGUAGETOGGLE_H_

#include "settingstoggle.h"
#include "texttoggle.h"

struct LanguageToggle : public TextToggle, public SettingsToggle
{
    LanguageToggle(const std::string & variable_name, const std::string & currentValue);
    std::string GetConfigValue() const override;
    void SetValueFromConfig(const std::string & value) override;
};
using LanguageTogglePtr = std::shared_ptr<LanguageToggle>;

#endif
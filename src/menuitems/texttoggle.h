/**
  * Copyright (C) 2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef TEXTTOGGLE_H_
#define TEXTTOGGLE_H_

#include "genericmenuentry.h"
#include <vector>

struct TextToggle : public GenericMenuEntry
{
    TextToggle();
    void setSelectedValue(std::string currentValue);
    void setSelectedValue(int newValue);
    unsigned int selectedValue;
    std::vector<std::string> values;
};

#endif
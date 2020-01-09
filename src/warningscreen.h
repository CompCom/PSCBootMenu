/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef WARNINGSCREEN_H_
#define WARNINGSCREEN_H_

#include "menuscreen.h"

class WarningScreen : public MenuScreen
{
public:
    WarningScreen(const std::string & warningText, const MenuItemPtr & item, bool isError = false);
    ~WarningScreen();
    void Init() override;

private:
    std::string warningText;
    bool isError;
};

#endif

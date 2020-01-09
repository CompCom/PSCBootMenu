/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef LOADSCREEN_H_
#define LOADSCREEN_H_

#include "menuscreen.h"
#include <chrono>

struct BootItem;

class LoadScreen : public MenuScreen
{
public:
    LoadScreen(const std::list<std::unique_ptr<BootItem>> & bootItems, BootItem * autoBootItem);
    void Init() override;
    void Update() override;
private:
    virtual void handleButtonPress(const GameControllerEvent * event);

    BootItem* autoBootItem;
    const std::list<std::unique_ptr<BootItem>> & bootItems;
    std::chrono::time_point<std::chrono::system_clock> bootTime;
};

#endif // LOADSCREEN_H_

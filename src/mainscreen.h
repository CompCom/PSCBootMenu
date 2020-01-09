/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef MAINSCREEN_H_
#define MAINSCREEN_H_

#include "menuscreen.h"
#include <map>

class ScrollingContainer;
struct BootItem;

class MainScreen : public MenuScreen
{
public:
    MainScreen(const std::list<std::unique_ptr<BootItem>> & bootItems);
    ~MainScreen();
    void Init() override;
    bool HandleSDLEvent(SDL_Event * e) override;
    void Update() override;

private:
    void handleButtonPress(const GameControllerEvent * event) override;
    bool saveToJSON(BootItem * bootItem);
    void selectBootItem(BootItem * bootItem);
    void setAutoBoot(BootItem * autoBootItem);

    const std::list<std::unique_ptr<BootItem>> & bootItems;
    std::shared_ptr<ScrollingContainer> container;
};

#endif

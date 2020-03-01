/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "menuscreen.h"
#include "menuitems/menuitem.h"
#include <map>

struct SettingsToggle;
class SDL_Context;
extern std::string cfgLocation;
extern std::string defaultCfgLocation;
extern std::string uiThemesFolder;

class SettingsScreen : public MenuScreen
{
public:
    SettingsScreen();
    ~SettingsScreen();
    virtual void Init();

private:
    void handleButtonPress(const GameControllerEvent * event) override;
    bool loadVariableTypes();
    void saveConfig();

    std::map<std::string, std::string> variableTypeMap;
    std::map<std::string, std::shared_ptr<SettingsToggle>> known_variables;
    std::map<std::string, std::string> unk_variables;
    std::list<std::string> lines;
};

#endif

/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "gamebutton.h"
#include "guihelper.h"
#include "languagemanager.h"
#include "menuitems/booltoggle.h"
#include "menuitems/itemcollection.h"
#include "menuitems/languagetoggle.h"
#include "menuitems/pushbutton.h"
#include "menuitems/themetoggle.h"
#include "menuitems/vitemscrollcollection.h"
#include "menuscreenmanager.h"
#include "settings.h"
#include "warningscreen.h"
#include "json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <set>

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

extern fs::path boot_menu_folder;

std::string cfgLocation;
std::string defaultCfgLocation;
std::string uiThemesFolder;

#ifdef SEGA
constexpr GameButton SaveBtn = GameButtons[GAME_BUTTON_SEGA_START], ToggleBtn = GameButtons[GAME_BUTTON_SEGA_A], BackBtn = GameButtons[GAME_BUTTON_SEGA_B];
#else
constexpr GameButton SaveBtn = GameButtons[GAME_BUTTON_START], ToggleBtn = GameButtons[GAME_BUTTON_X], BackBtn = GameButtons[GAME_BUTTON_O];
#endif // SEGA

SettingsScreen::SettingsScreen()
{
}
SettingsScreen::~SettingsScreen()
{
}

void SettingsScreen::Init()
{
    if(cfgLocation.size() == 0 || defaultCfgLocation.size() == 0)
    {
        manager->DisplayErrorScreen("Missing config location.", true);
        return;
    }

    if(loadVariableTypes() == false)
    {
        manager->DisplayErrorScreen("Error loading variable type data. See log for details.", true);
        return;
    }

    std::ifstream in;
    in.open(defaultCfgLocation);
    if(!in.is_open())
    {
        manager->DisplayErrorScreen("Cannot open default config.", true);
        return;
    }

    VItemScrollCollectionPtr container = std::make_shared<VItemScrollCollection>();
    container->displayItemCount = 6;
    container->yOffset = 64;
    container->y = 190;
    container->setArrowPosition(640+335+30);
    items.push_back(container);

    std::string temp;
    while(std::getline(in, temp))
    {
        if(temp.size() && (temp[0] == ';' || temp[0] == '['))
            lines.push_back(temp);
        else
        {
            std::string var_name = temp.substr(0, temp.find_first_of('='));
            std::string value = temp.substr(temp.find_first_of('=')+1);
            auto variableTypeIter = variableTypeMap.find(var_name);
            if(variableTypeIter != variableTypeMap.end())
            {
                if(variableTypeIter->second.compare("bool") == 0)
                {
                    known_variables[var_name] = std::make_shared<BoolToggle>(LanguageManager::GetString(var_name), value);
                    container->addItem(std::dynamic_pointer_cast<MenuItem>(known_variables[var_name]));
                }
                else if(variableTypeIter->second.compare("theme") == 0 && uiThemesFolder.size())
                {
                    known_variables[var_name] = std::make_shared<ThemeToggle>(LanguageManager::GetString(var_name), value);
                    container->addItem(std::dynamic_pointer_cast<MenuItem>(known_variables[var_name]));
                }
                else if(variableTypeIter->second.compare("language") == 0)
                {
                    known_variables[var_name] = std::make_shared<LanguageToggle>(LanguageManager::GetString(var_name), value);
                    container->addItem(std::dynamic_pointer_cast<MenuItem>(known_variables[var_name]));
                }
                else
                    unk_variables[var_name] = value;
            }
            else
                unk_variables[var_name] = value;

            lines.push_back(var_name);
        }
    }
    in.close();

    //Load existing values
    in.open(cfgLocation);
    if(in.is_open())
    {
        while(std::getline(in, temp))
        {
            if(temp.size() && temp[0] != ';' && temp[0] != '[')
            {
                std::string var_name = temp.substr(0, temp.find_first_of('='));
                std::string value = temp.substr(temp.find_first_of('=')+1);
                if(value[0] != '"')
                    value.insert(value.begin(), '"');
                if(value[value.size()-1] != '"')
                    value.push_back('"');
                auto variableTypeIter = variableTypeMap.find(var_name);
                if(variableTypeIter != variableTypeMap.end())
                {
                    if((variableTypeIter->second.compare("bool") == 0) ||
                       (variableTypeIter->second.compare("theme") == 0 && uiThemesFolder.size()) ||
                       (variableTypeIter->second.compare("language") == 0))
                        known_variables[var_name]->SetValueFromConfig(value);
                    else
                        unk_variables[var_name] = value;
                }
                else
                    unk_variables[var_name] = value;
            }
        }
        in.close();
    }

    GuiHelper & guiHelper = GuiHelper::GetInstance();
    textures.push_back(guiHelper.GetTexture(BACKGROUND));
    textures.push_back(guiHelper.GetTexture(FOOTER));
    guiHelper.CreateFooterItems({
                                    {SaveBtn.GUI_Value, LanguageManager::GetString("save_reset")},
                                    {ToggleBtn.GUI_Value, LanguageManager::GetString("toggle")},
                                    {BackBtn.GUI_Value, LanguageManager::GetString("back")},
                                    //{GuiElement::TRIANGLE_BUTTON, "Restore Defaults"}
                                },
                                textures);
    textures.push_back(guiHelper.CreateTitleTexture(LanguageManager::GetString("set_title"), 640, 72));

    for(int i = 0; i < 6; ++i)
        textures.insert(textures.end(), guiHelper.GetTexture(GuiElement::LINE_BREAK))->SetPosition(640, container->y+32+64*i, true);
}

void SettingsScreen::handleButtonPress(const GameControllerEvent * event)
{
    switch(event->button)
    {
    case BackBtn.SDL_Value:
        if(event->state == 1)
        {
            manager->RemoveCurrentScreen();
            return;
        }
        break;
    case SaveBtn.SDL_Value:
        if(event->state == 1)
        {
            ItemCollectionPtr container = std::make_shared<ItemCollection>();
            container->collectionType = ItemCollection::HORIZONTAL;

            PushButtonPtr no = std::make_shared<PushButton>("NO", 28, 740, 560);
            no->onPress = std::bind(&MenuScreenManager::RemoveCurrentScreen, manager);
            container->items.push_back(no);
            PushButtonPtr yes = std::make_shared<PushButton>("YES", 28, 540, 560);
            container->items.push_back(yes);
#ifdef SEGA
            yes->onPress = [this]() { this->saveConfig(); manager->RemoveAllScreens(); system("echo -n restart > /tmp/launchfilecommand"); };
            manager->AddNewScreen<WarningScreen>("Are you sure you want to overwrite the old settings?\nConsole will restart after saving.", container);
#else
            yes->onPress = [this]() { this->saveConfig(); manager->RemoveAllScreens(); };
            manager->AddNewScreen<WarningScreen>("Are you sure you want to overwrite the old settings?\nConsole will shut down after saving.", container);
#endif
        }
        break;
    default:
        MenuScreen::handleButtonPress(event);
        break;
    }
}

bool SettingsScreen::loadVariableTypes()
{
    std::ifstream in(boot_menu_folder/"modifiable_variables.json");

    if(in.is_open() == false)
    {
        std::cerr << "Could not open cfg variable json." << std::endl;
        return false;
    }

    json j;
    try
    {
        in >> j;
        in.close();
    }
    catch(...)
    {
        std::cerr << "Could not parse cfg variable json." << std::endl;
        in.close();
        return false;
    }

    try
    {
        for (auto& item : j.items())
        {
            for(std::string v : item.value())
                variableTypeMap[v] = item.key();
        }
    }
    catch(...)
    {
        std::cerr << "Error with values in cfg variable json." << std::endl;
        return false;
    }
    return true;
}

void SettingsScreen::saveConfig()
{
    std::ofstream out(cfgLocation);
    if(out.is_open())
    {
        for(auto & line : lines)
        {
            if(line[0] == ';' || line[0] == '[')
                out << line << "\n";
            else if(known_variables.count(line))
            {
                out << line << "=" << known_variables[line]->GetConfigValue() << "\n";
            }
            else
            {
                out << line << "=" << unk_variables[line] << "\n";
            }
        }
        out.close();
        manager->RemoveAllScreens();
    }
    else
    {
        std::cerr << "Could not open cfg file: " << cfgLocation << std::endl;
        manager->RemoveAllScreens();
        manager->DisplayErrorScreen("Could not open cfg file.");
    }
}

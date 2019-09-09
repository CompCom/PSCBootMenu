#include "settings.h"
#include "menuscreenmanager.h"
#include "warningscreen.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <set>

using json = nlohmann::json;

std::string cfgLocation;
std::string defaultCfgLocation;
std::string uiThemesFolder;

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
                    bool_variables[var_name] = std::make_shared<BoolToggle>(var_name, value.compare("\"1\"") == 0, renderer);
                else if(variableTypeIter->second.compare("theme") == 0 && uiThemesFolder.size())
                    theme_variable = std::make_shared<ThemeToggle>(renderer, var_name, value);
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
                    if(variableTypeIter->second.compare("bool") == 0)
                        bool_variables[var_name]->value = value.compare("\"1\"") == 0;
                    else if(variableTypeIter->second.compare("theme") == 0 && uiThemesFolder.size())
                        theme_variable->setSelectedValue(value);
                    else
                        unk_variables[var_name] = value;
                }
                else
                    unk_variables[var_name] = value;
            }
        }
        in.close();
    }

    trueTexture = Texture(font, "TRUE", 16, renderer);
    falseTexture = Texture(font, "FALSE", 16, renderer);

    ItemCollectionPtr container = std::make_shared<ItemCollection>();
    for(auto & item : bool_variables)
        container->addItem(item.second);
    if(uiThemesFolder.size())
        container->addItem(theme_variable);
    items.push_back(container);

    HItemCollectionPtr saveContainer = std::make_shared<HItemCollection>();
    items.push_back(saveContainer);
    PushButtonPtr cancel = std::make_shared<PushButton>("CANCEL", 16, renderer, 740, 660);
    cancel->onPress = std::bind(&MenuScreenManager::RemoveCurrentScreen, manager);
    saveContainer->items.push_back(cancel);

    PushButtonPtr save = std::make_shared<PushButton>("SAVE", 16, renderer, 540, 660);
    save->onPress = [&,this]()
    {
        HItemCollectionPtr container = std::make_shared<HItemCollection>();

        PushButtonPtr no = std::make_shared<PushButton>("NO", 28, renderer, 740, 560);
        no->onPress = std::bind(&MenuScreenManager::RemoveCurrentScreen, manager);
        container->items.push_back(no);
        PushButtonPtr yes = std::make_shared<PushButton>("YES", 28, renderer, 540, 560);
        yes->onPress = [&,this]() { this->saveConfig(); manager->RemoveAllScreens(); };
        container->items.push_back(yes);

        manager->AddNewScreen(std::make_shared<WarningScreen>("       Are you sure you want to overwrite the old settings?\n                Console will shut down after saving.", container));
    };
    saveContainer->items.push_back(save);

    textures.emplace_back(fontBold, "BleemSync Settings", 36, renderer, 640, 80, true);
}

bool SettingsScreen::loadVariableTypes()
{
#ifdef __arm__
    std::ifstream in("/media/bleemsync/etc/boot_menu/modifiable_variables.json");
#else
    std::ifstream in("modifiable_variables.json");
#endif // __arm__

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
            else if(bool_variables.count(line))
            {
                out << line << "=";
                if(bool_variables[line]->value)
                    out << "\"1\"\n";
                else
                    out << "\"0\"\n";
            }
            else if(line.compare("selected_theme") == 0 && uiThemesFolder.size())
            {
                out << line << "=\"" << theme_variable->values[theme_variable->selectedValue] << "\"\n";
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

#include "settings.h"
#include "framework/sdl_helper.h"
#include "menuitem.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <list>
#include <set>

using json = nlohmann::json;

extern std::vector<GameControllerEvent> ControllerEvents;
std::string cfgLocation;
std::string defaultCfgLocation;
std::string uiThemesFolder;

bool SaveConfirm(SDL_Context* sdl_context, std::vector<GameControllerPtr> & controllers)
{
    Texture heading(fontBold, "WARNING:", 36, sdl_context->renderer, 640, 180, true, 0xFF, 0, 0);
    Texture warningMessage1(font, "Are you sure you want to overwrite the old settings?", 28, sdl_context->renderer, 640, 340, true);
    Texture warningMessage2(font, "Console will shut down after saving.", 28, sdl_context->renderer, 640, 370, true);
    HItemCollectionPtr container = std::make_shared<HItemCollection>();
    bool result = false;
    bool isRunning = true;

    PushButtonPtr no = std::make_shared<PushButton>("NO", 28, sdl_context, 740, 560);
    no->onPress = [&]() { isRunning = false; };
    container->items.push_back(no);
    PushButtonPtr yes = std::make_shared<PushButton>("YES", 28, sdl_context, 540, 560);
    yes->onPress = [&]() { isRunning = false; result = true; };
    container->items.push_back(yes);

    ControllerEvents.clear();
    unsigned int selectedItem = 0;
    while(isRunning)
    {
        for(auto & controller : controllers)
            controller->Update();

        SDL_Event e;
        //Poll for quit event (CTRL+C)/Close
        while (SDL_PollEvent(&e)) {
            switch(e.type)
            {
            case SDL_CONTROLLERDEVICEADDED:
                {
                    controllers.emplace_back(std::make_unique<GameController>(e.cdevice.which));
                    std::cout << "Added device id: " << e.cdevice.which << std::endl;
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                {
                    auto controllerPtr = SDL_GameControllerFromInstanceID(e.cdevice.which);
                    controllers.erase(std::remove_if(controllers.begin(), controllers.end(), [&](const GameControllerPtr & c) { return c->GetController() == controllerPtr; }), controllers.end());
                }
                break;
            case SDL_QUIT:
                exit(1);
                break;
            }
        }
        for(const auto & event : ControllerEvents)
        {
            if(event.state)
                container->handleButtonPress(event.button, event.state);
        }
        ControllerEvents.clear();

        sdl_context->StartFrame();
        heading.Draw(sdl_context->renderer);
        warningMessage1.Draw(sdl_context->renderer);
        warningMessage2.Draw(sdl_context->renderer);
        container->draw(sdl_context->renderer, true);
        sdl_context->EndFrame();
    }
    return result;
}

bool LoadVariableTypes(std::map<std::string, std::string> & variableTypeMap)
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

void RunSettingsMenu(SDL_Context* sdl_context, std::vector<GameControllerPtr> & controllers)
{
    if(cfgLocation.size() == 0 || defaultCfgLocation.size() == 0)
        return;

    std::map<std::string, std::string> variableTypeMap;
    if(LoadVariableTypes(variableTypeMap) == false)
        return;

    std::map<std::string, BoolTogglePtr> bool_variables;
    std::map<std::string, std::string> unk_variables;
    ThemeTogglePtr theme_variable;

    std::ifstream in;
    in.open(defaultCfgLocation);
    if(!in.is_open())
        return;

    std::list<std::string> lines;
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
                    bool_variables[var_name] = std::make_shared<BoolToggle>(sdl_context, var_name, value.compare("\"1\"") == 0);
                else if(variableTypeIter->second.compare("theme") == 0 && uiThemesFolder.size())
                    theme_variable = std::make_shared<ThemeToggle>(sdl_context, var_name, value);
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

    bool runSettings = true;
    trueTexture = Texture(font, "TRUE", 16, sdl_context->renderer);
    falseTexture = Texture(font, "FALSE", 16, sdl_context->renderer);
    std::vector<MenuItemPtr> items;

    ItemCollectionPtr container = std::make_shared<ItemCollection>();
    for(auto & item : bool_variables)
        container->addItem(item.second);
    if(uiThemesFolder.size())
        container->addItem(theme_variable);
    items.push_back(container);

    HItemCollectionPtr saveContainer = std::make_shared<HItemCollection>();
    items.push_back(saveContainer);
    PushButtonPtr cancel = std::make_shared<PushButton>("CANCEL", 16, sdl_context, 740, 660);
    cancel->onPress = [&]() { runSettings = false; };
    saveContainer->items.push_back(cancel);

    PushButtonPtr save = std::make_shared<PushButton>("SAVE", 16, sdl_context, 540, 660);
    save->onPress = [&]()
    {
        if(SaveConfirm(sdl_context, controllers) == false)
            return;
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
            exit(0);
        }
        else
        {
            std::cerr << "Could not open cfg file: " << cfgLocation << std::endl;
            exit(1);
        }
    };
    saveContainer->items.push_back(save);

    Texture heading(fontBold, "BleemSync Settings", 36, sdl_context->renderer, 640, 80, true);
    unsigned int selectedItem = 0;
    ControllerEvents.clear();
    while(runSettings)
    {
        for(auto & controller : controllers)
            controller->Update();

        SDL_Event e;
        //Poll for quit event (CTRL+C)/Close
        while (SDL_PollEvent(&e)) {
            switch(e.type)
            {
            case SDL_CONTROLLERDEVICEADDED:
                {
                    controllers.emplace_back(std::make_unique<GameController>(e.cdevice.which));
                    std::cout << "Added device id: " << e.cdevice.which << std::endl;
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                {
                    auto controllerPtr = SDL_GameControllerFromInstanceID(e.cdevice.which);
                    controllers.erase(std::remove_if(controllers.begin(), controllers.end(), [&](const GameControllerPtr & c) { return c->GetController() == controllerPtr; }), controllers.end());
                }
                break;
            case SDL_QUIT:
                exit(1);
                break;
            }
        }
        for(int i = 0; i < ControllerEvents.size(); ++i)
        {
            auto & event = ControllerEvents[i];
            if(event.state)
            {
                int result = items[selectedItem]->handleButtonPress(event.button, event.state);
                if(result == -1 && selectedItem != 0)
                    --selectedItem;
                else if(result == 1 && selectedItem != items.size()-1)
                    ++selectedItem;
            }
        }
        ControllerEvents.clear();

        sdl_context->StartFrame();
        heading.Draw(sdl_context->renderer);
        for(unsigned int i = 0, count = items.size(); i < count; ++i)
            items[i]->draw(sdl_context->renderer, i == selectedItem);
        sdl_context->EndFrame();
    }

    //Cleanup
    trueTexture.texture.reset();
    falseTexture.texture.reset();
    sdl_context->ResetTimer();
}

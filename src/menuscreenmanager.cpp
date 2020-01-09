/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "menuitems/pushbutton.h"
#include "menuscreenmanager.h"
#include "menuscreen.h"
#include "warningscreen.h"

#include <algorithm>
#include <iostream>
#include <experimental/filesystem>

#ifdef SEGA
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <linux/input.h>
#endif

namespace fs = std::experimental::filesystem;
extern fs::path boot_menu_folder;

MenuScreenManager::MenuScreenManager(SDL_Context * sdl_context) : sdl_context(*sdl_context)
{
    //Load SDL Game Controller Mappings
    fs::path game_mapping_file = boot_menu_folder/"gamecontrollerdb.txt";
    if(fs::exists(game_mapping_file))
        SDL_GameControllerAddMappingsFromFile(game_mapping_file.c_str());

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if(SDL_IsGameController(i))
            controllers.emplace_back(std::make_unique<GameController>(i, controllerEvents));
    }

    //Clear SDL Events
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

    sdl_context->StartFrame();
    sdl_context->EndFrame();

#ifdef SEGA
    // Find and open input event file for sega power button input
    openPowerButtonEventFile();
#endif
}
MenuScreenManager::~MenuScreenManager()
{
#ifdef SEGA
    if(powerFd != -1)
        close(powerFd);
#endif
}

void MenuScreenManager::AddNewScreen(const std::shared_ptr<MenuScreen> & screen)
{
    controllerEvents.clear();
    screens.push_back(screen);
    screen->AttachManager(this);
    screen->Init();
    sdl_context.ResetTimer();
}

void MenuScreenManager::RemoveCurrentScreen()
{
    if(screens.size())
        screens.erase(--screens.end());
    controllerEvents.clear();
}

void MenuScreenManager::RemoveAllScreens()
{
    screens.clear();
}

bool MenuScreenManager::ScreenAvailable() const
{
    return screens.size();
}

void MenuScreenManager::Update()
{
#ifdef SEGA
    /* 
        If sega power gpio event file has been opened successfully
        scan input events for power or reset(home) button presses
    */ 
    if(powerFd != -1)
    {
        input_event e;
        if(read(powerFd, &e, sizeof(input_event)) > 0)
        {
            if(e.type == 1)
            {
                if(e.code == KEY_POWER)
                {
                    system("echo -n \"shutdown-detection 1\" > /tmp/launchfilecommand");
                    screens.clear();
                    return;
                }
                else if(e.code == KEY_HOME)
                {
                    system("echo -n restart > /tmp/launchfilecommand");
                    screens.clear();
                    return;
                }
            }
        }
    }
#endif
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if(screens.size() && screens.back()->HandleSDLEvent(&e))
            continue;
        switch(e.type)
        {
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            addRemoveController(e);
            break;
        case SDL_KEYDOWN:
            {
                switch(e.key.keysym.scancode)
                {
                case SDL_SCANCODE_AUDIOPLAY:
                case SDL_SCANCODE_SLEEP:
                    screens.clear();
                    return;
                default:
                    break;
                }
            }
            break;
        case SDL_QUIT:
            screens.clear();
            return;
        }
    }
    updateControllers();
}

void MenuScreenManager::Run()
{
    std::shared_ptr<MenuScreen> currentScreen = screens.back();
    Update();
    currentScreen->Update();
    sdl_context.StartFrame();
    currentScreen->Render();
    sdl_context.EndFrame();
    controllerEvents.clear();
}

void MenuScreenManager::DisplayErrorScreen(const std::string & error, bool removeCurrentScreen)
{
    if(removeCurrentScreen)
        RemoveCurrentScreen();

    PushButtonPtr okayButton = std::make_shared<PushButton>("OK", 28, 640, 560);
    okayButton->onPress = std::bind(&MenuScreenManager::RemoveCurrentScreen, this);
    AddNewScreen<WarningScreen>(error, okayButton, true);
}

void MenuScreenManager::addRemoveController(SDL_Event & e)
{
    switch(e.type)
    {
    case SDL_CONTROLLERDEVICEADDED:
        {
            controllers.emplace_back(std::make_unique<GameController>(e.cdevice.which, controllerEvents));
            std::cout << "Added device id: " << e.cdevice.which << std::endl;
        }
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        {
            auto controllerPtr = SDL_GameControllerFromInstanceID(e.cdevice.which);
            controllers.erase(std::remove_if(controllers.begin(), controllers.end(), [&](const GameControllerPtr & c) { return c->GetController() == controllerPtr; }), controllers.end());
        }
        break;
    }
}

void MenuScreenManager::updateControllers()
{
    for(auto & controller : controllers)
        controller->Update();
}

SDL_Renderer * MenuScreenManager::GetRenderer() const
{
    return sdl_context.renderer;
}

std::vector<GameControllerEvent> & MenuScreenManager::GetControllerEvents()
{
    return controllerEvents;
}

const std::vector<GameControllerPtr> & MenuScreenManager::GetControllers() const
{
    return controllers;
}

#ifdef SEGA
void MenuScreenManager::openPowerButtonEventFile()
{
    for(auto & entry : fs::directory_iterator("/dev/input/"))
    {
        std::string name = entry.path().filename();
        if(name.find("event") != std::string::npos)
        {
            std::ifstream in("/sys/class/input/"+name+"/device/name");
            if(in.good())
            {
                std::string temp;
                std::getline(in, temp);
                in.close();
                if(temp.compare("gpio-keys") == 0)
                {
                    powerFd = open(("/dev/input/"+name).c_str(), O_RDONLY | O_NONBLOCK);
                    return;
                }
            }
        }
    }
    powerFd = -1;
}
#endif

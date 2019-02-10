/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "framework/sdl_helper.h"
#include "gamecontroller.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <list>

using GameControllerPtr = std::unique_ptr<GameController>;

std::list<GameControllerEvent> ControllerEvents;

struct MenuItem
{
    Texture logo, itemText, selectedText;
    std::string launchCommand;
    bool selected = false;
    MenuItem(std::string logo_file, std::string itemName, std::string launchCommand, SDL_Renderer* renderer, int posX, int posY)
    : launchCommand(launchCommand)
    {
        logo = Texture(logo_file, renderer, posX, posY-14, true);
        itemText = Texture(itemName, 24, renderer, posX, posY+142, true);
        selectedText = Texture(itemName, 24, renderer, posX, posY+142, true, 0xFF00FFFF);
    };
    void Draw(SDL_Renderer* renderer)
    {
        logo.Draw(renderer);
        selected ? selectedText.Draw(renderer) : itemText.Draw(renderer);
    }
};

void handleButtonPress(uint8_t gamebutton, MenuItem * items)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        items[0].selected = !items[0].selected;
        items[1].selected = !items[1].selected;
        break;
    case SDL_CONTROLLER_BUTTON_A:
        std::ofstream out("/tmp/launchfilecommand");
        if(items[0].selected)
            out << items[0].launchCommand;
        else
            out << items[1].launchCommand;
        out.close();
        exit(0);
    }
}

int main(int argc, char * argv[])
{
    SDL_Context sdl_context(std::chrono::milliseconds(33));
    auto renderer = sdl_context.renderer;
    //Using vector because list SEGFAULTS
    std::vector<GameControllerPtr> controllers;
    SDL_GameControllerAddMapping("030000004c050000da0c000011010000,Sony Interactive Entertainment Controller,x:b3,a:b2,b:b1,y:b0,back:b8,start:b9,leftshoulder:b6,lefttrigger:b4,rightshoulder:b7,righttrigger:b5,leftx:a0,lefty:a1");

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if(std::string(SDL_JoystickNameForIndex(i)).find("Sony Interactive Entertainment Controller") != std::string::npos)
            controllers.emplace_back(std::make_unique<GameController>(i));
    }

    Texture Heading("Boot Menu", 48, renderer, 640, 80, true);
    MenuItem items[2] = { MenuItem("images/retroarch.png", "RetroArch", "/media/lolhack/retroarch.sh", renderer, 440, 360),
    MenuItem("images/bleem_logo.png", "BleemSync", "/media/lolhack/boot.sh", renderer, 840, 360) };
    items[0].selected = true;
    Texture CompComText("created by CompCom", 16, renderer, 1090, 678, true);

    while(true)
    {
        sdl_context.StartFrame();
        for(auto & controller : controllers)
        {
            controller->Update();
        }

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
                return 0;
            }
        }
        for(const auto & event : ControllerEvents)
        {
            if(event.state)
                handleButtonPress(event.button, items);
        }
        ControllerEvents.clear();

        Heading.Draw(renderer);
        for(auto & item : items)
            item.Draw(renderer);
        CompComText.Draw(renderer);

        sdl_context.EndFrame();
    }

    return 0;
}

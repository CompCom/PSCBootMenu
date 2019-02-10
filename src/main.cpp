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
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <list>
#include <string>
#include <experimental/filesystem>

#define BOOT_MENU_VERSION "0.5.0"

namespace fs = std::experimental::filesystem;
using GameControllerPtr = std::unique_ptr<GameController>;

std::list<GameControllerEvent> ControllerEvents;
std::string imageFolder("images");
std::string themeFolder;

Texture GetTexture(const std::string & pngFilePath, SDL_Renderer* renderer, int x = 0, int y = 0, bool centerImg = false)
{
    if(themeFolder.size() && fs::exists(themeFolder+pngFilePath))
    {
        return Texture(themeFolder+pngFilePath, renderer, x, y, centerImg);
    }
    else if(fs::exists(imageFolder+pngFilePath))
    {
        return Texture(imageFolder+pngFilePath, renderer, x, y, centerImg);
    }
    return Texture();
}

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
    MenuItem(std::string logo_file, std::string textFile, std::string hoverFile, std::string launchCommand, SDL_Renderer* renderer, int logoPosX, int logoPosY, int textPosX, int textPosY)
    : launchCommand(launchCommand)
    {
        logo = GetTexture(logo_file, renderer, logoPosX, logoPosY, true);
        itemText = GetTexture(textFile, renderer, textPosX, textPosY, true);
        selectedText = GetTexture(hoverFile, renderer, textPosX, textPosY, true);
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
        if(out.is_open())
        {
            if(items[0].selected)
                out << items[0].launchCommand;
            else
                out << items[1].launchCommand;
            out.close();
            exit(0);
        }
        else
            exit(1);
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

    //Init Audio
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,256);

    std::string musicFolder;
    //Check arguments
    for(int i = 1; i < argc; ++i)
    {
        if(strcasecmp(argv[i], "--image-folder") == 0)
        {
            imageFolder = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--music-folder") == 0)
        {
            musicFolder = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--theme-folder") == 0)
        {
            themeFolder = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--version") == 0)
        {
            std::cout << "PSC Boot Menu by CompCom version " << BOOT_MENU_VERSION << std::endl;
            return 0;
        }
    }

    if(musicFolder.size())
    {
        std::string wavFilePath;
        if(themeFolder.size() && fs::exists(themeFolder+"/bootmenu.wav"))
            wavFilePath = themeFolder+"/bootmenu.wav";
        else if(fs::exists(musicFolder+"/bootmenu.wav"))
            wavFilePath = musicFolder+"/bootmenu.wav";

        if(wavFilePath.size())
        {
            Mix_Chunk* wav = Mix_LoadWAV(wavFilePath.c_str());
            if(!wav || Mix_PlayChannel(-1, wav, -1) == -1)
                std::cerr << Mix_GetError() << std::endl;
        }
        else
            std::cerr << "Cannot open file: bootmenu.wav" << std::endl;
    }

    Texture background = GetTexture("/Background.png", renderer);
    MenuItem items[2] = { MenuItem("/RetroArch_Icon.png", "/RA.png", "/RA_Hover.png", "launch_retroarch", renderer, 384, 342, 385, 488),
    MenuItem("/BleemSync_Icon.png", "/BS.png", "/BS_Hover.png", "launch_StockUI", renderer, 896, 342, 896, 488), };
    items[0].selected = true;

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
            case SDL_KEYDOWN:
                {
                    switch(e.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_SLEEP:
                        return 0;
                    case SDL_SCANCODE_AUDIOPLAY:
                        handleButtonPress(SDL_CONTROLLER_BUTTON_DPAD_RIGHT, items);
                        break;
                    case SDL_SCANCODE_EJECT:
                        handleButtonPress(SDL_CONTROLLER_BUTTON_A, items);
                        break;
                    default:
                        break;
                    }
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

        background.Draw(renderer);
        for(auto & item : items)
            item.Draw(renderer);

        sdl_context.EndFrame();
    }

    return 0;
}

/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "menuscreenmanager.h"
#include "mainscreen.h"
#include "settings.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include <SDL_mixer.h>
#include <experimental/filesystem>

#define BOOT_MENU_VERSION "0.8.0"

namespace fs = std::experimental::filesystem;

fs::path imageFolder("images");
fs::path themeFolder;

int main(int argc, char * argv[])
{
    SDL_Context sdl_context(std::chrono::milliseconds(16));

    //Check arguments
    std::string musicFolder;
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
        else if(strcasecmp(argv[i], "--cfg-file") == 0)
        {
            cfgLocation = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--default-cfg-file") == 0)
        {
            defaultCfgLocation = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--ui-themes-folder") == 0)
        {
            uiThemesFolder = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--version") == 0)
        {
            std::cout << "PSC Boot Menu by CompCom version " << BOOT_MENU_VERSION << std::endl;
            return 0;
        }
    }

    //Init Audio
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);

    if(musicFolder.size())
    {
        std::string wavFilePath;
        if(fs::exists(themeFolder/"bootmenu.wav"))
            wavFilePath = themeFolder/"bootmenu.wav";
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

    MenuScreenManager manager(&sdl_context);
    manager.AddNewScreen(std::make_shared<MainScreen>());

    while(manager.ScreenAvailable())
        manager.Run();

    Mix_CloseAudio();

    return 0;
}

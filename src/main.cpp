/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#include "bootitem.h"
#include "font.h"
#include "guihelper.h"
#include "languagemanager.h"
#include "loadscreen.h"
#include "menuscreenmanager.h"
#include "mainscreen.h"
#include "settings.h"
#include <iostream>

#define BOOT_MENU_VERSION "1.0.0"

namespace fs = std::experimental::filesystem;

fs::path boot_menu_folder = "./";
std::string musicFolder;

int main(int argc, char * argv[])
{
    SDL_Context sdl_context(std::chrono::milliseconds(16));
    GuiHelper & guiHelper = GuiHelper::GetInstance();

    //Check arguments
    bool autoBoot = false;
    std::string languageFile;
    for(int i = 1; i < argc; ++i)
    {
        if(strcasecmp(argv[i], "--image-folder") == 0)
        {
            guiHelper.SetImageFolder(argv[i+1]);
            ++i;
        }
        else if(strcasecmp(argv[i], "--music-folder") == 0)
        {
            musicFolder = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--theme-folder") == 0)
        {
            guiHelper.SetThemeFolder(argv[i+1]);
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
        else if(strcasecmp(argv[i], "--language-file") == 0)
        {
            languageFile = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--boot-menu-folder") == 0)
        {
            boot_menu_folder = argv[i+1];
            ++i;
        }
        else if(strcasecmp(argv[i], "--auto-boot") == 0)
        {
            autoBoot = true;
        }
        else if(strcasecmp(argv[i], "--version") == 0)
        {
            std::cout << "PSC Boot Menu by CompCom version " << BOOT_MENU_VERSION << std::endl;
            return 0;
        }
    }

    //Populate default language file path
    if(languageFile.size() == 0 || fs::is_regular_file(languageFile) == false)
        languageFile = boot_menu_folder/"languages/ENG.json";

    MenuScreenManager manager(&sdl_context);
    guiHelper.AttachRenderer(sdl_context.renderer);

    //Load Boot Items and sort them into correct order.
    std::list<std::unique_ptr<BootItem>> bootItems = GetBootItems(boot_menu_folder/"boot_items");
    bootItems.sort([](const std::unique_ptr<BootItem> & a, const std::unique_ptr<BootItem> & b) { return a->filename < b->filename; });

    //Load Fonts, Language and then handle auto booting or booting to main screen
    if(FontManager::GetInstance().LoadFonts() == false)
    {
        std::cerr << "Failed to load fonts." << std::endl;
        manager.DisplayErrorScreen("Failed to load font JSON.", false);
    }
    else if(LanguageManager::LoadLanguage(languageFile) == false)
    {
        std::cerr << "Failed to load language." << std::endl;
        manager.DisplayErrorScreen("Failed to load language JSON.", false);
    }
    else
    {
        if(bootItems.size() == 0)
        {
            manager.DisplayErrorScreen("No menu items found.", false);
        }
        else
        {
            BootItem * autoBootItem = nullptr;
            if(autoBoot)
            {
                for(const auto & bootItem : bootItems)
                {
                    if(bootItem->autoBoot)
                    {
                        autoBootItem = bootItem.get();
                        break;
                    }
                }
            }

            if(autoBootItem)
                manager.AddNewScreen<LoadScreen>(bootItems, autoBootItem);
            else
                manager.AddNewScreen<MainScreen>(bootItems);
        }
    }

    while(manager.ScreenAvailable())
        manager.Run();

    return 0;
}

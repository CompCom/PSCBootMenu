/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "bootitem.h"
#include "gamebutton.h"
#include "guihelper.h"
#include "languagemanager.h"
#include "mainscreen.h"
#include "menuitems/imagepushbutton.h"
#include "menuitems/messagebox.h"
#include "menuitems/scrollingcontainer.h"
#include "menuscreenmanager.h"
#include "networkscreen.h"
#include "settings.h"
#include <fstream>
#include <iostream>
#include <SDL_mixer.h>
#include <thread>

extern fs::path boot_menu_folder;
extern std::string musicFolder;

#ifdef SEGA
constexpr GameButton SelectBtn = GameButtons[GAME_BUTTON_SEGA_A], SettingsBtn = GameButtons[GAME_BUTTON_SEGA_B], NetworkBtn = GameButtons[GAME_BUTTON_SEGA_C], AutoBootBtn = GameButtons[GAME_BUTTON_SEGA_START];
#else
constexpr GameButton SelectBtn = GameButtons[GAME_BUTTON_X], SettingsBtn = GameButtons[GAME_BUTTON_L1], NetworkBtn = GameButtons[GAME_BUTTON_R1], AutoBootBtn = GameButtons[GAME_BUTTON_TRIANGLE];
#endif // SEGA

MainScreen::MainScreen(const std::list<std::unique_ptr<BootItem>> & bootItems) : bootItems(bootItems)
{

}
MainScreen::~MainScreen()
{
    Mix_CloseAudio();
}

void MainScreen::Init()
{
    GuiHelper & guiHelper = GuiHelper::GetInstance();
    textures.emplace_back(guiHelper.GetTexture("Boot/Background.png"));
    guiHelper.CreateFooterItems({
                                    {SelectBtn.GUI_Value, LanguageManager::GetString("select")},
                                    {SettingsBtn.GUI_Value, LanguageManager::GetString("settings")},
                                    {NetworkBtn.GUI_Value, LanguageManager::GetString("network_manager")}
                                },
                                textures, 590, BOOT_FOOTER_TEXT);
    guiHelper.CreateFooterItems({
                                    {AutoBootBtn.GUI_Value, "Toggle AutoBoot"}
                                },
                                textures, 630, BOOT_FOOTER_TEXT);

    container = std::make_shared<ScrollingContainer>();
    container->x = 640;
    container->y = 360;
    container->offset = 430;
    items.push_back(container);

    for(auto & b : bootItems)
    {
        ImagePushButtonPtr button = std::make_shared<ImagePushButton>(guiHelper.GetTexture("Boot/"+b->texture), guiHelper.GetTexture("Boot/"+b->hoverTexture), renderer);
        button->onPress = std::bind(&MainScreen::selectBootItem, this, b.get());
        container->AddItem(button);

        if(b->isDefault)
            container->SetCurrentItem(container->items.size()-1);
    }

    //Init Audio
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);

    if(musicFolder.size())
    {
        std::string wavFilePath;
        if(fs::exists(guiHelper.GetThemeFolder()/"bootmenu.ogg"))
            wavFilePath = guiHelper.GetThemeFolder()/"bootmenu.ogg";
        else if(fs::exists(guiHelper.GetThemeFolder()/"bootmenu.wav"))
            wavFilePath = guiHelper.GetThemeFolder()/"bootmenu.wav";
        else if(fs::exists(musicFolder+"/bootmenu.ogg"))
            wavFilePath = musicFolder+"/bootmenu.ogg";
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
}

bool MainScreen::HandleSDLEvent(SDL_Event * e)
{
    if(e->type == SDL_KEYDOWN)
    {
        switch(e->key.keysym.scancode)
        {
            case SDL_SCANCODE_SLEEP:
                manager->RemoveAllScreens();
                return true;
            case SDL_SCANCODE_AUDIOPLAY:
                container->SetCurrentItem((container->currentItem+1) % container->items.size());
                return true;
            case SDL_SCANCODE_EJECT:
                manager->GetControllerEvents().push_back({SDL_CONTROLLER_BUTTON_A, 1});
                return true;
            default:
                return false;
        }
    }
    else
        return false;
}

void MainScreen::Update()
{
    auto & controllerEvents = manager->GetControllerEvents();
    for(const GameControllerEvent & event : controllerEvents)
    {
        handleButtonPress(&event);
    }

    for(unsigned int i = 0; i < items.size(); ++i)
        items[i]->Update();
}

void MainScreen::handleButtonPress(const GameControllerEvent * event)
{
    //Hacky method to prevent input while messagebox is open
    if(selectedItem != 0)
        return;

    switch(event->button)
    {
    case SettingsBtn.SDL_Value:
        if(event->state == 1)
        {
            manager->AddNewScreen<SettingsScreen>();
            return;
        }
        break;
    case NetworkBtn.SDL_Value:
        if(event->state == 1)
        {
            manager->AddNewScreen<NetworkScreen>();
            return;
        }
        break;
    case AutoBootBtn.SDL_Value:
        if(event->state == 1)
        {
            setAutoBoot(std::next(bootItems.begin(), container->currentItem)->get());
            return;
        }
        break;
    default:
        items[selectedItem]->handleButtonPress(event->button, event->state);
        break;
    }
}

void MainScreen::selectBootItem(BootItem * bootItem)
{
    if(bootItem->Launch())
        manager->RemoveAllScreens();
    else
        manager->DisplayErrorScreen("Cannot open launch command file.", true);
}

bool MainScreen::saveToJSON(BootItem * bootItem)
{
    switch(bootItem->Save())
    {
    case 0:
        //SUCCESS
        return true;
    case 1:
        manager->DisplayErrorScreen("Cannot open json file:\n" + bootItem->filename);
        break;
    case 2:
        manager->DisplayErrorScreen("Error occurred while writing json file:\n" + bootItem->filename);
        break;
    default:
        std::cerr << "No idea wtf just happened here." << std::endl;
        manager->DisplayErrorScreen("An unknown error occurred while writing json file:\n" + bootItem->filename);
        break;
    }
    return false;
}

void MainScreen::setAutoBoot(BootItem * autoBootItem)
{
    for(auto & bootItem : bootItems)
    {
        if(autoBootItem != bootItem.get() && bootItem->autoBoot)
        {
            bootItem->autoBoot = false;
            saveToJSON(bootItem.get());
        }
    }
    autoBootItem->autoBoot = !autoBootItem->autoBoot;
    if(saveToJSON(autoBootItem))
    {
        auto msgBox = std::make_shared<MessageBox>("Information", (autoBootItem->autoBoot) ? "AutoBoot has been set." : "AutoBoot has been unset.", renderer);
        items.push_back(msgBox);
        msgBox->setPosition(640,360);
        msgBox->backgroundTask = std::async(std::launch::async, []() { std::this_thread::sleep_for(std::chrono::seconds(1)); } );
        msgBox->endFunction =   [this, prevSelectedItem=selectedItem]()
                                {
                                    items.erase(--items.end());
                                    selectedItem = prevSelectedItem;
                                };
        selectedItem = items.size()-1;
    }
}

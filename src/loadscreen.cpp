/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "bootitem.h"
#include "font.h"
#include "gamebutton.h"
#include "guihelper.h"
#include "languagemanager.h"
#include "loadscreen.h"
#include "mainscreen.h"
#include "menuscreenmanager.h"

LoadScreen::LoadScreen(const std::list<std::unique_ptr<BootItem>> & bootItems, BootItem * autoBootItem) : autoBootItem(autoBootItem), bootItems(bootItems)
{

}

void LoadScreen::Init()
{
    GuiHelper & guiHelper = GuiHelper::GetInstance();
    textures.emplace_back(guiHelper.GetTexture("Boot/AutoBoot_Screen.png"));

    textures.insert(textures.end(), guiHelper.GetTexture("Boot/"+autoBootItem->hoverTexture))->SetPosition(640, 360, true);
    textures.push_back(FontManager::CreateTexture(HEADER_TEXT, LanguageManager::GetString("autobooting"), 33, 640, 160, true));
    guiHelper.CreateFooterItems({
                                    {GameButtons[GAME_BUTTON_O].GUI_Value, LanguageManager::GetString("cancel")},
                                },
                                textures, 580, BOOT_FOOTER_TEXT);

    using namespace std::chrono_literals;
    bootTime = std::chrono::system_clock::now()+1000ms;
}

void LoadScreen::Update()
{
    if(std::chrono::system_clock::now() >= bootTime)
    {
        manager->RemoveAllScreens();
        autoBootItem->Launch();
    }
    else
        MenuScreen::Update();
}

void LoadScreen::handleButtonPress(const GameControllerEvent * event)
{
    if(event->button == GameButtons[GAME_BUTTON_O].SDL_Value && event->state)
    {
        manager->RemoveAllScreens();
        manager->AddNewScreen<MainScreen>(bootItems);
    }
}

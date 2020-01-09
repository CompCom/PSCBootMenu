/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef MENUSCREENMANAGER_H_
#define MENUSCREENMANAGER_H_

#include "gamecontroller.h"
#include "framework/sdl_helper.h"
#include <list>
#include <memory>

class MenuScreen;

class MenuScreenManager
{
public:
    MenuScreenManager(SDL_Context* sdl_context);
    ~MenuScreenManager();

    template<class T, class... Args>
    inline void AddNewScreen(Args&&... args)
    {
        AddNewScreen(std::make_shared<T>(std::forward<Args>(args)...));
    }
    void AddNewScreen(const std::shared_ptr<MenuScreen> & screen);
    void RemoveCurrentScreen();
    void RemoveAllScreens();
    bool ScreenAvailable() const;
    void Update();
    void Run();
    void DisplayErrorScreen(const std::string & error, bool removeCurrentScreen = false);

    SDL_Renderer * GetRenderer() const;
    std::vector<GameControllerEvent> & GetControllerEvents();
    const std::vector<GameControllerPtr> & GetControllers() const;

private:
    void addRemoveController(SDL_Event & e);
    void updateControllers();

    SDL_Context & sdl_context;
    std::list<std::shared_ptr<MenuScreen>> screens;
    std::vector<GameControllerPtr> controllers;
    std::vector<GameControllerEvent> controllerEvents;

#ifdef SEGA
    void openPowerButtonEventFile();
    int powerFd;
#endif
};

#endif

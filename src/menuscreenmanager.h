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
    void AddNewScreen(const std::shared_ptr<MenuScreen> & screen);
    void RemoveCurrentScreen();
    void RemoveAllScreens();
    bool ScreenAvailable() const;
    void Update();
    void Run();
    void DisplayErrorScreen(const std::string & error, bool removeCurrentScreen = false);
    const Texture & GetBackground() const;
    SDL_Renderer * GetRenderer() const;
    std::vector<GameControllerEvent> & GetControllerEvents();

private:
    void addRemoveController(SDL_Event & e);
    void updateControllers();

    SDL_Context & sdl_context;
    std::list<std::shared_ptr<MenuScreen>> screens;
    std::vector<GameControllerPtr> controllers;
    std::vector<GameControllerEvent> controllerEvents;
    Texture background;
};

#endif

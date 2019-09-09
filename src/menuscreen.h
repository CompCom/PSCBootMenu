#ifndef MENUSCREEN_H_
#define MENUSCREEN_H_

#include "menuitem.h"
#include <list>
#include <vector>
#include <functional>

class MenuScreenManager;
union SDL_Event;
struct GameControllerEvent;

class MenuScreen
{
public:
    MenuScreen();
    virtual ~MenuScreen();
    virtual void Init() = 0;
    virtual bool HandleSDLEvent(SDL_Event * e);
    virtual void Render();
    virtual void Update();
    void AttachManager(MenuScreenManager * manager);

protected:
    virtual void handleButtonPress(const GameControllerEvent * event);

    SDL_Renderer * renderer;
    std::list<Texture> textures;
    std::vector<MenuItemPtr> items;
    unsigned int selectedItem;
    MenuScreenManager * manager;
};

#endif

#include "menuscreen.h"
#include "menuscreenmanager.h"
#include "gamecontroller.h"

MenuScreen::MenuScreen() : selectedItem(0)
{}

MenuScreen::~MenuScreen()
{
}

bool MenuScreen::HandleSDLEvent(SDL_Event * e)
{
    return false;
}

void MenuScreen::Render()
{
    for(Texture & texture : textures)
        texture.Draw(renderer);

    for(unsigned int i = 0; i < items.size(); ++i)
            items[i]->draw(renderer, selectedItem == i);
}

void MenuScreen::Update()
{
    auto & controllerEvents = manager->GetControllerEvents();
    for(const GameControllerEvent & event : controllerEvents)
    {
        if(event.state)
            handleButtonPress(&event);
    }
}

void MenuScreen::AttachManager(MenuScreenManager * manager)
{
    this->manager = manager;
    renderer = manager->GetRenderer();
}

void MenuScreen::handleButtonPress(const GameControllerEvent * event)
{
    int result = items[selectedItem]->handleButtonPress(event->button, event->state);
    if(result == -1 && selectedItem != 0)
        --selectedItem;
    else if(result == 1 && selectedItem != items.size()-1)
        ++selectedItem;
}

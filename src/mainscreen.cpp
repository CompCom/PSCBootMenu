#include "mainscreen.h"
#include "menuscreenmanager.h"
#include "settings.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

extern fs::path imageFolder;
extern fs::path themeFolder;

std::string GetTexturePath(const std::string & pngFileName)
{
    if(fs::exists(themeFolder/pngFileName))
    {
        return themeFolder/pngFileName;
    }
    else if(fs::exists(imageFolder/pngFileName))
    {
        return imageFolder/pngFileName;
    }
    return pngFileName;
}

MainScreen::MainScreen()
{

}
MainScreen::~MainScreen()
{
}

void MainScreen::Init()
{
    textures.emplace_back(GetTexturePath("/Background.png"), renderer);

    container = std::make_shared<ScrollingContainer>();
    container->x = 640;
    container->y = 360;
    container->offset = 430;
    items.push_back(container);

#ifdef __arm__
    for(auto & entry : fs::directory_iterator("/media/bleemsync/etc/boot_menu/boot_items"))
#else
    for(auto & entry : fs::directory_iterator("boot_items"))
#endif // __arm__
    {
        const auto & path = entry.path();
        try
        {
            if(fs::is_regular_file(path) && path.extension().compare(".json") == 0)
            {
                json j;
                std::ifstream in(path.string());
                if(in.is_open())
                {
                    in >> j;
                    in.close();
                    std::string texture = j["texture"], hoverTexture = j["hoverTexture"], launchCommand = j["launchCommand"];
                    ImagePushButtonPtr button = std::make_shared<ImagePushButton>(GetTexturePath(texture), GetTexturePath(hoverTexture), renderer);
                    button->onPress = std::bind(&MainScreen::selectBootItem, this, launchCommand);
                    container->AddItem(button);
                    auto isDefaultItem = j.find("default");
                    if(isDefaultItem != j.end() && *isDefaultItem)
                        container->SetCurrentItem(container->items.size()-1);
                }
            }
        }
        catch(...)
        {
            std::cerr << "Error parsing boot item json: " << path << std::endl;
        }
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
    container->Update();
}

void MainScreen::handleButtonPress(const GameControllerEvent * event)
{
    switch(event->button)
    {
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        if(event->state == 1)
        {
            manager->AddNewScreen(std::make_shared<SettingsScreen>());
            return;
        }
        break;
    default:
        container->handleButtonPress(event->button, event->state);
        break;
    }
}

void MainScreen::selectBootItem(std::string launchCommand)
{
    std::ofstream out("/tmp/launchfilecommand");
    if(out.is_open())
    {
        out << launchCommand;
        out.close();
        manager->RemoveAllScreens();
    }
    else
    {
        manager->DisplayErrorScreen("Cannot open launch command file.", true);
    }
}

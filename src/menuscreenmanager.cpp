#include "menuscreenmanager.h"
#include "menuscreen.h"
#include "warningscreen.h"

#include <iostream>
#include <algorithm>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

MenuScreenManager::MenuScreenManager(SDL_Context * sdl_context) : sdl_context(*sdl_context)
{
    //Load SDL Game Controller Mappings
#ifdef __arm__
    std::string game_mapping_file("/media/bleemsync/etc/boot_menu/gamecontrollerdb.txt");
#else
    std::string game_mapping_file("gamecontrollerdb.txt");
#endif // __arm__
    if(fs::exists(game_mapping_file))
        SDL_GameControllerAddMappingsFromFile(game_mapping_file.c_str());

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if(SDL_IsGameController(i))
            controllers.emplace_back(std::make_unique<GameController>(i, controllerEvents));
    }

    //Clear SDL Events
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

    //Load Background image
#ifdef __arm__
    background = Texture("/usr/sony/share/data/images/GR/JP_US_BG.png", sdl_context->renderer);
#else
    background = Texture("./root/usr/sony/share/data/images/GR/JP_US_BG.png", sdl_context->renderer);
#endif // __arm__
}
MenuScreenManager::~MenuScreenManager()
{}

void MenuScreenManager::AddNewScreen(const std::shared_ptr<MenuScreen> & screen)
{
    controllerEvents.clear();
    screens.push_back(screen);
    screen->AttachManager(this);
    screen->Init();
}

void MenuScreenManager::RemoveCurrentScreen()
{
    if(screens.size())
        screens.erase(--screens.end());
    controllerEvents.clear();
}

void MenuScreenManager::RemoveAllScreens()
{
    screens.clear();
}

bool MenuScreenManager::ScreenAvailable() const
{
    return screens.size();
}

void MenuScreenManager::Update()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if(screens.size() && screens.back()->HandleSDLEvent(&e))
            continue;
        switch(e.type)
        {
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            addRemoveController(e);
            break;
        case SDL_KEYDOWN:
            {
                switch(e.key.keysym.scancode)
                {
                case SDL_SCANCODE_AUDIOPLAY:
                case SDL_SCANCODE_SLEEP:
                    screens.clear();
                    return;
                default:
                    break;
                }
            }
            break;
        case SDL_QUIT:
            screens.clear();
            return;
        }
    }
    updateControllers();
}

void MenuScreenManager::Run()
{
    std::shared_ptr<MenuScreen> currentScreen = screens.back();
    Update();
    currentScreen->Update();
    sdl_context.StartFrame();
    currentScreen->Render();
    sdl_context.EndFrame();
    controllerEvents.clear();
}

void MenuScreenManager::DisplayErrorScreen(const std::string & error, bool removeCurrentScreen)
{
    if(removeCurrentScreen)
        RemoveCurrentScreen();

    PushButtonPtr okayButton = std::make_shared<PushButton>("OK", 28, sdl_context.renderer, 640, 560);
    okayButton->onPress = std::bind(&MenuScreenManager::RemoveCurrentScreen, this);
    AddNewScreen(std::make_shared<WarningScreen>(error, okayButton, true));
}

void MenuScreenManager::addRemoveController(SDL_Event & e)
{
    switch(e.type)
    {
    case SDL_CONTROLLERDEVICEADDED:
        {
            controllers.emplace_back(std::make_unique<GameController>(e.cdevice.which, controllerEvents));
            std::cout << "Added device id: " << e.cdevice.which << std::endl;
        }
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        {
            auto controllerPtr = SDL_GameControllerFromInstanceID(e.cdevice.which);
            controllers.erase(std::remove_if(controllers.begin(), controllers.end(), [&](const GameControllerPtr & c) { return c->GetController() == controllerPtr; }), controllers.end());
        }
        break;
    }
}

void MenuScreenManager::updateControllers()
{
    for(auto & controller : controllers)
        controller->Update();
}

const Texture & MenuScreenManager::GetBackground() const
{
    return background;
}

SDL_Renderer * MenuScreenManager::GetRenderer() const
{
    return sdl_context.renderer;
}

std::vector<GameControllerEvent> & MenuScreenManager::GetControllerEvents()
{
    return controllerEvents;
}

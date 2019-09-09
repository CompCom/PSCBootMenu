#ifndef MAINSCREEN_H_
#define MAINSCREEN_H_

#include "menuscreen.h"
#include <map>

class ScrollingContainer;

class MainScreen : public MenuScreen
{
public:
    MainScreen();
    ~MainScreen();
    void Init() override;
    bool HandleSDLEvent(SDL_Event * e) override;
    void Update() override;

private:
    void handleButtonPress(const GameControllerEvent * event) override;
    void selectBootItem(std::string launchCommand);
    std::shared_ptr<ScrollingContainer> container;
};

#endif

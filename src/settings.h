#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <vector>
#include "gamecontroller.h"

class SDL_Context;
extern std::string cfgLocation;
extern std::string defaultCfgLocation;
extern std::string uiThemesFolder;

void RunSettingsMenu(SDL_Context* sdl_context, std::vector<GameControllerPtr> & controllers);

#endif

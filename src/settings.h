#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "menuscreen.h"
#include "menuitem.h"
#include <map>

class SDL_Context;
extern std::string cfgLocation;
extern std::string defaultCfgLocation;
extern std::string uiThemesFolder;

class SettingsScreen : public MenuScreen
{
public:
    SettingsScreen();
    ~SettingsScreen();
    virtual void Init();

private:
    bool loadVariableTypes();
    void saveConfig();

    std::map<std::string, std::string> variableTypeMap;
    std::map<std::string, BoolTogglePtr> bool_variables;
    std::map<std::string, std::string> unk_variables;
    ThemeTogglePtr theme_variable;
    std::list<std::string> lines;
};

#endif

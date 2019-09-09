#ifndef WARNINGSCREEN_H_
#define WARNINGSCREEN_H_

#include "menuscreen.h"

class WarningScreen : public MenuScreen
{
public:
    WarningScreen(const std::string & warningText, const MenuItemPtr & item, bool isError = false);
    ~WarningScreen();
    void Init() override;

private:
    std::string warningText;
    bool isError;
};

#endif

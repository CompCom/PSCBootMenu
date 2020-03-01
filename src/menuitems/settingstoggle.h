#ifndef SETTINGSTOGGLE_H_
#define SETTINGSTOGGLE_H_

#include <string>

struct SettingsToggle
{
    virtual std::string GetConfigValue() const = 0;
    virtual void SetValueFromConfig(const std::string & value) = 0;
};

#endif
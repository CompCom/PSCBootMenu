#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "framework/texture.h"

#include <memory>
#include <functional>
#include <vector>

class SDL_Context;

extern const std::string fontBold;
extern const std::string font;
extern Texture trueTexture;
extern Texture falseTexture;

struct MenuItem
{
    virtual int handleButtonPress(uint8_t gamebutton, uint8_t state) = 0;
    virtual void setPosition(int x, int y);
    virtual void draw(SDL_Renderer* renderer, bool active) = 0;
};

using MenuItemPtr = std::shared_ptr<MenuItem>;

struct ItemCollection : MenuItem
{
    int x = 50, y = 120;
    std::vector<MenuItemPtr> items;
    unsigned int currentItem = 0;
    void addItem(MenuItemPtr item);
    void draw(SDL_Renderer* renderer, bool active);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using ItemCollectionPtr = std::shared_ptr<ItemCollection>;

struct HItemCollection : ItemCollection
{
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using HItemCollectionPtr = std::shared_ptr<HItemCollection>;

struct BoolToggle : MenuItem
{
    BoolToggle(SDL_Context* sdl_context, const std::string & variable_name, bool value);
    Texture nameTexture;
    bool value;
    void draw(SDL_Renderer* renderer, bool active);
    void setPosition(int x, int y);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using BoolTogglePtr = std::shared_ptr<BoolToggle>;

struct PushButton : MenuItem
{
    PushButton(const std::string & buttonText, int fontSize, SDL_Context* sdl_context, int x, int y);
    Texture buttonTexture;
    std::function<void()> onPress;
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void draw(SDL_Renderer* renderer, bool active);
};
using PushButtonPtr = std::shared_ptr<PushButton>;

struct ThemeToggle : MenuItem
{
    ThemeToggle(SDL_Context* sdl_context, const std::string & variable_name, const std::string & currentValue);
    Texture nameTexture;
    Texture valueTexture;
    unsigned int selectedValue;
    std::vector<std::string> values;
    SDL_Context* sdl_context;
    void setSelectedValue(std::string currentValue);
    void draw(SDL_Renderer* renderer, bool active);
    void setPosition(int x, int y);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using ThemeTogglePtr = std::shared_ptr<ThemeToggle>;

#endif

#include "menuitem.h"
#include "settings.h"
#include "framework/sdl_context.h"
#include <algorithm>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#ifdef __arm__
    const std::string fontBold = "/usr/share/fonts/ttf/LiberationMono-Bold.ttf";
    const std::string font = "/usr/share/fonts/ttf/LiberationMono-Regular.ttf";
#else
    const std::string fontBold = "./root/usr/share/fonts/ttf/LiberationMono-Bold.ttf";
    const std::string font = "./root/usr/share/fonts/ttf/LiberationMono-Regular.ttf";
#endif // __arm__

Texture trueTexture;
Texture falseTexture;

void MenuItem::setPosition(int x, int y) {}

void ItemCollection::addItem(MenuItemPtr item)
{
    item->setPosition(x+10,y+20*items.size());
    items.push_back(item);
}
void ItemCollection::draw(SDL_Renderer* renderer, bool active)
{
    for(unsigned int i = 0, count = items.size(); i < count; ++i)
        items[i]->draw(renderer, active && i == currentItem);
}
int ItemCollection::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    int result = items[currentItem]->handleButtonPress(gamebutton, state);
    if(result == -1)
    {
        if(currentItem == 0)
            return -1;
        currentItem = currentItem-1;
    }
    else if(result == 1)
    {
        if(currentItem == items.size()-1)
            return 1;
        currentItem = currentItem+1;
    }
    return 0;
}
int HItemCollection::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            currentItem = 0;
            return -1;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            currentItem = 0;
            return 1;
        default:
            currentItem = (currentItem + items[currentItem]->handleButtonPress(gamebutton, state) + items.size()) % items.size();
            break;
    }
    return 0;
}

BoolToggle::BoolToggle(SDL_Context* sdl_context, const std::string & variable_name, bool value) : value(value)
{
    nameTexture = Texture(fontBold, variable_name, 16, sdl_context->renderer);
}
void BoolToggle::draw(SDL_Renderer* renderer, bool active)
{
    nameTexture.Draw(renderer);
    Texture valText = value ? trueTexture : falseTexture;
    valText.rect.x = nameTexture.rect.x + nameTexture.rect.w + 20;
    valText.rect.y = nameTexture.rect.y;
    if(active)
        SDL_SetTextureColorMod(valText.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(valText.texture.get(), 255, 255, 255);
    valText.Draw(renderer);
}
void BoolToggle::setPosition(int x, int y)
{
    nameTexture.rect.x = x;
    nameTexture.rect.y = y;
}
int BoolToggle::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        value = !value;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return 1;
    }
    return 0;
}

PushButton::PushButton(const std::string & buttonText, int fontSize, SDL_Context* sdl_context, int x, int y)
{
    buttonTexture = Texture(fontBold, buttonText, fontSize, sdl_context->renderer, x, y, true);
}
int PushButton::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_A:
        if(state != 2)
            onPress();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return 1;
    }
    return 0;
}
void PushButton::draw(SDL_Renderer* renderer, bool active)
{
    if(active)
        SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 255);
    buttonTexture.Draw(renderer);
}

ThemeToggle::ThemeToggle(SDL_Context* sdl_context, const std::string & variable_name, const std::string & currentValue) : sdl_context(sdl_context)
{
    nameTexture = Texture(fontBold, variable_name, 16, sdl_context->renderer);
    values.push_back("stock");
    for(auto & item : fs::directory_iterator(uiThemesFolder))
    {
        if(fs::is_directory(item.path()))
        {
            values.push_back(item.path().filename().string());
        }
    }
    setSelectedValue(currentValue);
}
void ThemeToggle::setSelectedValue(std::string currentValue)
{
    currentValue.erase(std::remove_if(currentValue.begin(), currentValue.end(), [](const char & c){ return c == '"';}), currentValue.end());
    auto iter = std::find(values.begin(), values.end(), currentValue);
    if(iter != values.end())
        selectedValue = std::distance(values.begin(), iter);
    else
        selectedValue = 0;
    valueTexture = Texture(font, values[selectedValue], 16, sdl_context->renderer);
}
void ThemeToggle::draw(SDL_Renderer* renderer, bool active)
{
    nameTexture.Draw(renderer);
    if(active)
        SDL_SetTextureColorMod(valueTexture.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(valueTexture.texture.get(), 255, 255, 255);
    valueTexture.Draw(renderer);
}
void ThemeToggle::setPosition(int x, int y)
{
    nameTexture.rect.x = x;
    nameTexture.rect.y = y;
    valueTexture.rect.x = nameTexture.rect.x + nameTexture.rect.w + 20;
    valueTexture.rect.y = y;
}
int ThemeToggle::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        selectedValue = (selectedValue - 1 + values.size()) % values.size();
        valueTexture = Texture(font, values[selectedValue], 16, sdl_context->renderer, valueTexture.rect.x, valueTexture.rect.y);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        selectedValue = (selectedValue + 1) % values.size();
        valueTexture = Texture(font, values[selectedValue], 16, sdl_context->renderer, valueTexture.rect.x, valueTexture.rect.y);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return 1;
    }
    return 0;
}

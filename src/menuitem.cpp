#include "menuitem.h"
#include "framework/sdl_helper.h"
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

TextEditButton::TextEditButton(const std::string & label, std::string * text, SDL_Renderer * renderer) : renderer(renderer), text(text)
{
    labelTexture = Texture(fontBold, label, 16, renderer);
    updateTextTexture();
    textBGTexture = Texture(renderer, 0, 0, 300, textTexture.rect.h, ~0, false);
}
int TextEditButton::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    if(state == 0)
        return 0;
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_A:
        onPress();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return 1;
    }
    return 0;
}

void TextEditButton::setPosition(int x, int y)
{
    labelTexture.rect.x = x;
    labelTexture.rect.y = y;
    textTexture.rect.x = labelTexture.rect.x + labelTexture.rect.w + 10;
    textTexture.rect.y = y;
    textBGTexture.rect.x = textTexture.rect.x-4;
    textBGTexture.rect.y = y;
}
void TextEditButton::draw(SDL_Renderer* renderer, bool active)
{
    if(active)
        SDL_SetTextureColorMod(textBGTexture.texture.get(), 255, 200, 0);
    else
        SDL_SetTextureColorMod(textBGTexture.texture.get(), 255, 255, 255);
    textBGTexture.Draw(renderer);
    labelTexture.Draw(renderer);
    textTexture.Draw(renderer);
}

void TextEditButton::updateTextTexture()
{
    std::string displayText;
    if(text->size() == 0)
        displayText = " ";
    else if(text->size() > 29)
        displayText = text->substr(0,26) + "...";
    else
        displayText = *text;
    textTexture = Texture(font, displayText, 16, renderer, labelTexture.rect.x + labelTexture.rect.w + 10, labelTexture.rect.y,false,0,0,0);
}

GameToggle::GameToggle(const GameData & gameData, bool selected, SDL_Renderer * renderer)
            : gameData(gameData), selected(selected), renderer(renderer)
{}
int GameToggle::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    if(gamebutton == SDL_CONTROLLER_BUTTON_A && state)
        selected = !selected;
    return 0;
}
void GameToggle::setPosition(int x, int y)
{
    icon.rect.x = x;
    icon.rect.y = y+20;
    gameLabel.rect.x = x+icon.rect.w/2 - gameLabel.rect.w/2;
    gameLabel.rect.y = y+icon.rect.h+24;
}
void GameToggle::setPosition(int x, int y, int minX, int maxX)
{
    setPosition(x, y);
    if(gameLabel.rect.x < minX)
        gameLabel.rect.x = minX;
    else if(maxX+icon.rect.w < gameLabel.rect.x + gameLabel.rect.w)
        gameLabel.rect.x = maxX+icon.rect.w - gameLabel.rect.w;
}
void GameToggle::draw(SDL_Renderer* renderer, bool active)
{
    static Texture enabled = Texture(font, "ENABLED", 16, renderer,0,0,false,0,0xFF,0);
    static Texture disabled = Texture(font, "DISABLED", 16, renderer);
    Texture & status = (selected) ? enabled : disabled;
    status.rect.x = icon.rect.x+icon.rect.w/2-status.rect.w/2;
    status.rect.y = icon.rect.y-20;
    status.Draw(renderer);
    icon.Draw(renderer);
    if(active)
        gameLabel.Draw(renderer);
}
void GameToggle::activate()
{
    icon = Texture(gameData.imagePath, renderer);
    icon.rect.h = 113;
    icon.rect.w = 113;
    gameLabel = Texture(font, gameData.gameName, 16, renderer);
}
void GameToggle::deactivate()
{
    icon.texture.reset();
    gameLabel.texture.reset();
}

ImagePushButton::ImagePushButton(const std::string & imagePath, const std::string & activeImagePath, SDL_Renderer * renderer) : renderer(renderer)
{
    buttonTexture = Texture(imagePath, renderer);
    activeTexture = Texture(activeImagePath, renderer);
}
void ImagePushButton::setPosition(int x, int y)
{
    buttonTexture.rect.x = x - buttonTexture.rect.w/2;
    buttonTexture.rect.y = y - buttonTexture.rect.h/2;
    activeTexture.rect = buttonTexture.rect;
}

void ImagePushButton::draw(SDL_Renderer* renderer, bool active)
{
    (active) ? activeTexture.Draw(renderer) : buttonTexture.Draw(renderer);
}

void MenuItem::setPosition(int x, int y) {}

void ItemCollection::addItem(MenuItemPtr item)
{
    if(moveItem)
        item->setPosition(x+xOffset,y+yOffset*items.size());
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
            if(items.size())
                currentItem = (currentItem + items[currentItem]->handleButtonPress(gamebutton, state) + items.size()) % items.size();
            break;
    }
    return 0;
}

int VItemCollection::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            return -1;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            return 1;
        default:
            if(items.size())
                currentItem = (currentItem + items[currentItem]->handleButtonPress(gamebutton, state) + items.size()) % items.size();
            break;
    }
    return 0;
}

BoolToggle::BoolToggle(const std::string & variable_name, bool value, SDL_Renderer * renderer) : value(value)
{
    nameTexture = Texture(fontBold, variable_name, 16, renderer);
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

PushButton::PushButton()
{}
PushButton::PushButton(const std::string & buttonText, int fontSize, SDL_Renderer * renderer, int x, int y, bool center)
{
    buttonTexture = Texture(fontBold, buttonText, fontSize, renderer, x, y, center);
}
int PushButton::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_A:
        if(state == 1)
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
void PushButton::setPosition(int x, int y)
{
    buttonTexture.rect.x = x;
    buttonTexture.rect.y = y;
}
void PushButton::draw(SDL_Renderer* renderer, bool active)
{
    if(active)
        SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(buttonTexture.texture.get(), 255, 255, 255);
    buttonTexture.Draw(renderer);
}

extern std::string uiThemesFolder;

ThemeToggle::ThemeToggle(SDL_Renderer * renderer, const std::string & variable_name, const std::string & currentValue) : renderer(renderer)
{
    nameTexture = Texture(fontBold, variable_name, 16, renderer);
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
    valueTexture = Texture(font, values[selectedValue], 16, renderer);
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
        valueTexture = Texture(font, values[selectedValue], 16, renderer, valueTexture.rect.x, valueTexture.rect.y);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        selectedValue = (selectedValue + 1) % values.size();
        valueTexture = Texture(font, values[selectedValue], 16, renderer, valueTexture.rect.x, valueTexture.rect.y);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return 1;
    }
    return 0;
}

RangeScroll::RangeScroll(int & currentValue, int min, int max, SDL_Renderer * renderer) : min(min), max(max), value(currentValue), renderer(renderer)
{
    upArrow = Texture(font, u8"\u25B2", 50, renderer);
    downArrow = upArrow;
}

void RangeScroll::createValueTexture()
{
    valueTexture = Texture(font, std::to_string(value), 50, renderer, x, y, true);
}

void RangeScroll::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
    createValueTexture();
    upArrow.rect.x = x - upArrow.rect.w/2;
    upArrow.rect.y = y - valueTexture.rect.h - 8;
    downArrow.rect.x = upArrow.rect.x;
    downArrow.rect.y = y + 6;
}

int RangeScroll::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        if(++value > max)
            value = min;
        createValueTexture();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return -1;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        if(--value < min)
            value = max;
        createValueTexture();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return 1;
    }
    return 0;
}

void RangeScroll::draw(SDL_Renderer* renderer, bool active)
{
    upArrow.Draw(renderer);
    if(active)
        SDL_SetTextureColorMod(valueTexture.texture.get(), 255, 255, 0);
    else
        SDL_SetTextureColorMod(valueTexture.texture.get(), 255, 255, 255);
    valueTexture.Draw(renderer);
    downArrow.Draw(renderer, SDL_RendererFlip::SDL_FLIP_VERTICAL);
}

void ScrollingContainer::AddItem(const ImagePushButtonPtr & item)
{
    item->setPosition(x + (items.size()-currentItem)*offset, y);
    items.push_back(item);
}
void ScrollingContainer::Update()
{
    if(scrolling == 0 && requestedScroll != 0)
    {
        if(requestedScroll & LEFT)
            scrolling = (currentItem > 0) ? -1 : 0;
        else if(requestedScroll & RIGHT)
            scrolling = (currentItem+1 < items.size()) ? 1 : 0;
    }
    if(scrolling != 0)
    {
        if(frameCount < maxFrameCount)
            ++frameCount;
        else
        {
            currentItem += scrolling;
            scrolling = 0;
            frameCount = 0;
        }
        setPositions();
    }
}
void ScrollingContainer::SetCurrentItem(int id)
{
    if(id >= 0 && id < items.size())
    {
        currentItem = id;
        setPositions();
    }
}
void ScrollingContainer::setPositions()
{
    int moveOffset = (scrolling == 0) ? 0 : frameCount/(float)maxFrameCount * offset * scrolling;
    for(int i = 0, count = items.size(); i < count; ++i)
        items[i]->setPosition(x + (i-currentItem)*offset - moveOffset, y);
}
int ScrollingContainer::handleButtonPress(uint8_t gamebutton, uint8_t state)
{
    switch(gamebutton)
    {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            return -1;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            return 1;
        default:
            int result = items[currentItem]->handleButtonPress(gamebutton, state);
            if(state > 0)
            {
                if(result == -1)
                    requestedScroll |= LEFT;
                else if(result == 1)
                    requestedScroll |= RIGHT;
            }
            else
            {
                if(result == -1)
                    requestedScroll &= ~LEFT;
                else if(result == 1)
                    requestedScroll &= ~RIGHT;
            }
            break;
    }
    return 0;
}
void ScrollingContainer::draw(SDL_Renderer* renderer, bool active)
{
    for(unsigned int i = 0, count = items.size(); i < count; ++i)
        items[i]->draw(renderer, (scrolling) ? false : active && i == currentItem);
}

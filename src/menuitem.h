#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "framework/texture.h"

#include <memory>
#include <functional>
#include <vector>
#include <limits>

class SDL_Context;
class SDL_Renderer;
class Manager;

extern const std::string fontBold;
extern const std::string font;
extern Texture trueTexture;
extern Texture falseTexture;

struct GameData
{
    int gameId, nodeId, type;
    std::string gameName, imagePath;
};

struct MenuItem
{
    virtual int handleButtonPress(uint8_t gamebutton, uint8_t state) = 0;
    virtual void setPosition(int x, int y);
    virtual void draw(SDL_Renderer* renderer, bool active) = 0;
};

using MenuItemPtr = std::shared_ptr<MenuItem>;

struct TextEditButton : MenuItem
{
    TextEditButton(const std::string & label, std::string * text, SDL_Renderer * renderer);
    Texture textTexture, labelTexture, textBGTexture;
    SDL_Renderer * renderer;
    std::function<void()> onPress;
    std::string * text;
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void setPosition(int x, int y);
    void draw(SDL_Renderer* renderer, bool active);
    void updateTextTexture();
};
using TextEditButtonPtr = std::shared_ptr<TextEditButton>;

struct GameToggle : MenuItem
{
    GameToggle(const GameData & gameData, bool selected, SDL_Renderer * renderer);
    const GameData & gameData;
    Texture icon, gameLabel;
    bool selected;
    SDL_Renderer* renderer;
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void setPosition(int x, int y);
    void setPosition(int x, int y, int minX, int maxX);
    void draw(SDL_Renderer* renderer, bool active);
    void activate();
    void deactivate();
};
using GameTogglePtr = std::shared_ptr<GameToggle>;

template <typename T>
struct ImageButtonCollection : MenuItem
{
    ImageButtonCollection();
    std::vector<T> gameButtons;
    unsigned int selectedGame = std::numeric_limits<unsigned int>::max(), topGameId = selectedGame;
    const unsigned int maxColCount = 6, maxRowCount = 3;
    int xOffset = 90, xSpacing = 170, yOffset = 114, ySpacing = 170;
    void AddGame(const T & gameButton);
    void SetSelectedGame(int id);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void draw(SDL_Renderer* renderer, bool active);
};
//using ImageButtonCollectionPtr = std::shared_ptr<ImageButtonCollection>;

struct ItemCollection : MenuItem
{
    bool moveItem = true;
    int x = 50, y = 120, xOffset = 10, yOffset = 20;
    std::vector<MenuItemPtr> items;
    int currentItem = 0;
    virtual void addItem(MenuItemPtr item);
    void draw(SDL_Renderer* renderer, bool active);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using ItemCollectionPtr = std::shared_ptr<ItemCollection>;

struct HItemCollection : ItemCollection
{
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using HItemCollectionPtr = std::shared_ptr<HItemCollection>;

struct VItemCollection : ItemCollection
{
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using VItemCollectionPtr = std::shared_ptr<VItemCollection>;

struct BoolToggle : MenuItem
{
    BoolToggle(const std::string & variable_name, bool value, SDL_Renderer * renderer);
    Texture nameTexture;
    bool value;
    void draw(SDL_Renderer* renderer, bool active);
    void setPosition(int x, int y);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
};
using BoolTogglePtr = std::shared_ptr<BoolToggle>;

struct PushButton : MenuItem
{
    PushButton();
    PushButton(const std::string & buttonText, int fontSize, SDL_Renderer * renderer, int x, int y, bool center = true);
    Texture buttonTexture;
    std::function<void()> onPress;
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void setPosition(int x, int y) override;
    void draw(SDL_Renderer* renderer, bool active);
};
using PushButtonPtr = std::shared_ptr<PushButton>;

struct ImagePushButton : PushButton
{
    ImagePushButton(const std::string & imagePath, const std::string & activeImagePath, SDL_Renderer * renderer);
    Texture activeTexture;
    SDL_Renderer * renderer;
    void setPosition(int x, int y);
    void draw(SDL_Renderer* renderer, bool active);
};
using ImagePushButtonPtr = std::shared_ptr<ImagePushButton>;

struct ThemeToggle : MenuItem
{
    ThemeToggle(SDL_Renderer * renderer, const std::string & variable_name, const std::string & currentValue);
    Texture nameTexture;
    Texture valueTexture;
    unsigned int selectedValue;
    std::vector<std::string> values;
    SDL_Renderer * renderer;
    void setSelectedValue(std::string currentValue);
    void draw(SDL_Renderer* renderer, bool active);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void setPosition(int x, int y);
};
using ThemeTogglePtr = std::shared_ptr<ThemeToggle>;

struct RangeScroll : MenuItem
{
    RangeScroll(int & currentValue, int min, int max, SDL_Renderer * renderer);
    int min, max, x, y;
    int & value;
    SDL_Renderer * renderer;
    Texture valueTexture, upArrow, downArrow;
    void createValueTexture();
    void setPosition(int x, int y);
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void draw(SDL_Renderer* renderer, bool active);
};
using RangeScrollPtr = std::shared_ptr<RangeScroll>;

struct ScrollingContainer : MenuItem
{
    enum ScrollDirection { LEFT = 1, RIGHT = 1 << 1};
    int x, y, offset;
    int currentItem = 0;
    int scrolling = 0;
    int requestedScroll = 0;
    int frameCount = 0, maxFrameCount = 20;
    std::vector<ImagePushButtonPtr> items;
    void AddItem(const ImagePushButtonPtr & item);
    void SetCurrentItem(int id);
    void Update();
    void setPositions();
    int handleButtonPress(uint8_t gamebutton, uint8_t state);
    void draw(SDL_Renderer* renderer, bool active);
};
using ScrollingContainerPtr = std::shared_ptr<ScrollingContainer>;
#endif

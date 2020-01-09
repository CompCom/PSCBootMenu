/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "edittextscreen.h"
#include "font.h"
#include "gamebutton.h"
#include "guihelper.h"
#include "menuitems/keyboardbutton.h"
#include "menuscreenmanager.h"
#include <algorithm>

#ifdef SEGA
constexpr GameButton CapsBtn = GameButtons[GAME_BUTTON_SEGA_C], BackSpaceBtn = GameButtons[GAME_BUTTON_SEGA_Y], InsertBtn = GameButtons[GAME_BUTTON_SEGA_A], CancelBtn = GameButtons[GAME_BUTTON_SEGA_B], SpaceBtn = GameButtons[GAME_BUTTON_SEGA_X], ConfirmBtn = GameButtons[GAME_BUTTON_SEGA_START];
#else
constexpr GameButton CapsBtn = GameButtons[GAME_BUTTON_L1], BackSpaceBtn = GameButtons[GAME_BUTTON_SQUARE], InsertBtn = GameButtons[GAME_BUTTON_X], CancelBtn = GameButtons[GAME_BUTTON_O], SpaceBtn = GameButtons[GAME_BUTTON_TRIANGLE], ConfirmBtn = GameButtons[GAME_BUTTON_START];
#endif // SEGA

constexpr int maxTextLength = 63;

EditTextScreen::EditTextScreen(const std::string & title, std::string * originalText, const std::function<void(std::string)> & saveCallback) : originalText(originalText), saveCallback(saveCallback), shift(false), title(title)
{
    if(originalText)
        text = *originalText;
}

EditTextScreen::~EditTextScreen()
{

}

void EditTextScreen::Init()
{
    GuiHelper & guiHelper = GuiHelper::GetInstance();
    textures.push_back(guiHelper.GetTexture(GuiElement::BACKGROUND));
    textures.push_back(guiHelper.CreateTitleTexture(title, 640, 72));
    textures.push_back(guiHelper.GetTexture(GuiElement::FOOTER));
    guiHelper.CreateFooterItems({
                                    {CapsBtn.GUI_Value, "Caps Lock"},
                                    {BackSpaceBtn.GUI_Value, "Back Space"},
                                    {InsertBtn.GUI_Value, "Insert"},
                                    {CancelBtn.GUI_Value, "Cancel"},
                                    {SpaceBtn.GUI_Value, "Space"},
                                    {ConfirmBtn.GUI_Value, "Confirm"}
                                },
                                textures);
    textures.insert(textures.end(), guiHelper.GetTexture(GuiElement::LINE_BREAK))->SetPosition(640, 590, true);

    textures.insert(textures.end(), guiHelper.GetTexture("Input/Input_TextBox.png"))->SetPosition(640, 195, true);
    textures.insert(textures.end(), guiHelper.GetTexture("Input/Input_Keyboard_BG.png"))->SetPosition(640, 400, true);

    dotMatrix = guiHelper.GetTexture("Input/Input_Keyboard_DotMatrix.png");
    dotMatrix.SetPosition(640, 400, true);
    keyHighlightTexture = guiHelper.GetTexture("Input/Input_Keyboard_Selection.png");

    recreateTexture();

    for(auto & collection : collections)
    {
        collection = std::make_shared<ItemCollection>();
        collection->collectionType = ItemCollection::HORIZONTAL;
        collection->moveItem = false;
        items.push_back(collection);
    }

#ifdef SEGA
    auto collection = std::make_shared<ItemCollection>();
    collection->collectionType = ItemCollection::HORIZONTAL;
    collection->moveItem = false;
    items.push_back(collection);
    auto backspaceGUIButton = std::make_shared<PushButton>("Backspace", 20, 590, 541);
    auto spaceGUIButton = std::make_shared<PushButton>("Space", 20, 690, 541);
    backspaceGUIButton->onPress = std::bind(&EditTextScreen::backspace, this);
    spaceGUIButton->onPress = std::bind(&EditTextScreen::space, this);
    collection->addItem(backspaceGUIButton);
    collection->addItem(spaceGUIButton);
#endif

    createKeys();
}

void EditTextScreen::Update()
{
    auto & controllerEvents = manager->GetControllerEvents();
    for(unsigned int i = 0; i < controllerEvents.size(); ++i)
    {
        const auto & event = controllerEvents[i];
        if(event.state == 0)
            continue;

        switch(event.button)
        {
        case CapsBtn.SDL_Value:
            if(event.state == 1)
            {
                shift = !shift;
                createKeys();
            }
            break;
        case BackSpaceBtn.SDL_Value:
            backspace();
            break;
        case CancelBtn.SDL_Value:
            if(event.state == 1)
            {
                manager->RemoveCurrentScreen();
                return;
            }
            break;
        case SpaceBtn.SDL_Value:
            space();
            break;
        case ConfirmBtn.SDL_Value:
            manager->RemoveCurrentScreen();
            if(saveCallback)
                saveCallback(text);
            return;
        default:
            {
                const int selectedKey = (selectedItem < lineCount) ? collections[selectedItem]->currentItem : 0;
                if(int result = items[selectedItem]->handleButtonPress(event.button, event.state))
                {
                    selectedItem = (selectedItem + result + items.size()) % items.size();
                    if(selectedItem < lineCount)
                        collections[selectedItem]->currentItem = std::min(selectedKey,(int)collections[selectedItem]->items.size()-1);
                }
            }
            break;
        }
    }
}

void EditTextScreen::Render()
{
    MenuScreen::Render();
    textTexture.Draw(renderer);
    dotMatrix.Draw(renderer);
}

void EditTextScreen::backspace()
{
    if(text.size())
    {
        do
        {
            text.erase(text.end()-1);
        }
        while(text.size() && (unsigned char)text.back() >= 0x80);
        recreateTexture();
    }
}

void EditTextScreen::createKeys()
{
    constexpr char lowerCaseLines[lineCount][10] =  {
                                                        {'1','2','3','4','5','6','7','8','9','0'},
                                                        {'q','w','e','r','t','y','u','i','o','p'},
                                                        {'a','s','d','f','g','h','j','k','l','`'},
                                                        {'z','x','c','v','b','n','m','<','>','?'},
                                                        {'-','=','[',']','\\',';','\'',',','.','/'}
                                                    };

    constexpr char upperCaseLines[lineCount][10] =  {
                                                        {'!','@','#','$','%','^','&','*','(',')'},
                                                        {'Q','W','E','R','T','Y','U','I','O','P'},
                                                        {'A','S','D','F','G','H','J','K','L','~'},
                                                        {'Z','X','C','V','B','N','M','<','>','?'},
                                                        {'_','+','{','{','|',':','"',',','.','/'}
                                                    };

    auto & lines = (shift) ? upperCaseLines : lowerCaseLines;

    for(int lineNo = 0; lineNo < lineCount; ++lineNo)
    {
        auto & line = lines[lineNo];
        auto & collection = collections[lineNo];
        collection->items.clear();
        int i = 0;
        int y = 306+lineNo*47;
        for(auto c : line)
        {
            KeyboardButtonPtr button = std::make_shared<KeyboardButton>(std::string(1, c), 20, 32, keyHighlightTexture, renderer, 428 + i++ * 47, y);
            collection->addItem(button);
            button->onPress = [&,c]()
            {
                if(getCharacterCount() < maxTextLength)
                {
                    text += c;
                    recreateTexture();
                }
            };
        }
    }
}

int EditTextScreen::getCharacterCount()
{
    int count = 0;
    for(const unsigned char & c : text)
    {
        if(c < 0x80)
            ++count;
    }
    return count;
}

void EditTextScreen::recreateTexture()
{
    std::string displayText = text;
    if(getCharacterCount() < maxTextLength)
        displayText += "_";
    textTexture = FontManager::CreateTexture(KEYBOARD_TEXT, displayText, 26, 640, 195, true);
}

void EditTextScreen::space()
{
    if(getCharacterCount() < maxTextLength)
    {
        text += ' ';
        recreateTexture();
    }
}

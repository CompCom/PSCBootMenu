/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */

#ifndef EDITTEXTSCREEN_H_
#define EDITTEXTSCREEN_H_

#include "menuitems/itemcollection.h"
#include "menuscreen.h"

class EditTextScreen : public MenuScreen
{
public:
    EditTextScreen(const std::string & title, std::string * originalText, const std::function<void(std::string)> & saveCallback);
    ~EditTextScreen();
    virtual void Init();
    virtual void Update();
    virtual void Render();

private:
    void backspace();
    void createKeys();
    int getCharacterCount();
    void recreateTexture();
    void space();

    std::string * originalText;
    std::function<void(std::string)> saveCallback;
    bool shift;
    std::string title;
    std::string text;
    Texture dotMatrix, keyHighlightTexture, textTexture;

    static const int lineCount = 5;
    ItemCollectionPtr collections[lineCount];
};

#endif

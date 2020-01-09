/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef GUIHELPER_H_
#define GUIHELPER_H_

#include "font.h"
#include "framework/sdl_helper.h"
#include <experimental/filesystem>
#include <list>

namespace fs = std::experimental::filesystem;

enum GuiElement : unsigned int
{
    BACKGROUND = 0, TITLE_BG, FOOTER,
#ifdef SEGA
    A_BUTTON, B_BUTTON, C_BUTTON, X_BUTTON, Y_BUTTON, Z_BUTTON, START_BUTTON,
#else
    X_BUTTON, O_BUTTON, TRIANGLE_BUTTON, SQUARE_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, LEFT_RIGHT_BUTTONS, START_BUTTON, SELECT_BUTTON, L1_BUTTON, L2_BUTTON, R1_BUTTON, R2_BUTTON, L1_R1_BUTTONS,
#endif
    FOLDER_FOCUS, LIST_NORMAL, LIST_FOCUS, LINE_BREAK, ARROW_UP, ARROW_DOWN, ARROW_RIGHT, SLIDER_OFF, SLIDER_ON, ELEMENTS_END
};

class GuiHelper
{
public:
    ~GuiHelper();
    static GuiHelper & GetInstance();
    void AttachRenderer(SDL_Renderer * renderer);
    Texture CreateTitleTexture(const std::string & titleString, int x, int y, bool centered = true);
    Texture CreateFooterItemTexture(GuiElement element, const std::string & text, TextItemType itemType);
    void CreateFooterItems(const std::initializer_list<std::tuple<GuiElement, std::string>> & items, std::list<Texture> & textures, int yPos = 660);
    void CreateFooterItems(const std::initializer_list<std::tuple<GuiElement, std::string>> & items, std::list<Texture> & textures, int yPos, TextItemType itemType);
    SDL_Renderer * GetRenderer() const;
    const Texture & GetTexture(GuiElement element);
    Texture GetTexture(const fs::path & file);
    const fs::path & GetTextureFolder();
    const fs::path & GetThemeFolder();
    void SetImageFolder(const fs::path & imageFolder);
    void SetThemeFolder(const fs::path & themeFolder);
    void SetTextureFolder(const fs::path & textureFolder);

private:
    GuiHelper();
    GuiHelper(const GuiHelper&) = delete;
    GuiHelper& operator =(const GuiHelper&) = delete;
    void generateCommonTextures();

    SDL_Renderer* renderer;
    Texture textures[ELEMENTS_END];
    fs::path image_folder, texture_folder, theme_folder;
};

#endif // GUIHELPER_H_

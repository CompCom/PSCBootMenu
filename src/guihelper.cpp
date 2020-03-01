/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "guihelper.h"

GuiHelper::GuiHelper() : image_folder("images"), texture_folder("/media/bleemsync/etc/bleemsync/THEME/stock/menu_files/")
{}

GuiHelper::~GuiHelper()
{}

GuiHelper & GuiHelper::GetInstance()
{
    static GuiHelper _instance;
    return _instance;
}

void GuiHelper::AttachRenderer(SDL_Renderer * renderer)
{
    this->renderer = renderer;
    generateCommonTextures();
}

Texture GuiHelper::CreateArrowedText(Texture & valueTexture, const int spacing)
{
    Texture arrow = GuiHelper::GetInstance().GetTexture(GuiElement::ARROW_RIGHT);
    const int arrowW = arrow.rect.w;
    const auto & valueRect = valueTexture.rect;

    Texture outputTexture;
    outputTexture.CreateRendableTexture(renderer, (valueRect.w + (arrowW + spacing) * 2), valueRect.h, 0, 0, false);
    outputTexture.SetAsRenderTarget(renderer);
    arrow.SetPosition(arrowW/2, outputTexture.rect.h/2, true);
    arrow.Draw(renderer, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
    valueTexture.SetPosition((arrowW + spacing), 0, false);
    valueTexture.Draw(renderer);
    arrow.rect.x = valueRect.x + valueRect.w + spacing;
    arrow.Draw(renderer);
    SDL_SetRenderTarget(renderer, NULL);

    return outputTexture;
}

Texture GuiHelper::CreateTitleTexture(const std::string & titleString, int x, int y, bool centered)
{
    Texture title;
    title.CreateRendableTexture(renderer, 426, 54, x, y, centered);
    title.SetAsRenderTarget(renderer);

    textures[TITLE_BG].Draw(renderer);
    Texture text = FontManager::CreateTexture(HEADER_TEXT, titleString, 33, 426/2, 54/2, true);
    text.Draw(renderer);

    SDL_SetRenderTarget(renderer, NULL);

    return title;
}

Texture GuiHelper::CreateFooterItemTexture(GuiElement element, const std::string & text, TextItemType itemType)
{
    Texture newTexture;
    Texture button = textures[element];
    Texture textTexture = FontManager::CreateTexture(itemType, text, 20, 0, 0, false);
    textTexture.SetPosition(button.rect.w+5, button.rect.h/2-textTexture.rect.h/2, false);
    newTexture.CreateRendableTexture(renderer, button.rect.w + textTexture.rect.w + 5, button.rect.h);

    newTexture.SetAsRenderTarget(renderer);
    button.Draw(renderer);
    textTexture.Draw(renderer);
    SDL_SetRenderTarget(renderer, NULL);

    return newTexture;
}

void GuiHelper::CreateFooterItems(const std::initializer_list<std::tuple<GuiElement, std::string>> & items, std::list<Texture> & textures, int yPos)
{
    CreateFooterItems(items, textures, yPos, FOOTER_TEXT);
}

void GuiHelper::CreateFooterItems(const std::initializer_list<std::tuple<GuiElement, std::string>> & items, std::list<Texture> & textures, int yPos, TextItemType itemType)
{
    std::list<Texture> footerItems;
    for(const std::tuple<GuiElement, std::string> & item : items)
        footerItems.push_back(CreateFooterItemTexture(std::get<0>(item), std::get<1>(item), itemType));

    int boxSize = (footerItems.size()-1) * 20;
    for(const Texture & texture : footerItems)
        boxSize += texture.rect.w;

    int boxPosX = 1280/2 - boxSize/2;
    for(Texture & texture : footerItems)
    {
        texture.rect.x = boxPosX;
        texture.rect.y = yPos-texture.rect.h/2;
        boxPosX += texture.rect.w + 20;
        textures.push_back(texture);
    }
}

SDL_Renderer * GuiHelper::GetRenderer() const
{
    return renderer;
}

const Texture & GuiHelper::GetTexture(GuiElement element)
{
    return textures[element];
}

Texture GuiHelper::GetTexture(const fs::path & file)
{
    if(fs::exists(theme_folder/file))
    {
        return Texture(theme_folder/file, renderer);
    }
    else if(fs::exists(image_folder/file.filename()))
    {
        return Texture(image_folder/file.filename(), renderer);
    }
    else if(fs::exists(texture_folder/file))
    {
        return Texture(texture_folder/file, renderer);
    }
    return Texture(file, renderer);
}

const fs::path & GuiHelper::GetTextureFolder()
{
    return texture_folder;
}

const fs::path & GuiHelper::GetThemeFolder()
{
    return theme_folder;
}

void GuiHelper::SetImageFolder(const fs::path & imageFolder)
{
    image_folder = imageFolder;
}

void GuiHelper::SetThemeFolder(const fs::path & themeFolder)
{
    theme_folder = themeFolder;
}

void GuiHelper::SetTextureFolder(const fs::path & textureFolder)
{
    texture_folder = textureFolder;
}

void GuiHelper::generateCommonTextures()
{
    textures[BACKGROUND] = GetTexture("JP_US_BG.png");
    textures[TITLE_BG] = GetTexture("Title.png");
    textures[TITLE_BG].rect.h = 54;
    textures[TITLE_BG].rect.w = 426;

    textures[FOOTER] = GetTexture("Footer.png");
    textures[FOOTER].SetPosition(640, 677, true);
    textures[FOLDER_FOCUS] = GetTexture("Setting_Image_Focus.png");
    textures[LIST_NORMAL] = GetTexture("Setting_List_Nomal.png");
    textures[LIST_FOCUS] = GetTexture("Setting_List_Focus.png");
    textures[LINE_BREAK] = GetTexture("LineBreaker.png");
    textures[ARROW_UP] = GetTexture("Arrow_Up.png");
    textures[ARROW_DOWN] = GetTexture("Arrow_Down.png");
    textures[ARROW_RIGHT] = GetTexture("Arrow_Right.png");
    textures[SLIDER_OFF] = GetTexture("Settings/OFF_BoolSlider.png");
    textures[SLIDER_ON] = GetTexture("Settings/ON_BoolSlider.png");

#ifdef SEGA
    textures[A_BUTTON] = GetTexture("Buttons/A_Btn.png");
    textures[B_BUTTON] = GetTexture("Buttons/B_Btn.png");
    textures[C_BUTTON] = GetTexture("Buttons/C_Btn.png");
    textures[X_BUTTON] = GetTexture("Buttons/X_Btn.png");
    textures[Y_BUTTON] = GetTexture("Buttons/Y_Btn.png");
    textures[Z_BUTTON] = GetTexture("Buttons/Z_Btn.png");
    textures[START_BUTTON] = GetTexture("Buttons/Start_Btn.png");
#else
    //Create Controller Button Textures
    textures[X_BUTTON] = GetTexture("Buttons/X_Btn.png");
    textures[O_BUTTON] = GetTexture("Buttons/Circle_Btn.png");
    textures[TRIANGLE_BUTTON] = GetTexture("Buttons/Tri_Btn.png");
    textures[SQUARE_BUTTON] = GetTexture("Buttons/Square_Btn.png");
    textures[RIGHT_BUTTON] = GetTexture("Buttons/D_Btn.png");
    textures[START_BUTTON] = GetTexture("Buttons/Start_Btn.png");
    textures[SELECT_BUTTON] = GetTexture("Buttons/Sel_Btn.png");
    textures[L1_BUTTON] = GetTexture("Buttons/Sholder_L1_Btn.png");
    textures[L2_BUTTON] = GetTexture("Buttons/Sholder_L2_Btn.png");
    textures[R1_BUTTON] = GetTexture("Buttons/Sholder_R1_Btn.png");
    textures[R2_BUTTON] = GetTexture("Buttons/Sholder_R2_Btn.png");

    //Create Left Button Texture
    textures[LEFT_BUTTON].CreateRendableTexture(renderer, textures[RIGHT_BUTTON].rect.w, textures[RIGHT_BUTTON].rect.h);
    textures[LEFT_BUTTON].SetAsRenderTarget(renderer);
    textures[RIGHT_BUTTON].Draw(renderer, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
    SDL_SetRenderTarget(renderer, NULL);

    //Create Dual Button Texture
    textures[LEFT_RIGHT_BUTTONS].CreateRendableTexture(renderer, textures[RIGHT_BUTTON].rect.w*2, textures[RIGHT_BUTTON].rect.h);
    textures[LEFT_RIGHT_BUTTONS].SetAsRenderTarget(renderer);
    textures[LEFT_BUTTON].Draw(renderer);
    textures[RIGHT_BUTTON].rect.x = textures[LEFT_BUTTON].rect.w;
    textures[RIGHT_BUTTON].Draw(renderer);
    textures[RIGHT_BUTTON].rect.x = 0;
    SDL_SetRenderTarget(renderer, NULL);

    textures[L1_R1_BUTTONS].CreateRendableTexture(renderer, textures[L1_BUTTON].rect.w*2+2, textures[L1_BUTTON].rect.h);
    textures[L1_R1_BUTTONS].SetAsRenderTarget(renderer);
    textures[L1_BUTTON].Draw(renderer);
    textures[R1_BUTTON].rect.x = textures[L1_BUTTON].rect.w+2;
    textures[R1_BUTTON].Draw(renderer);
    textures[R1_BUTTON].rect.x = 0;
    SDL_SetRenderTarget(renderer, NULL);
#endif // SEGA
}

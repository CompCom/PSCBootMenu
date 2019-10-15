#ifndef EDITTEXTSCREEN_H_
#define EDITTEXTSCREEN_H_

#include "menuscreen.h"

class EditTextScreen : public MenuScreen
{
public:
    EditTextScreen(const std::string & title, std::string * originalText, const std::function<void(std::string)> & callbackFunction);
    ~EditTextScreen();
    virtual void Init();
    virtual void Update();
    virtual void Render();

private:
    void createKeys();
    void recreateTexture();

    std::function<void(std::string)> callbackFunction;
    std::string * originalText;
    std::string text;
    Texture textTexture;
    std::string title;
    bool shift;
    static const int lineCount = 4;
    HItemCollectionPtr collections[lineCount];
};

#endif

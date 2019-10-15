#include "edittextscreen.h"
#include "menuscreenmanager.h"

EditTextScreen::EditTextScreen(const std::string & title, std::string * originalText, const std::function<void(std::string)> & callbackFunction) : callbackFunction(callbackFunction), originalText(originalText), title(title), shift(false)
{
    if(originalText)
        text = *originalText;
}
EditTextScreen::~EditTextScreen()
{

}
void EditTextScreen::Init()
{
    textures.push_back(manager->GetBackground());

    textures.emplace_back(renderer, 640, 360, 500, 350, 0x80000000, true);

    Texture heading(fontBold, title, 36, renderer, 70, 50);
    heading.rect.x = 640-heading.rect.w/2;
    textures.push_back(heading);

    recreateTexture();
    Texture textBGTexture(renderer, 640, 230, 450, textTexture.rect.h, ~0);
    Texture instructions(fontBold, "R1 - Caps Lock    Triangle - Space    Circle - Backspace    Start - Save    Select - Cancel", 16, renderer, 640, 660, true);

    textures.push_back(textBGTexture);
    textures.push_back(instructions);

    for(auto & collection : collections)
    {
        collection = std::make_shared<HItemCollection>();
        collection->moveItem = false;
        items.push_back(collection);
    }

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
        case SDL_CONTROLLER_BUTTON_BACK:
            if(event.state == 1)
            {
                manager->RemoveCurrentScreen();
                return;
            }
            break;
        case SDL_CONTROLLER_BUTTON_START:
            manager->RemoveCurrentScreen();
            callbackFunction(text);
            return;
        case SDL_CONTROLLER_BUTTON_Y:
            text += ' ';
            recreateTexture();
            break;
        case SDL_CONTROLLER_BUTTON_B:
            if(text.size())
            {
                text.erase(text.end()-1);
                recreateTexture();
            }
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            if(event.state == 1)
            {
                shift = !shift;
                createKeys();
            }
            break;
        default:
            {
                int selectedKey = collections[selectedItem]->currentItem;
                if(int result = items[selectedItem]->handleButtonPress(event.button, event.state))
                {
                    selectedItem = (selectedItem + result + items.size()) % items.size();
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
}

void EditTextScreen::createKeys()
{
    const std::list<char> lines[lineCount] =       {{'`','1','2','3','4','5','6','7','8','9','0','-','='},
                                                    {'q','w','e','r','t','y','u','i','o','p','[',']','\\'},
                                                    {'a','s','d','f','g','h','j','k','l',';','\''},
                                                    {'z','x','c','v','b','n','m',',','.','/'}};
    const std::list<char> capLines[lineCount] =    {{'~','!','@','#','$','%','^','&','*','(',')','_','+'},
                                                    {'Q','W','E','R','T','Y','U','I','O','P','{','}','|'},
                                                    {'A','S','D','F','G','H','J','K','L',':','\"'},
                                                    {'Z','X','C','V','B','N','M','<','>','?'}};

    for(int lineNo = 0; lineNo < lineCount; ++lineNo)
    {
        auto & line = (shift) ? capLines[lineNo] : lines[lineNo];
        auto & collection = collections[lineNo];
        collection->items.clear();
        int i = 0;
        //int x = (lineNo == 0) ? 478 : 460 + lineNo*18;
        int y = 310+lineNo*36;
        for(auto c : line)
        {
            PushButtonPtr button = std::make_shared<PushButton>(std::string{c}, 36, renderer, 410 + i++ * 36, y, false);
            collection->addItem(button);
            button->onPress = [&,c]()
            {
                if(text.size() < 30)
                {
                    text.push_back(c);
                    recreateTexture();
                }
            };
        }
    }
}

void EditTextScreen::recreateTexture()
{
    textTexture = Texture(font, (text.size()) ? text : " ", 24, renderer, 640, 230, true, 0, 0, 0);
}

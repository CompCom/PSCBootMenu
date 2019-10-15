#include "edittextscreen.h"
#include "networkscreen.h"
#include "menuscreenmanager.h"
#include "wpa/os.h"
#include "wpa/wpa_ctrl.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>

using namespace std::chrono_literals;

namespace fs = std::experimental::filesystem;

void run_wpa_command(wpa_ctrl* ctrl, const std::string & cmd, std::list<std::string> * result = nullptr)
{
    char buffer[4096] = {0};
    size_t bufferSize = 4095;
    int wpa_result = wpa_ctrl_request(ctrl, cmd.c_str(), cmd.size(), buffer, &bufferSize, NULL);
    if(wpa_result != 0)
    {
        std::cout << "Failed to execute wpa command: " << cmd << std::endl;
        if(result)
            result->clear();
        return;
    }
    buffer[bufferSize] = '\0';
    std::cout << "WPA COMMAND: " << cmd << std::endl;
    std::cout << "WPA REPLY: " << (int)bufferSize << " " << buffer << std::endl;

    if(result)
    {
        result->clear();
        std::string temp;
        std::stringstream s(buffer);
        while(std::getline(s, temp))
        {
            result->push_back(temp);
        }
    }
}

std::string run_wpa_command_return_value(wpa_ctrl* ctrl, const std::string & cmd)
{
    std::list<std::string> result;
    run_wpa_command(ctrl, cmd, &result);
    if(result.size())
        return result.back();
    return std::string();
}

NetworkScreen::NetworkScreen() : ctrl(nullptr), isConnecting(0), statusCheckTime(std::chrono::system_clock::now())
{
}
NetworkScreen::~NetworkScreen()
{
    if(ctrl)
        wpa_ctrl_close(ctrl);
}

void NetworkScreen::Init()
{
    textures.emplace_back(manager->GetBackground());
    statusBg = textures.insert(textures.end(), Texture());
    statusTexture = textures.insert(textures.end(), Texture());
    textures.emplace_back(fontBold, "X - Connect to Network", 16, renderer, 1280*.3, 660, true);
    removeNetworkTexture = textures.insert(textures.end(), Texture(fontBold, u8"\u25A0 - Remove Network", 16, renderer, 1280*.5, 660, true));
    textures.emplace_back(fontBold, u8"\u25B2 - Rescan Networks", 16, renderer, 1280*.7, 660, true);

    if(system("ip link set wlan0 up") != 0)
    {
        manager->DisplayErrorScreen("Cannot detect wireless adapter.", true);
        return;
    }
    ctrl = wpa_ctrl_open("/var/run/wpa_supplicant/wlan0");
    if(ctrl == nullptr || run_wpa_command_return_value(ctrl, "PING").compare("PONG") != 0)
    {
        manager->DisplayErrorScreen("Cannot connect to wpa supplicant.", true);
        return;
    }

    // Populate network list
    std::list<std::string> networkList;
    run_wpa_command(ctrl, "LIST_NETWORKS", &networkList);
    networkList.erase(networkList.begin());
    
    for(auto & networkString : networkList)
    {
        int separatorPos = networkString.find('\t');
        int networkId = std::stoi(networkString.substr(0,separatorPos));
        int separatorPos1 = networkString.find('\t', separatorPos+1);
        std::string ssid = (separatorPos1 > 0) ? networkString.substr(separatorPos+1, separatorPos1-separatorPos-1) : networkString.substr(separatorPos+1);
        networks[ssid] = networkId;
        std::cout << "Found Network SSID:\"" << ssid << "\"" << std::endl;
    }

    collection = std::make_shared<VItemScrollCollection>();
    collection->displayItemCount = 24;
    items.push_back(collection);
    scan();
    checkStatus();
}
void NetworkScreen::Update()
{
    //Hacky method to only modify items within main thread.
    if(isConnecting.load() == 2)
    {
        items.erase(--items.end());
        isConnecting = 0;
        if(run_wpa_command_return_value(ctrl, "SAVE_CONFIG").compare("OK") != 0)
            manager->DisplayErrorScreen("Failed to save network config.");
    }
    else if(isConnecting.load() == 0 && (std::chrono::system_clock::now()-statusCheckTime) >= 1s)
        checkStatus();

    MenuScreen::Update();
}

void NetworkScreen::checkStatus()
{
    std::list<std::string> result;
    std::string status;
    run_wpa_command(ctrl, "STATUS", &result);
    for(const auto & line : result)
    {
        status += line;
        status += '\n';
    }
    *statusTexture = Texture(font, status, 16, renderer, 1280*.6, 720*.4);
    statusCheckTime = std::chrono::system_clock::now();
}

void NetworkScreen::connectToNetwork(const std::string & ssid)
{
    if(networks.count(ssid) == 0)
    {
        auto callback = [ssid,this] (std::string password)
        {
            std::string netIdString = run_wpa_command_return_value(ctrl, "ADD_NETWORK");
            if(netIdString.compare("FAIL") == 0)
            {
                manager->DisplayErrorScreen("Failed to add new network.");
                return;
            }
            int netId = std::stoi(netIdString);
            if(run_wpa_command_return_value(ctrl, "SET_NETWORK " + netIdString + " ssid \"" + ssid + "\"").compare("OK") != 0)
            {
                manager->DisplayErrorScreen("Failed to set network ssid.");
                return;
            }
            if(password.size())
            {
                if(run_wpa_command_return_value(ctrl, "SET_NETWORK " + netIdString + " psk \"" + password + "\"").compare("OK") != 0)
                {
                    run_wpa_command(ctrl, "REMOVE_NETWORK " + netIdString);
                    manager->DisplayErrorScreen("Failed to set network password.");
                    return;
                }
            }
            networks[ssid] = netId;

            auto msgBox = std::make_shared<MessageBox>("Information", "Attempting to connect to network.", renderer);
            items.push_back(msgBox);
            msgBox->setPosition(640,360);

            isConnecting = 1;
            connectThread = std::async(std::launch::async, [this,ssid]() { enableNetwork(ssid); } );
        };
        manager->AddNewScreen(std::make_shared<EditTextScreen>("Enter Password", nullptr, callback));

        return;
    }

    auto msgBox = std::make_shared<MessageBox>("Information", "Attempting to connect to network.", renderer);
    items.push_back(msgBox);
    msgBox->setPosition(640,360);

    isConnecting = 1;
    connectThread = std::async(std::launch::async, [this,ssid]() { enableNetwork(ssid); } );
}

void NetworkScreen::enableNetwork(const std::string & ssid)
{
    int udhcpcPid = -1;
    if(fs::exists("/var/run/udhcpc.wlan0.pid"))
    {
        std::ifstream in("/var/run/udhcpc.wlan0.pid");
        if(in.is_open())
        {
            in >> udhcpcPid;
            in.close();
            if(udhcpcPid > 0)
                kill(udhcpcPid, SIGUSR2);
        }
    }
    run_wpa_command(ctrl, "DISCONNECT");
    run_wpa_command(ctrl, "SELECT_NETWORK  " + std::to_string(networks[ssid]));
    run_wpa_command(ctrl, "RECONNECT");

    if(udhcpcPid > 0)
        kill(udhcpcPid, SIGUSR1);
    else
        system("udhcpc -R -x hostname:\"$HOSTNAME\" -t 5 -n -p /var/run/udhcpc.wlan0.pid -i wlan0");

    isConnecting = 2;
}

void NetworkScreen::handleButtonPress(const GameControllerEvent * event)
{
    if(isConnecting.load())
        return;

    switch(event->button)
    {
    case SDL_CONTROLLER_BUTTON_B:
        if(event->state == 1)
        {
            manager->RemoveCurrentScreen();
            return;
        }
        break;
    case SDL_CONTROLLER_BUTTON_Y:
        if(event->state == 1)
            scan();
        break;
    default:
        MenuScreen::handleButtonPress(event);
        break;
    }
}

void NetworkScreen::removeNetwork(const std::string & ssid)
{
    if(networks.count(ssid))
    {
        run_wpa_command(ctrl, "REMOVE_NETWORK " + std::to_string(networks[ssid]));
        networks.erase(ssid);
        if(run_wpa_command_return_value(ctrl, "SAVE_CONFIG").compare("OK") != 0)
            manager->DisplayErrorScreen("Failed to save network config.");
    }
    updateIcons(ssid);
}

void NetworkScreen::scan()
{
    collection->clearItems();
#ifdef __arm__
    run_wpa_command(ctrl, "SCAN");
    std::list<std::string> networkList;
    run_wpa_command(ctrl, "SCAN_RESULTS", &networkList);
    networkList.erase(networkList.begin());
    for(auto & network : networkList)
    {
        network = network.substr(network.find_last_of('\t')+1);
    }
#else
    std::list<std::string> networkList;
    for(int i = 0; i < 30; ++i)
        networkList.push_back("Network" + std::to_string(i));
#endif
    for(const auto & network : networkList)
    {
        AdvancedPushButtonPtr networkButton = std::make_shared<AdvancedPushButton>(network, 16, renderer, 0, 0);
        networkButton->onPress[SDL_CONTROLLER_BUTTON_A] = std::bind(&NetworkScreen::connectToNetwork, this, network);
        networkButton->onPress[SDL_CONTROLLER_BUTTON_X] = std::bind(&NetworkScreen::removeNetwork, this, network);
        networkButton->onEnter = std::bind(&NetworkScreen::updateIcons, this, network);
        collection->addItem(std::dynamic_pointer_cast<MenuItem>(networkButton));
    }
}

void NetworkScreen::updateIcons(const std::string & ssid)
{
    if(networks.count(ssid))
    {
        SDL_SetTextureColorMod(removeNetworkTexture->texture.get(), 0xFF, 0xFF, 0xFF);
    }
    else
    {
        SDL_SetTextureColorMod(removeNetworkTexture->texture.get(), 0x80, 0x80, 0x80);
    }
}

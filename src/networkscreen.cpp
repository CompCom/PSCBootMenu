/**
  * Copyright (C) 2018-2020 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#include "edittextscreen.h"
#include "gamebutton.h"
#include "guihelper.h"
#include "languagemanager.h"
#include "menuitems/genericmenuentry.h"
#include "menuitems/messagebox.h"
#include "menuitems/vitemscrollcollection.h"
#include "menuscreenmanager.h"
#include "networkscreen.h"
#include "wpa/wpa_ctrl.h"
#include "wpa/wpa_passphrase.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>

using namespace std::chrono_literals;

namespace fs = std::experimental::filesystem;

#ifdef SEGA
constexpr GameButton ConnectBtn = GameButtons[GAME_BUTTON_SEGA_A], RemoveBtn = GameButtons[GAME_BUTTON_SEGA_C], ScanBtn = GameButtons[GAME_BUTTON_SEGA_START], BackBtn = GameButtons[GAME_BUTTON_SEGA_B];
#else
constexpr GameButton ConnectBtn = GameButtons[GAME_BUTTON_X], RemoveBtn = GameButtons[GAME_BUTTON_SQUARE], ScanBtn = GameButtons[GAME_BUTTON_TRIANGLE], BackBtn = GameButtons[GAME_BUTTON_O];
#endif // SEGA

static std::mutex wpa_mutex;
void run_wpa_command(wpa_ctrl* ctrl, const std::string & cmd, std::list<std::string> * result = nullptr, bool logResult = true)
{
    std::lock_guard<std::mutex> _lock(wpa_mutex);
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
    if(logResult)
    {
        std::cout << "WPA COMMAND: " << cmd << std::endl;
        std::cout << "WPA REPLY: " << (int)bufferSize << " " << buffer << std::endl;
    }

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

NetworkScreen::NetworkScreen() : ctrl(nullptr), isConnecting(false), statusCheckTime(std::chrono::system_clock::now())
{
}
NetworkScreen::~NetworkScreen()
{
    if(ctrl)
        wpa_ctrl_close(ctrl);
}

void NetworkScreen::Init()
{
    GuiHelper & guiHelper = GuiHelper::GetInstance();
    textures.push_back(guiHelper.GetTexture(BACKGROUND));
    textures.push_back(guiHelper.GetTexture(FOOTER));
    guiHelper.CreateFooterItems({
                                    {ConnectBtn.GUI_Value, LanguageManager::GetString("connect")},
                                    {RemoveBtn.GUI_Value, LanguageManager::GetString("remove")},
                                    {ScanBtn.GUI_Value, LanguageManager::GetString("scan")},
                                    {BackBtn.GUI_Value, LanguageManager::GetString("back")}
                                },
                                textures);
    removeNetworkTexture = std::prev(textures.end(), 3);
    textures.insert(textures.end(), guiHelper.GetTexture(GuiElement::LINE_BREAK))->SetPosition(640, 568, true);
    statusTexture = textures.insert(textures.end(), Texture());
    textures.push_back(guiHelper.CreateTitleTexture(LanguageManager::GetString("nm_title"), 640, 72));
    if(system("ip link set wlan0 up") != 0)
    {
        manager->DisplayErrorScreen(LanguageManager::GetString("cannot_find_adapter"), true);
        return;
    }
    ctrl = wpa_ctrl_open("/var/run/wpa_supplicant/wlan0");
    if(ctrl == nullptr || run_wpa_command_return_value(ctrl, "PING").compare("PONG") != 0)
    {
        manager->DisplayErrorScreen(LanguageManager::GetString("wpa_connection_error"), true);
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
    collection->displayItemCount = 7;
    collection->yOffset = 64;
    collection->y = 150;
    //Arrow Pos = center screen + half generic entry size + spacer
    collection->setArrowPosition(640+335+30);
    items.push_back(collection);
    scan();
    checkStatus();
}
void NetworkScreen::Update()
{
    if(isConnecting.load() == false && (std::chrono::system_clock::now()-statusCheckTime) >= 1s)
        checkStatus();
    else
        items[selectedItem]->Update();

    MenuScreen::Update();
}

void NetworkScreen::checkStatus()
{
    std::string connectStatus = "N/A", ipAddress = "N/A", ssid = "N/A";
    std::list<std::string> result;
    run_wpa_command(ctrl, "STATUS", &result);
    for(const std::string & line : result)
    {
        std::string variable = line.substr(0, line.find('='));
        if(variable.compare("ssid") == 0)
            ssid = line.substr(line.find('=')+1);
        else if(variable.compare("wpa_state") == 0)
            connectStatus = line.substr(line.find('=')+1);
        else if(variable.compare("ip_address") == 0)
            ipAddress = line.substr(line.find('=')+1);
    }
    std::string status = "SSID: " + ssid + "    " + LanguageManager::GetString("status") + ": " + connectStatus + "    IP ADDRESS: " + ipAddress;
    *statusTexture = FontManager::CreateTexture(TextItemType::DEFAULT_TEXT, status, 16, 640, 598, true);
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
                manager->DisplayErrorScreen(LanguageManager::GetString("network_add_error"));
                return;
            }
            int netId = std::stoi(netIdString);
            if(run_wpa_command_return_value(ctrl, "SET_NETWORK " + netIdString + " ssid \"" + ssid + "\"").compare("OK") != 0)
            {
                manager->DisplayErrorScreen(LanguageManager::GetString("network_ssid_error"));
                return;
            }
            if(password.size())
            {
                std::string psk;
                try
                {
                    if(password.size() >= 8 && password.size() <= 63)
                        wpa_passphrase(ssid, password, psk);
                }
                catch(...) {}

                if(psk.size() == 0 || run_wpa_command_return_value(ctrl, "SET_NETWORK " + netIdString + " psk " + psk).compare("OK") != 0)
                {
                    run_wpa_command(ctrl, "REMOVE_NETWORK " + netIdString);
                    manager->DisplayErrorScreen(LanguageManager::GetString("network_password_error"));
                    return;
                }
            }
            networks[ssid] = netId;

            createConnectingMessageBox(ssid);
        };
        manager->AddNewScreen<EditTextScreen>(LanguageManager::GetString("enter_pass_title"), nullptr, callback);

        return;
    }

    createConnectingMessageBox(ssid);
}

void NetworkScreen::createConnectingMessageBox(const std::string & ssid)
{
    isConnecting = true;
    auto msgBox = std::make_shared<MessageBox>(LanguageManager::GetString("info_box_title"), LanguageManager::GetString("network_connecting"), renderer);
    items.push_back(msgBox);
    msgBox->setPosition(640,360);
    msgBox->backgroundTask = std::async(std::launch::async, [this,ssid]() { enableNetwork(ssid); } );
    msgBox->endFunction =   [this, prevSelectedItem=selectedItem]()
                            {
                                items.erase(--items.end());
                                selectedItem = prevSelectedItem;
                                isConnecting = false;
                                if(run_wpa_command_return_value(ctrl, "SAVE_CONFIG").compare("OK") != 0)
                                    manager->DisplayErrorScreen(LanguageManager::GetString("save_net_config_failed"));
                            };
    selectedItem = items.size()-1;
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
}

void NetworkScreen::handleButtonPress(const GameControllerEvent * event)
{
    if(isConnecting.load())
        return;

    switch(event->button)
    {
    case BackBtn.SDL_Value:
        if(event->state == 1)
        {
            manager->RemoveCurrentScreen();
            return;
        }
        break;
    case ScanBtn.SDL_Value:
        if(event->state == 1)
            scan();
        break;
    default:
        MenuScreen::handleButtonPress(event);
        break;
    }
}

void NetworkScreen::populateNetworks()
{
    collection->clearItems();

    std::list<std::string> networkList;
    run_wpa_command(ctrl, "SCAN_RESULTS", &networkList);
    networkList.erase(networkList.begin());
    for(auto & network : networkList)
    {
        network = network.substr(network.find_last_of('\t')+1);
    }

    GuiHelper & guiHelper = GuiHelper::GetInstance();

    for(const auto & network : networkList)
    {
        // Ignore if network name is missing or network is hidden
        if(network.size() == 0 || network.compare(0, 4, "\\x00") == 0)
            continue;
        
        GenericMenuEntryPtr networkButton = std::make_shared<GenericMenuEntry>(guiHelper.GetTexture(GuiElement::LIST_NORMAL), guiHelper.GetTexture(GuiElement::LIST_FOCUS), 440);
        networkButton->SetLeft(network);
        networkButton->onPress[ConnectBtn.SDL_Value] = std::bind(&NetworkScreen::connectToNetwork, this, network);
        networkButton->onPress[RemoveBtn.SDL_Value] = std::bind(&NetworkScreen::removeNetwork, this, network);
        networkButton->onEnter = std::bind(&NetworkScreen::updateIcons, this, network);
        collection->addItem(std::dynamic_pointer_cast<MenuItem>(networkButton));
    }
}

void NetworkScreen::removeNetwork(const std::string & ssid)
{
    if(networks.count(ssid))
    {
        run_wpa_command(ctrl, "REMOVE_NETWORK " + std::to_string(networks[ssid]));
        networks.erase(ssid);
        if(run_wpa_command_return_value(ctrl, "SAVE_CONFIG").compare("OK") != 0)
            manager->DisplayErrorScreen(LanguageManager::GetString("save_net_config_failed"));
    }
    updateIcons(ssid);
}

void NetworkScreen::scan()
{
    // Check that wpa is ready for scanning
    if(run_wpa_command_return_value(ctrl, "SCAN").compare("OK") != 0)
        return;

    // Create a message box to wait for wpa to report scanning completed
    isConnecting = true;
    auto msgBox = std::make_shared<MessageBox>(LanguageManager::GetString("info_box_title"), LanguageManager::GetString("scanning_networks"), renderer);
    items.push_back(msgBox);
    msgBox->setPosition(640, 360);
    msgBox->backgroundTask = std::async(std::launch::async,
                                        [this]()
                                        {
                                            std::string wpaState = "SCANNING";
                                            while(wpaState.compare("SCANNING") == 0)
                                            {
                                                std::this_thread::sleep_for(100ms);

                                                std::list<std::string> result;
                                                run_wpa_command(ctrl, "STATUS", &result, false);

                                                for (const std::string &line : result)
                                                {
                                                    std::string variable = line.substr(0, line.find('='));
                                                    if (variable.compare("wpa_state") == 0)
                                                        wpaState = line.substr(line.find('=') + 1);
                                                }
                                            }
                                        });
    msgBox->endFunction =   [this, prevSelectedItem = selectedItem]()
                            {
                                items.erase(--items.end());
                                selectedItem = prevSelectedItem;
                                isConnecting = false;
                                populateNetworks();
                            };

    selectedItem = items.size() - 1;
}

void NetworkScreen::updateIcons(const std::string & ssid)
{
    if(networks.count(ssid))
    {
        SDL_SetTextureAlphaMod(removeNetworkTexture->texture.get(), 0xFF);
    }
    else
    {
        SDL_SetTextureAlphaMod(removeNetworkTexture->texture.get(), 0x80);
    }
}

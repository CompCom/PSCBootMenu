/**
  * Copyright (C) 2018-2019 CompCom
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 3
  * of the License, or (at your option) any later version.
  */
 
#ifndef NETWORKSCREEN_H_
#define NETWORKSCREEN_H_

#include "menuscreen.h"
#include <chrono>
#include <future>
#include <map>
class VItemScrollCollection;
using VItemScrollCollectionPtr = std::shared_ptr<VItemScrollCollection>;
struct wpa_ctrl;

class NetworkScreen : public MenuScreen
{
public:
    NetworkScreen();
    ~NetworkScreen();
    void Init() override;
    void Update() override;

private:
    void checkStatus();
    void connectToNetwork(const std::string & ssid);
    void createConnectingMessageBox(const std::string & ssid);
    void enableNetwork(const std::string & ssid);
    void handleButtonPress(const GameControllerEvent * event) override;
    void removeNetwork(const std::string & ssid);
    void scan();
    void updateIcons(const std::string & ssid);

    VItemScrollCollectionPtr collection;
    wpa_ctrl* ctrl;
    std::atomic_bool isConnecting;
    std::map<std::string, int> networks;
    std::list<Texture>::iterator removeNetworkTexture, statusTexture;
    std::chrono::time_point<std::chrono::system_clock> statusCheckTime;
};

#endif

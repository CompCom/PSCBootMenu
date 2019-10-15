#ifndef NETWORKSCREEN_H_
#define NETWORKSCREEN_H_

#include "menuscreen.h"
#include <chrono>
#include <future>
#include <map>
class VItemCollection;
using VItemCollectionPtr = std::shared_ptr<VItemCollection>;
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
    void enableNetwork(const std::string & ssid);
    void handleButtonPress(const GameControllerEvent * event) override;
    void removeNetwork(const std::string & ssid);
    void scan();
    void updateIcons(const std::string & ssid);

    VItemScrollCollectionPtr collection;
    std::future<void> connectThread;
    wpa_ctrl* ctrl;
    std::atomic_int isConnecting;
    std::map<std::string, int> networks;
    std::list<Texture>::iterator removeNetworkTexture, statusBg, statusTexture;
    std::chrono::time_point<std::chrono::system_clock> statusCheckTime;
};

#endif

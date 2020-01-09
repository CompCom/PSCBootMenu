#ifndef WPA_PASSPHRASE_H_
#define WPA_PASSPHRASE_H_

#include <string>

void wpa_passphrase(const std::string & ssid, const std::string & passphrase, std::string & pskOut);

#endif
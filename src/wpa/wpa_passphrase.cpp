/*
 * WPA Supplicant - ASCII passphrase to WPA PSK tool
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

// Modified for C++ use by CompCom 2019

#include "sha1.h"
#include "wpa_passphrase.h"

void wpa_passphrase(const std::string & ssid, const std::string & passphrase, std::string & pskOut)
{
	unsigned char psk[32];

	pbkdf2_sha1(passphrase.c_str(), (u8 *) ssid.c_str(), ssid.size(), 4096, psk, 32);

	pskOut.resize(64);
	for (int i = 0; i < 32; i++)
		sprintf(&pskOut[i*2], "%02x", psk[i]);
}

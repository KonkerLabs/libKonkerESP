#ifndef __CONFIG_WIFI_H__
#define __CONFIG_WIFI_H__

#ifndef ESP32
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
//note: WiFi comes from {ESP8266WiFi,Wifi}.h
#include "globals.h"
#include "manage_eeprom.h"
//note: deviceEEPROM comes from manage_eeprom.h

#define MAX_NUM_WIFI_CRED       3
#define WIFI_CRED_ARRAY_SIZE    16

// struct is used to store credentials in device memory
// 34 bytes
struct wifi_credentials
{
  uint16_t enabled;
  char SSID[WIFI_CRED_ARRAY_SIZE];
  char PASSWD[WIFI_CRED_ARRAY_SIZE];
};

class WifiManager
{
private:
  wifi_credentials wifiCredentials[MAX_NUM_WIFI_CRED];
  int numWifiCredentials = 0;

  bool tryConnect(String ssid, String password);
  bool tryConnectSSID(String ssid, String password, int retries);

public:
  WifiManager();
  void setConfig(String ssid, String password);
  void setConfig(String ssid0, String password0, String ssid1, String password1);
  void setConfig(String ssid0, String password0, String ssid1, String password1, String ssid2, String password2);
  void removeConfig(String ssid);

  bool tryConnectClientWifi();
  bool checkConnectionStatus();
  void disconnectClientWifi();

  int getNumberOfWifiCredentials();
  IPAddress getLocalIP();
  int getWifiStrenght();
  String getWifiSSID();

  int saveWifiCredentials();
  // overwrites existing credentials (if any)
  int restoreWifiCredentials();

  //TODO AP mode
};

#endif

#ifndef __CONFIG_WIFI_H__
#define __CONFIG_WIFI_H__

#ifndef ESP32
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include "globals.h"

#define MAX_NUM_WIFI_CRED   3

// struct is used to store credentials in device memory
struct wifi_credentials
{
  String SSID;
  String PASSWD;
};
typedef struct wifi_credentials WifiCredentials;

class WifiManager
{
private:
  WifiCredentials wifiCredentials[MAX_NUM_WIFI_CRED];
  int numWifiCredentials = 0;
  bool i=0;
  bool g=0;
  bool s=0;

  // void setIP (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
  // void setGateway (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
  // void setSubnet (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
  bool tryConnect(String ssid, String password);
  bool tryConnectSSID(String ssid, String password, int retries);

public:
  // IPAddress ip;
  // IPAddress gateway;
  // IPAddress subnet;
  String wifiFile;

  WifiManager();
  WifiManager(String wifiFile);
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

  // IPAddress getIP();
  // bool isConfigured();

//TODO gravar credenciais do Wifi em arquivo no FS

//TODO AP mode
};

#endif

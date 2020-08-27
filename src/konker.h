#ifndef __KONKER_H__
#define __KONKER_H__

#include <Arduino.h>

#ifndef ESP32
#include "user_interface.h"
#endif

#ifndef ESP32
#include <ESP8266WebServer.h>
#endif

#include "globals.h"
#include "buffer/buffer_entry.h"
#include "wireless/manage_wifi.h"
#include "platform/manage_platform.h"
#include "protocols/all_protocols.h"
#include "file_system/manage_eeprom.h"
#include "health/health_monitor.h"
#include "helpers/NTP_helper.h"
#include "helpers/json_helper.h"
#include "update/firmware_update.h"

// Konker ESP CLASS

class KonkerDevice
{
private:
  WifiManager deviceWifi;
  HealthMonitor deviceMonitor;

  ConnectionType defaultConnectionType;
  ConnectionType fallbackConnectionType;

#ifndef ESP32
  int resetPin = D5;
#else
  int resetPin = 13;
#endif

  // bool _encripted = false;
#ifndef ESP32
  ESP8266WebServer webServer;
#else
  WebServer webServer;
#endif

  NTPHelper deviceNTP;

  ESPHTTPKonkerUpdate deviceUpdate;

  // identificacao do device
  String deviceID;
  String chipID; // = deviceID + ESP.getChipId

  // fila de envio do device
  BufferEntry sendBuffer;
  BufferEntry receiveBuffer;

  // flag when the device checked for a device update
  unsigned int avgLoopDuration = 0;
  unsigned int loopCount = 1;

  Protocol * currentProtocol;
  Protocol * httpProtocol;

  // void flushBuffer();
  // void formatFileSystem();
  void setChipID(String deviceID);
  // Unique ID operations
  void setDeviceIds(String id);
  // returns true if currentProtocol is set (aka, not null)
  bool checkProtocol();
  bool saveWifiCredentials();
  bool restoreWifiCredentials();
  bool savePlatformCredentials();
  bool restorePlatformCredentials();
  // tries to recover credentials from memory
  // if fails, tries to get from server
  // if fails, restart device
  int getCredentialsForPlatform(String *, String *, String *);

public:
  KonkerDevice();
  ~KonkerDevice();

  void restartDevice();
  void resetALL();
  String getDeviceId();

  // configuration functions
  void setServer(String host, int port);
  void setServer(String host, int port, int httpPort);
  void setPlatformCredentials(String userid, String password);
  void setPlatformCredentials(String deviceID, String userid, String password);

  // Credentials operations
  bool saveAllCredentials();
  bool restoreAllCredentials();

  // WiFi operations
  void addWifi(String ssid, String password);
  void clearWifi(String ssid);
  bool connectWifi();
  bool checkWifiConnection();
  int getNumWifiCredentials();

  // Platform connection functions
  void setDefaultConnectionType(ConnectionType c);
  void setFallbackConnectionType(ConnectionType c);
  ConnectionType getDefaultConnectionType();
  ConnectionType getFallbackConnectionType();
  void startConnection(bool afterReconnect);
  void stopConnection();
  int checkPlatformConnection();

  void loopDuration(unsigned int duration); //measured in useconds
  void loop();

  // communication interface
  void getCurrentTime(char * timestamp);

  /* returns 1 if send is OK or <= 0 if error ocurred when sending data to the server */
  int sendData();
  int sendData(String channel, String payload);
  // buffer operations
  int storeData(String channel, String payload);
  BufferElement recoverData();

  // TODO
  /* returns 1 if a message exists and is copied to the buffer or 0 if nothing exists */
  // int receive(String *buffer);
};
#endif /* __KONKER_H__ */

#ifndef __KONKER_H__
#define __KONKER_H__

#include <Arduino.h>

#ifndef ESP32
#include "user_interface.h"
#endif

#ifndef ESP32
#include <ESP8266WebServer.h>
// #include <ESP8266HTTPClient.h>
#endif

#include "globals.h"
#include "buffer/buffer_entry.h"
#include "wireless/manage_wifi.h"
#include "platform/manage_platform.h"
#include "protocols/all_protocols.h"
#include "file_system/manage_eeprom.h"
#include "health/health_monitor.h"
#include "helpers/NTP_helper.h"
#include "update/firmwareUpdate.h"

#define _STATUS_LED 2

// debug levels
/*
// 0 - OFF	The lowest possible rank and is intended to turn off logging.
// 1 - INFO	Designates informational messages that highlight the progress of the application at coarse-grained level.
// 2 - DEBUG	Designates fine-grained informational events that are most useful to debug an application.
// 3 - WARN	Designates potentially harmful situations.
// 4 - ERROR	Designates error events that might still allow the application to continue running.
// 5 - FATAL	Designates very severe error events that will presumably lead the application to abort.
// 6 - ALL	All levels including custom levels.
*/

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

  String NAME = DEFAULT_NAME; // pra que?

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
  // returns true if currentProtocol is set (aka, not null)
  bool checkProtocol();
  bool saveWifiCredentials();
  bool restoreWifiCredentials();
  bool savePlatformCredentials();
  bool restorePlatformCredentials();

public:
  KonkerDevice();
  ~KonkerDevice();

  void restartDevice();

  // configuration functions
  void setServer(String host, int port);
  void setPlatformCredentials(String userid, String password);
  void setPlatformCredentials(String deviceID, String userid, String password);

  // WiFi operations
  void addWifi(String ssid, String password);
  void clearWifi(String ssid);
  bool connectWifi();
  bool checkWifiConnection();
  int getNumWifiCredentials();

  // Unique ID operations
  void setUniqueID(String id);
  String getUniqueID();

  // Platform connection functions
  void setDefaultConnectionType(ConnectionType c);
  void setFallbackConnectionType(ConnectionType c);
  ConnectionType getDefaultConnectionType();
  ConnectionType getFallbackConnectionType();
  void startConnection(bool afterReconnect);
  void stopConnection();
  int checkPlatformConnection();

  // Credentials operations
  bool saveAllCredentials();
  bool restoreAllCredentials();
  void resetALL();

  void loopDuration(unsigned int duration); //measured in useconds
  void loop();

  // communication interface
  void getCurrentTime(char * timestamp);

  /* returns 1 if send is OK or <= 0 if error ocurred when sending data to the server */
  int sendData(String channel, String payload);
  int sendData();
  int storeData(String channel, String payload);
  BufferElement recoverData();

  // int testSendHTTP();
  // /* returns 1 if a message exists and is copied to the buffer or 0 if nothing exists */
  // int receive(String *buffer);
  //
  // HTTPClient* getAPIClient();
  //
  // // internal interface
  // // check if is there any update / reconfiguration for this device on the platform
  // void checkForUpdates(); needed???
};
#endif /* __KONKER_H__ */

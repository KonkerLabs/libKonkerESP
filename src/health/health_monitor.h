#ifndef __HEALTH_H__
#define __HEALTH_H__

#include <string>
#include <unordered_map>
#include "ESP8266Ping.h"
#include "globals.h"
#include "protocol.h"
#include "connection/http_protocol.h"
#include "manage_wifi.h"
#include "json_helper.h"
#include "manage_eeprom.h"
#include "manage_platform.h"

/*
 * Health information to send:
 * - Number of times Protocol failed to send or connect
 * - Number of times HTTPProtocol failed to send or connect
 * - Number of times failed to connect to Wifi
 * - WiFi SSID, RSSI and local IP
 * - Build number
 * - MAC address
 * - Main loop average duration time (reset when sent to platform)
 * - Memory usage [ESP.getFreeHeap()]
 * - Device voltage [ESP.getVcc()]
 * - Ping platform (ms)
 */

 struct health_conn_st_t
 {
   uint16_t nfail;
   uint16_t mfail;
   uint16_t hfail;
 };

class HealthMonitor
{
private:
  const char health_channel[8];
  // String healthFile = "health.json";

  WifiManager * pDeviceWifi;
  Protocol* pCurrentProtocol; //protcol to get health information [TODO] needed?
  HTTPProtocol* httpObj; //protocol to send health updates

  stringmap healthInfo;

  unsigned long last_time_health_send = 0;

  int pingPlatform();
  void collectHealthInfo(unsigned int loopDuration);

public:
  HealthMonitor();
  HealthMonitor(WifiManager * deviceWifi);
  ~HealthMonitor();

  void setProtocol(Protocol * protocol, Protocol* httpProtocol);

  bool saveHealthInfo();
  bool restoreHealthInfo();

  // heart beat to the server to send status information for the device
  void healthUpdate(unsigned int loopDuration);
};

#endif /* __HEALTH_H__ */

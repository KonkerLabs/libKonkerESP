#ifndef __HEALTH_H__
#define __HEALTH_H__

#include <unordered_map>
#include <string>
#include "globals.h"
#include "protocol.h"
#include "connection/http_protocol.h"
#include "manage_wifi.h"
#include "json_helper.h"
#include "manage_eeprom.h"

/*
 * Health information to send:
 * - Number of times Protocol failed to send or connect
 * - Number of times failed to connect to Wifi
 * - WiFi SSID, RSSI and local IP
 * - Build number
 * - MAC address
 * - Main loop average duration time (reset when sent to platform)
 * - Memory usage [ESP.getFreeHeap()]
 * - Device voltage [ESP.getVcc()] (?)
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
  Protocol* pCurrentProtocol;
  HTTPProtocol httpObj; //create new object to send health updates

  stringmap healthInfo;

  unsigned long last_time_health_send = 0;

  void collectHealthInfo(unsigned int loopDuration);

public:
  HealthMonitor();
  HealthMonitor(WifiManager * deviceWifi);
  ~HealthMonitor();

  void setProtocol(Protocol * protocol);

  bool saveHealthInfo();
  bool restoreHealthInfo();

  // heart beat to the server to send status information for the device
  void healthUpdate(unsigned int loopDuration);
};

#endif /* __HEALTH_H__ */

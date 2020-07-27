#ifndef __HEALTH_H__
#define __HEALTH_H__

#include <unordered_map>
#include <string>
#include "globals.h"
#include "protocol.h"
#include "manage_wifi.h"
#include "json_helper.h"

/*
 * Health information to send:
 * - Number of times Protocol failed to send or connect
 * - Number of times failed to connect to Wifi
 * - WiFi SSID, RSSI and local IP
 * - Build number (?)
 * - MAC address
 * Information that needs to be collected:
 * - CPU frequency [ESP.getCpuFreqMHz()]
 * - Memory usage [ESP.getFreeHeap()]
 * - Device temperature [ESP.getVcc()]
 */

class HealthMonitor
{
private:
  const char health_channel[8];
  // String healthFile = "health.json";

  WifiManager * pDeviceWifi;
  Protocol* currentProtocol; // does this needs to be http?

  stringmap healthInfo;

  unsigned long last_time_health_send = 0;

  void collectHealthInfo();

public:
  HealthMonitor();
  HealthMonitor(WifiManager * deviceWifi);
  ~HealthMonitor();

  void setProtocol(Protocol * protocol);

  // heart beat to the server to send status information for the device
  void healthUpdate();
};

#endif /* __HEALTH_H__ */

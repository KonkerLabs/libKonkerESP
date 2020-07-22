#ifndef __HEALTH_H__
#define __HEALTH_H__

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

#include "globals.h"

class HealthMonitor: public BaseProtocol, public WifiManager
{
private:
  String health_channel = "_health";
  String healthFile="health.json";

  unsigned long last_time_health_send = 0;
  int _netFailureAdress=0;
  int _mqttFailureAdress=1;
  int _httpFailureAdress=2;
public:
  HealthMonitor();
  ~HealthMonitor();
  // heart beat to the server to send status information for the device
  void healthUpdate();
};

#endif /* __HEALTH_H__ */

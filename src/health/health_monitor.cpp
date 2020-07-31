#include "health_monitor.h"

HealthMonitor::HealthMonitor() : health_channel{'_', 'h', 'e', 'a', 'l', 't', 'h'}, httpObj()
{
  httpObj.setConnection("data.prod.konkerlabs.net", 80);
}

HealthMonitor::HealthMonitor(WifiManager * deviceWifi) : HealthMonitor()
{
  this->pDeviceWifi = deviceWifi;
}

HealthMonitor::~HealthMonitor()
{
  this->healthInfo.erase(this->healthInfo.begin(), this->healthInfo.end());
}

void HealthMonitor::setProtocol(Protocol *protocol)
{
  bool conn = false;

  this->pCurrentProtocol = protocol;
  httpObj.setPlatformCredentials(this->pCurrentProtocol->getUser(),
                                 this->pCurrentProtocol->getPassword());
  conn = httpObj.connect();
  if(conn)
  {
    Log.trace("[HMON] Connected to health channel\n");
  }
  else
  {
    Log.trace("[HMON] Could not connect to health channel\n");
  }
}

bool HealthMonitor::saveHealthInfo()
{
  health_conn_st_t info;
  uint8_t buffer[sizeof(health_conn_st_t)];

  memset(buffer, 0, sizeof(buffer));

  info.hfail = pDeviceWifi->numConnFail;
  info.mfail = pCurrentProtocol->getNumConnFail();
  info.hfail = httpObj.getNumConnFail();

  Log.trace("[HMON] Saving health information to EEPROM\n");
  memcpy(buffer, &info, sizeof(info));

  return deviceEEPROM.storeHealthInfo(buffer);;
}

bool HealthMonitor::restoreHealthInfo()
{
  int ret;
  health_conn_st_t info;
  uint8_t retBuffer[sizeof(health_conn_st_t)];

  memset(retBuffer, 0, sizeof(retBuffer));
  ret = deviceEEPROM.recoverHealthInfo(retBuffer);

  if(ret)
  {
    memcpy(&info, retBuffer, sizeof(info));

    pDeviceWifi->numConnFail = info.hfail;
    pCurrentProtocol->setNumConnFail(info.mfail);
    httpObj.setNumConnFail(info.hfail);
  }
  else
  {
    Log.warning("[PLAT] Could not restore information from memory!\n");
  }

  return true;
}

int HealthMonitor::pingPlatform()
{
  int ping_ms = 0;

  if(Ping.ping("konkerlabs.com"))
  {
    ping_ms = Ping.averageTime();
    // Log.trace("[HMON] Ping = %d\n", ping_ms);
  }

  return ping_ms;
}

void HealthMonitor::collectHealthInfo(unsigned int loopDuration)
{
  char ipBuffer[20];
  char intBuffer[5];

  this->healthInfo["build"] = std::string(PIO_SRC_REV);

  this->healthInfo["ssid"] = std::string(pDeviceWifi->getWifiSSID().c_str());
  sprintf(intBuffer, "%d", pDeviceWifi->getWifiStrenght());
  this->healthInfo["rssi"] = std::string(intBuffer);
  IPAddress ipBytes = pDeviceWifi->getLocalIP();
  sprintf(ipBuffer, "%d.%d.%d.%d", ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
  this->healthInfo["ip"] = std::string(ipBuffer);
  this->healthInfo["mac"] = std::string(pDeviceWifi->getMacAddress().c_str());

  sprintf(intBuffer, "%d", pDeviceWifi->numConnFail);
  this->healthInfo["nfail"] = std::string(intBuffer);
  sprintf(intBuffer, "%d", pCurrentProtocol->getNumConnFail());
  this->healthInfo["mfail"] = std::string(intBuffer);
  sprintf(intBuffer, "%d", httpObj.getNumConnFail());
  this->healthInfo["hfail"] = std::string(intBuffer);

  sprintf(intBuffer, "%d", pingPlatform());
  this->healthInfo["ping"] = std::string(intBuffer);

  // sprintf(intBuffer, "%u", ESP.getCpuFreqMHz());
  // this->healthInfo["cpu"] = std::string(intBuffer);
  sprintf(intBuffer, "%u", ESP.getFreeHeap());
  this->healthInfo["mem"] = std::string(intBuffer);
  sprintf(intBuffer, "%u", ESP.getVcc());
  this->healthInfo["vcc"] = std::string(intBuffer);
  sprintf(intBuffer, "%u", loopDuration);
  this->healthInfo["loop"] = std::string(intBuffer);

  Log.trace("[HMON] Information collected\n");
}

void HealthMonitor::healthUpdate(unsigned int loopDuration)
{
  //throtle this call at maximum 1 per minute
  if ((this->last_time_health_send !=0) &&
      ((millis()-this->last_time_health_send) < 60000))
  {
    return;
  }

  this->last_time_health_send = millis();

	char * payload;

	delay(10);
  Log.trace("[HMON] Collecting health information for device\n");
  collectHealthInfo(loopDuration);

  payload = jsonHelper.createMessage(&healthInfo);

  if(httpObj.checkConnection())
  {
    Log.trace("[HMON] Publishing on channel: %s\n", this->health_channel);
    Log.trace("[HMON] Message: %s\n", payload);
  }
  else
  {
    Log.notice("[HMON] Lost connection to health channel. Reconnecting\n");
    httpObj.increaseConnFail();
    httpObj.connect();
    // maybe try to reconnect here, or reset
  }
  httpObj.send(this->health_channel, String(payload));
}

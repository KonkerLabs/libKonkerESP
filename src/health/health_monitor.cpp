#include "health_monitor.h"

HealthMonitor::HealthMonitor() : health_channel{'_', 'h', 'e', 'a', 'l', 't', 'h'}
{
}

HealthMonitor::HealthMonitor(WifiManager * deviceWifi) : HealthMonitor()
{
  this->pDeviceWifi = deviceWifi;
}

HealthMonitor::~HealthMonitor()
{
  this->healthInfo.erase(this->healthInfo.begin(), this->healthInfo.end());
}

void HealthMonitor::setProtocol(Protocol *protocol, Protocol* httpProtocol)
{
  this->pCurrentProtocol = protocol;
  this->httpObj = static_cast<HTTPProtocol *>(httpProtocol);
  // httpObj.setPlatformCredentials(this->pCurrentProtocol->getUser(),
  //                                this->pCurrentProtocol->getPassword());
  // conn = httpObj.connect();
  if(this->httpObj->checkConnection())
  {
    Log.trace("[HMON] Connected to health channel\n");
  }
  else
  {
    Log.trace("[HMON] Not connect to health channel\n");
  }
}

bool HealthMonitor::saveHealthInfo()
{
  health_conn_st_t info;
  uint8_t buffer[sizeof(health_conn_st_t)];

  memset(buffer, 0, sizeof(buffer));

  info.hfail = pDeviceWifi->numConnFail;
  info.mfail = pCurrentProtocol->getNumConnFail();
  info.hfail = this->httpObj->getNumConnFail();

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
    this->httpObj->setNumConnFail(info.hfail);
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

#ifdef LOCAL_BUILD
  if(Ping.ping("192.168.1.123"))
#else
  if(Ping.ping("konkerlabs.com"))
#endif
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
  sprintf(intBuffer, "%d", this->httpObj->getNumConnFail());
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

  if(this->httpObj->checkConnection())
  {
    Log.trace("[HMON] Publishing on channel: %s\n", this->health_channel);
    Log.trace("[HMON] Message: %s\n", payload);
  }
  else
  {
    Log.notice("[HMON] Lost connection to health channel. Reconnecting\n");
    this->httpObj->increaseConnFail();
    this->httpObj->connect();
    // maybe try to reconnect here, or reset
  }
  this->httpObj->send(this->health_channel, String(payload));
}

void HealthMonitor::collectDeviceInfo(stringmap * info)
{
  char intBuffer[5];
  auto& map = *info;

  // Log.trace("[HMON] info is %d\n", info->empty());
  sprintf(intBuffer, "%d", pDeviceWifi->getWifiStrenght());
  map["rssi"] = std::string(intBuffer);
  // Log.trace("[HMON] %d elements: %s\n", info->size(), intBuffer);
  sprintf(intBuffer, "%u", ESP.getFreeHeap());
  map["mem"] = std::string(intBuffer);
  // Log.trace("[HMON] %d elements: %s\n", info->size(), intBuffer);
  sprintf(intBuffer, "%u", ESP.getVcc());
  map["vcc"] = std::string(intBuffer);
  // Log.trace("[HMON] %d elements: %s\n", info->size(), intBuffer);

  Log.trace("[HMON] Device information collected\n");
}

bool HealthMonitor::collectDeviceStatus(int stageKey)
{
  stringmap deviceInfo;

  Log.trace("[HMON] Collecting device usage information\n");
  collectDeviceInfo(&deviceInfo);

  if(jsonHelper.addInfoObject(stageKey, &deviceInfo))
  {
    Log.trace("[HMON] Device usage information stored\n");
    // printDeviceStatus();
    return true;
  }
  // else if(stageKey == 4 && jsonHelper.mergeInfo(stageKey, &deviceInfo))
  // {
  //   Log.trace("[HMON] Device usage information merged\n");
  //   printDeviceStatus();
  //   return true;
  // }
  return false;
}

bool HealthMonitor::saveDeviceStatus()
{
  return jsonHelper.saveStatusInfo();
}

bool HealthMonitor::recoverDeviceStatus()
{
  bool ret = jsonHelper.loadStatusInfo();
  if(ret)
  {
    Log.trace("[HMON] Device usage information recovered from memory\n");
    printDeviceStatus();
  }
  else
  {
    Log.warning("[HMON] Could not recover device usage information from memory\n");
  }
  
  return ret;
}

void HealthMonitor::getDeviceStatusCollected(char * buffer)
{
  jsonHelper.getStatusCollected(buffer);
}

void HealthMonitor::printDeviceStatus()
{
  jsonHelper.printStatus();
}

void HealthMonitor::printStatusAddresses()
{
  jsonHelper.printAddresses();
}

void HealthMonitor::clearDeviceStatus()
{
  jsonHelper.clearInfo();
}

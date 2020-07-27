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

void HealthMonitor::setProtocol(Protocol *protocol)
{
  this->currentProtocol = protocol;
}

void HealthMonitor::collectHealthInfo()
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
  sprintf(intBuffer, "%d", currentProtocol->getNumConnFail());
  this->healthInfo["pfail"] = std::string(intBuffer);

  Log.trace("[HMON] Information collected\n");
}

void HealthMonitor::healthUpdate()
{
  if ((this->last_time_health_send !=0) &&
      ((millis()-this->last_time_health_send) < 60000))
  {
    //throtle this call at maximum 1 per minute
    return;
  }

  this->last_time_health_send = millis();

	char * payload;

	delay(10);
  Log.trace("[HMON] Collecting health information for device\n");
  collectHealthInfo();

  payload = jsonHelper.createMessage(&healthInfo);
  Log.trace("[HMON] Publishing on channel: %s\n", this->health_channel);
  Log.trace("[HON] Message: %s\n", payload);

  currentProtocol->send(this->health_channel, String(payload));

  // if(nf==0 && mf==0 && hf==0)
  // {
  //   return;
  // }
  //clear error flags
  // saveFile(healthFile,(char*)"000");
}

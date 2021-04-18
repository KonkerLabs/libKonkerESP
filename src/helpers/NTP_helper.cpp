#include "NTP_helper.h"

#ifdef LOCAL_BUILD
NTPHelper::NTPHelper() : timeClient(ntpUDP, "192.168.1.123")
#else
NTPHelper::NTPHelper() : timeClient(ntpUDP, "br.pool.ntp.org")
#endif
{
}

NTPHelper::~NTPHelper()
{
  timeClient.end();
}

void NTPHelper::startNTP()
{
  timeClient.begin();
  Log.trace("[NTP] Client started\n");
}

void NTPHelper::updateNTP()
{
  Log.trace("[NTP] Updating client\n");
  timeClient.updateMs();
}

void NTPHelper::getTimeNTP(char * timestamp)
{
  unsigned long r;
  unsigned int ms;

  r = timeClient.getEpochTimeMs(&ms);
  // Log.trace("From NTPClient [long uint]: %l\n", r);
  sprintf(timestamp, "%lu%03u", r, ms);
  // Log.trace("From NTPClient: %s\n", timestamp);
}

#include "NTP_helper.h"

NTPHelper::NTPHelper() : timeClient(ntpUDP, "br.pool.ntp.org")
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
  sprintf(timestamp, "%lu%u", r, ms);
  // Log.trace("From NTPClient: %s\n", timestamp);
}

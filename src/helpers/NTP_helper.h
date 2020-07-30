#ifndef _NTPHELPER_H_
#define _NTPHELPER_H_

#include <WiFiUdp.h>
#include <NTPClient.h>
#include "globals.h"

class NTPHelper
{
private:
  WiFiUDP ntpUDP;
  NTPClient timeClient;
public:
  NTPHelper();
  ~NTPHelper();

  void startNTP();
  void updateNTP();
  void getTimeNTP(char * timestamp);
};

#endif /* _NTPHELPER_H_ */

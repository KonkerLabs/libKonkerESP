#ifndef __EEPROM_MANAGER_H__
#define __EEPROM_MANAGER_H__

/*
|--------------|-------|---------------|--|--|--|--|--|
^              ^       ^               ^     ^
Sketch    OTA update   File system   >EEPROM<  WiFi config (SDK)
*/

#include <EEPROM.h>
#include "globals.h"

#define MAX_NUM_WIFI_CRED     3
#define WIFI_CRED_INIT_ADDR   0
#define WIFI_CRED_SIZE_BYTES  34    //total size = 34 bytes x 3
#define PLAT_CRED_INIT_ADDR   1024
#define PLAT_CRED_SIZE_BYTES  50

#define ENABLED_PATTERN       0xABCDu

class EEPROMManager
{
private:
  EEPROMClass mem_cred;
  int wifiCredSizeBytes;
  int platCredSizeBytes;

  uint16_t crc_16 (uint8_t *buffer, unsigned int length);

  void clearWiFiCredential(uint8_t * buffer, int credIndex);
  void clearPlatformCredential(uint8_t * buffer);

  int write(uint8_t * buffer, unsigned int size, unsigned int initAddr);
  int read(uint8_t * retBuffer, unsigned int size, unsigned int initAddr);
  void printBuffer(uint8_t * buffer, int size);

public:
  EEPROMManager();
  EEPROMManager(int wifiCredSizeBytes, int platCredSizeBytes);

  ~EEPROMManager();

  void begin();

  int clearAll();

  int storeWifiCredentials(uint8_t * wifiBuffer);
  int recoverWifiCredentials(uint8_t * retBuffer);

  int storePlatformCredentials(uint8_t * platBuffer);
  int recoverPlatformCredentials(uint8_t * retBuffer);
};

extern EEPROMManager deviceEEPROM;

#endif /* __EEPROM_MANAGER_H__ */

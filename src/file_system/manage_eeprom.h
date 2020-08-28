#ifndef __EEPROM_MANAGER_H__
#define __EEPROM_MANAGER_H__

/* Flash layout, from ESP8266 Arduino Core docs
|--------------|-------|---------------|--|--|--|--|--|
^              ^       ^               ^     ^
Sketch    OTA update   File system   >EEPROM<  WiFi config (SDK)
*/

#include <EEPROM.h>
#include "globals.h"

#define MAX_NUM_WIFI_CRED     3
#define WIFI_CRED_INIT_ADDR   0
#define WIFI_CRED_SIZE_BYTES  34    //total size = 34 bytes x 3
//#define PLAT_CRED_INIT_ADDR   512
#define PLAT_CRED_INIT_ADDR   WIFI_CRED_SIZE_BYTES * MAX_NUM_WIFI_CRED + 3
#define PLAT_CRED_SIZE_BYTES  74
//#define HLTH_INFO_INIT_ADDR   1024
#define HLTH_INFO_INIT_ADDR   PLAT_CRED_INIT_ADDR + PLAT_CRED_SIZE_BYTES + 3
#define HLTH_INFO_SIZE_BYTES  6
#define BOOT_INFO_SIZE_BYTES  1
#define BOOT_INFO_INIT_ADDR   HLTH_INFO_INIT_ADDR + HLTH_INFO_SIZE_BYTES + 3

#define ENABLED_PATTERN       0xABCDu

class EEPROMManager
{
private:
  EEPROMClass mem_cred;

  uint16_t crc_16(uint8_t *buffer, unsigned int length);

  void clearWiFiCredential(uint8_t * buffer, int credIndex);
  void clearPlatformCredential(uint8_t * buffer);

  int write(uint8_t * buffer, unsigned int size, unsigned int initAddr);
  int read(uint8_t * retBuffer, unsigned int size, unsigned int initAddr);
  void printBuffer(uint8_t * buffer, int size);

public:
  EEPROMManager();

  ~EEPROMManager();

  void begin();

  int clearAll();

  int storeWifiCredentials(uint8_t * wifiBuffer);
  int recoverWifiCredentials(uint8_t * retBuffer);

  int storePlatformCredentials(uint8_t * platBuffer);
  int recoverPlatformCredentials(uint8_t * retBuffer);

  int storeHealthInfo(uint8_t * infoBuffer);
  int recoverHealthInfo(uint8_t * retBuffer);

  int storeBootInfo(uint8_t firstBoot);
  int recoverBootInfo(uint8_t * firstBootRet);
};

extern EEPROMManager deviceEEPROM;

#endif /* __EEPROM_MANAGER_H__ */

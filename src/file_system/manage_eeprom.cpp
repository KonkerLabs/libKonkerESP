#include "manage_eeprom.h"

EEPROMManager::EEPROMManager()
{
  this->wifiCredSizeBytes = WIFI_CRED_SIZE_BYTES;
  this->platCredSizeBytes = PLAT_CRED_SIZE_BYTES;

  mem_cred.begin(4096);
}

EEPROMManager::EEPROMManager(int wifiCredSizeBytes, int platCredSizeBytes)
{
  this->wifiCredSizeBytes = wifiCredSizeBytes;
  this->platCredSizeBytes = platCredSizeBytes;

  mem_cred.begin(4096);
}

EEPROMManager::~EEPROMManager()
{
  mem_cred.end();
}

void EEPROMManager::begin()
{
  // recoverWifiCredentials();
  // recoverPlatformCredentials();
}

int EEPROMManager::clearAll()
{
  for (unsigned int i = 0; i < mem_cred.length(); ++i)
  {
    mem_cred.write(i, 0x00u);
  }

  return mem_cred.commit() ? 1 : 0;
}

int EEPROMManager::write(uint8_t * buffer, unsigned int size, unsigned int initAddr)
{
  unsigned int endAddr = size + initAddr;
  uint8_t * bufferPtr = buffer;

  Log.trace("[FS] Writing %d bytes @ %d\n", size, initAddr);
  for (unsigned int i = initAddr; i < endAddr; ++i)
  {
    // Log.trace("[FS] Writing byte %X @ %d\n", *bufferPtr, i);
    mem_cred.write(i, *bufferPtr++);
  }

  uint16_t crc = crc_16(buffer, size);
  mem_cred.write(endAddr, crc >> 8);
  mem_cred.write(endAddr+1, crc & 0xFFu);

  printBuffer(buffer, size);
  Log.trace("[FS] crc_store = %X\n", crc);

  return mem_cred.commit() ? 1 : 0;
}

int EEPROMManager::storeWifiCredentials(uint8_t * wifiBuffer)
{
  uint16_t enabled = 0;
  int ret = 0;

  for (int i = 0; i < MAX_NUM_WIFI_CRED; ++i)
  {
    enabled = wifiBuffer[i * WIFI_CRED_SIZE_BYTES + 1] << 8 | wifiBuffer[i * WIFI_CRED_SIZE_BYTES];
    Log.trace("[FS] Enabled = %x\n", enabled);
    if (enabled != ENABLED_PATTERN)
    {
      clearWiFiCredential(wifiBuffer, i);
    }
  }

  // Log.trace("[FS] Buffer address: %X\n", wifiBuffer);
  ret = write(wifiBuffer, WIFI_CRED_SIZE_BYTES * MAX_NUM_WIFI_CRED, WIFI_CRED_INIT_ADDR);
  Log.trace("[FS] Return = %d\n", ret);

  return ret;
}

int EEPROMManager::storePlatformCredentials(uint8_t * platBuffer)
{
  uint16_t enabled = 0;

  enabled = platBuffer[1] << 8 | platBuffer[0];
  Log.trace("[FS] Enabled = %x\n", enabled);
  if (enabled != ENABLED_PATTERN)
  {
    clearPlatformCredential(platBuffer);
  }

  // Log.trace("[FS] Buffer address: %X\n", platBuffer);
  return write(platBuffer, PLAT_CRED_SIZE_BYTES, PLAT_CRED_INIT_ADDR);
}

int EEPROMManager::read(uint8_t * retBuffer, unsigned int size, unsigned int initAddr)
{
  unsigned int endAddr = size + initAddr;
  uint8_t * bufferPtr = retBuffer;
  int ret = 1;

  for (unsigned int i = initAddr; i < endAddr; ++i)
  {
    *bufferPtr = mem_cred.read(i);
    bufferPtr++;
  }

  uint16_t stored_crc = mem_cred.read(endAddr) << 8 | mem_cred.read(endAddr+1);

  printBuffer(retBuffer, size);
  Log.trace("[FS] stored_crc = %x\n", stored_crc);
  Log.trace("[FS] read_crc = %x\n", crc_16(retBuffer, size));

  if (stored_crc != crc_16(retBuffer, size))
  {
    ret = 0;
  }

  return ret;
}

int EEPROMManager::recoverWifiCredentials(uint8_t * retBuffer)
{
  int ret = 0;

  ret = read(retBuffer, WIFI_CRED_SIZE_BYTES * MAX_NUM_WIFI_CRED, WIFI_CRED_INIT_ADDR);

  return ret;
}

int EEPROMManager::recoverPlatformCredentials(uint8_t * retBuffer)
{
  int ret = 0;

  ret = read(retBuffer, PLAT_CRED_SIZE_BYTES, PLAT_CRED_INIT_ADDR);

  return ret;
}


uint16_t EEPROMManager::crc_16(uint8_t *buffer, unsigned int length)
{
  uint16_t temp_bit, temp_int, crc;
  crc = 0xFFFF;

  for (unsigned int i = 0; i < length; i++)
  {
    temp_int = (uint8_t) *buffer++;
    crc ^= temp_int;
    for (int j = 0; j < 8; j++)
    {
      temp_bit = crc & 0x0001;
      crc >>= 1;
      if (temp_bit != 0)
        crc ^= 0xA001;
    }
  }

  return crc;
}

void EEPROMManager::clearWiFiCredential(uint8_t * buffer, int credIndex)
{
  Log.trace("[FS] Cleaning credential %d\n", credIndex);
  memset(&buffer[credIndex * this->wifiCredSizeBytes], 0xFFu, this->wifiCredSizeBytes);
}

void EEPROMManager::clearPlatformCredential(uint8_t * buffer)
{
  Log.trace("[FS] Cleaning credential\n");
  memset(buffer, 0xFFu, this->platCredSizeBytes);
}

void EEPROMManager::printBuffer(uint8_t * buffer, int size)
{
  Log.trace("[FS] Buffer: %X\n>", buffer);
  if(DEBUG_LEVEL > 0)
  {
    for (int i=0; i< size; i++)
    {
      Serial.print(" 0x");
      Serial.print(buffer[i], HEX);
    }
    Serial.println(" <");
  }
}

EEPROMManager deviceEEPROM;

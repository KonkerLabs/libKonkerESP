#include "manage_platform.h"

PlatformManager::PlatformManager()
{
  // Log.trace("[PLAT] Initializing\n");
}

PlatformManager::~PlatformManager()
{
  // Log.trace("[PLAT] Initializing\n");
}

void PlatformManager::setServer(String host, int port)
{
  int size;

  size = host.length();

  // Remove "http://" from host, if needed
  if(host.indexOf("http://") > 0)
  {
    size = size - 7;
    host.remove(0, 7);
  }

  if(size > PLAT_ADDR_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! Host size too big!\n");
    size = PLAT_ADDR_ARRAY_SIZE;
  }

  strncpy(platformAddress.host, host.c_str(), size);
  platformAddress.host[size] = '\0';
  platformAddress.port = port;

  Log.trace("[PLAT] Setting %s:%d as platform address\n", platformAddress.host, platformAddress.port);
}

String PlatformManager::getHost()
{
  return String(platformAddress.host);
}

int PlatformManager::getPort()
{
  return platformAddress.port;
}

void PlatformManager::setPlatformCredentials(String user, String password)
{
  int size;

  size = user.length();
  if(size > PLAT_CRED_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! User size too big!\n");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(PlatformManager::platformCredential.user, user.c_str(), size);
  PlatformManager::platformCredential.user[size] = '\0';

  size = password.length();
  if(size > PLAT_CRED_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! Password size too big!\n");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(PlatformManager::platformCredential.passwd, password.c_str(), size);
  PlatformManager::platformCredential.passwd[size] = '\0';

  PlatformManager::platformCredential.enabled = ENABLED_PATTERN;

  Log.trace("[PLAT] Setting %s<>%s as platform credential\n", PlatformManager::platformCredential.user, PlatformManager::platformCredential.passwd);

  this->credentialSet = true;
}

String PlatformManager::getUser()
{
  return String(PlatformManager::platformCredential.user);
}

String PlatformManager::getPassword()
{
  return String(PlatformManager::platformCredential.passwd);
}

bool PlatformManager::isCredentialSet()
{
  return this->credentialSet;
}

void PlatformManager::setCredentialStatus(bool status)
{
  this->credentialSet = status;
}

int PlatformManager::savePlatformCredentials()
{
  uint8_t buffer[sizeof(plat_cred_st_t)];

  memset(buffer, 0, sizeof(buffer));

  Log.trace("[PLAT] Saving platform credentials to EEPROM\n");
  memcpy(buffer, &this->platformCredential, sizeof(this->platformCredential));
  // Log.trace("[PLAT] Buffer {%X} | cred={%X} %d bytes\n>", buffer, &this->platformCredential, sizeof(plat_cred_st_t));
  // for (unsigned int i=0; i< sizeof(buffer); i++)
  // {
  //   Serial.print(" 0x");
  //   Serial.print(buffer[i], HEX);
  // }
  // Serial.println(" <");

  return deviceEEPROM.storePlatformCredentials(buffer);
}

int PlatformManager::restorePlatformCredentials()
{
  uint8_t retBuffer[sizeof(plat_cred_st_t)];
  plat_cred_st_t temp_cred;
  int ret;

  memset(retBuffer, 0, sizeof(retBuffer));
  ret = deviceEEPROM.recoverPlatformCredentials(retBuffer);

  if (ret)
  {
    memcpy(&temp_cred, retBuffer, sizeof(temp_cred));

    if(temp_cred.enabled == ENABLED_PATTERN)
    {
      Log.trace("[PLAT] Setting %s<>%s as platform credential\n", temp_cred.user, temp_cred.passwd);

      this->platformCredential.enabled = ENABLED_PATTERN;
      strcpy(platformCredential.user, temp_cred.user);
      strcpy(platformCredential.passwd, temp_cred.passwd);
      this->credentialSet = true;
    }
  }
  else
  {
    this->credentialSet = false;
    Log.warning("[PLAT] Could not restore credentials from memory!\n");
  }

  return ret;
}

// initialize platformCredential
struct plat_cred_st_t PlatformManager::platformCredential{0x0000u, '\0', '\0'};

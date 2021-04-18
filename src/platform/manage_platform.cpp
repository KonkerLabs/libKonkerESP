#include "manage_platform.h"

PlatformManager::PlatformManager()
{
  // Log.trace("[PLAT] Initializing\n");
  this->credentialSet = false;
}

PlatformManager::~PlatformManager()
{
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
  this->platformAddress.host[size] = '\0';
  this->platformAddress.port = port;

  Log.trace("[PLAT] Setting %s:%d as platform address\n", this->platformAddress.host, this->platformAddress.port);
}

String PlatformManager::getHost()
{
  return String(this->platformAddress.host);
}

int PlatformManager::getPort()
{
  return this->platformAddress.port;
}

void PlatformManager::setDeviceId(String id)
{
  int size;

  size = id.length();
  if(size > PLAT_CRED_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! ID size too big!\n");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(this->platformCredential.id, id.c_str(), size);
  this->platformCredential.id[size] = '\0';
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
  strncpy(this->platformCredential.user, user.c_str(), size);
  this->platformCredential.user[size] = '\0';

  size = password.length();
  if(size > PLAT_CRED_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! Password size too big!\n");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(this->platformCredential.passwd, password.c_str(), size);
  this->platformCredential.passwd[size] = '\0';

  this->platformCredential.enabled = ENABLED_PATTERN;
  this->credentialSet = true;

  Log.trace("[PLAT] Setting %s<>%s as platform credential (%d)\n", this->platformCredential.user, this->platformCredential.passwd, this->credentialSet);
}

String PlatformManager::getUser()
{
  return String(this->platformCredential.user);
}

String PlatformManager::getPassword()
{
  return String(this->platformCredential.passwd);
}

String PlatformManager::getDeviceId()
{
  return String(this->platformCredential.id);
}

bool PlatformManager::isCredentialSet()
{
  return this->credentialSet;
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
      strcpy(this->platformCredential.id, temp_cred.id);
      strcpy(this->platformCredential.user, temp_cred.user);
      strcpy(this->platformCredential.passwd, temp_cred.passwd);
      this->credentialSet = true;
    }
    else
    {
      Log.notice("[PLAT] Credentials recovered but not set. Enabled = %X\n", temp_cred.enabled);
      this->credentialSet = false;
      ret = false;
    }
  }
  else
  {
    Log.warning("[PLAT] Could not restore credentials from memory!\n");
    this->credentialSet = false;
  }

  return ret;
}

// initialize platformCredential
struct plat_cred_st_t PlatformManager::platformCredential{0x0000u, '\0', '\0'};

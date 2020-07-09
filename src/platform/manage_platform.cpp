#include "manage_platform.h"

PlatformManager::PlatformManager()
{

}

void PlatformManager::setServer(String host, int port)
{
  int size;

  size = host.length();

  if(size > PLAT_ADDR_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! Host size too big!");
    size = PLAT_ADDR_ARRAY_SIZE;
  }

  strncpy(platformAddress.host, host.c_str(), size);
  platformAddress.port = port;
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
    Log.warning("[PLAT] Caution! User size too big!");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(platformCredential.user, user.c_str(), size);

  size = password.length();
  if(size > PLAT_CRED_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! Password size too big!");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(platformCredential.passwd, password.c_str(), size);
}

String PlatformManager::getUser()
{
  return String(platformCredential.user);
}

String PlatformManager::getPassword()
{
  return String(platformCredential.passwd);
}

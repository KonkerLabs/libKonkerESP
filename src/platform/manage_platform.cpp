#include "manage_platform.h"

PlatformManager::PlatformManager()
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
  strncpy(platformCredential.user, user.c_str(), size);
  platformCredential.user[size] = '\0';

  size = password.length();
  if(size > PLAT_CRED_ARRAY_SIZE)
  {
    Log.warning("[PLAT] Caution! Password size too big!\n");
    size = PLAT_CRED_ARRAY_SIZE;
  }
  strncpy(platformCredential.passwd, password.c_str(), size);
  platformCredential.passwd[size] = '\0';

  Log.trace("[PLAT] Setting %s<>%s as platform credential\n", platformCredential.user, platformCredential.passwd);
}

String PlatformManager::getUser()
{
  return String(platformCredential.user);
}

String PlatformManager::getPassword()
{
  return String(platformCredential.passwd);
}

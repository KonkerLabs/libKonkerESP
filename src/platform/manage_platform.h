#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "globals.h"

#define PLAT_ADDR_ARRAY_SIZE   36
#define PLAT_CRED_ARRAY_SIZE   24

struct plat_addr_st_t
{
  uint16_t enabled;
  char host[PLAT_ADDR_ARRAY_SIZE];
  int port;
};

struct plat_cred_st_t
{
  uint16_t enabled; // ???
  char user[PLAT_CRED_ARRAY_SIZE];
  char passwd[PLAT_CRED_ARRAY_SIZE];
};

class PlatformManager
{
private:

  plat_addr_st_t platformAddress;
  plat_cred_st_t platformCredential;

public:
  PlatformManager();
  ~PlatformManager();

  void setServer(String host, int port);
  String getHost();
  int getPort();

  void setPlatformCredentials(String user, String password);
  String getUser();
  String getPassword();
  // TODO
  int storePlatformCredentials();
  int restorePlatofrmCredentials();
};

#endif /* __PLATFORM_H__ */

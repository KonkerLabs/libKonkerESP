#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "globals.h"
#include "manage_eeprom.h"
//note: deviceEEPROM comes from manage_eeprom.h

#define PLAT_ADDR_ARRAY_SIZE   36
#define PLAT_CRED_ARRAY_SIZE   24

struct plat_addr_st_t
{
  char host[PLAT_ADDR_ARRAY_SIZE];
  int port;
};

// 74 bytes
struct plat_cred_st_t
{
  uint16_t enabled;
  char id[PLAT_CRED_ARRAY_SIZE];
  char user[PLAT_CRED_ARRAY_SIZE];
  char passwd[PLAT_CRED_ARRAY_SIZE];
};

class PlatformManager
{
private:
  plat_addr_st_t platformAddress;
  static plat_cred_st_t platformCredential;
  bool credentialSet;

public:

  PlatformManager();
  ~PlatformManager();

  void setServer(String host, int port);
  String getHost();
  int getPort();

  void setDeviceId(String id);
  void setPlatformCredentials(String user, String password);
  bool isPlatformCredsEnabled();
  String getUser();
  String getPassword();
  String getDeviceId();

  bool isCredentialSet();
  int savePlatformCredentials();
  // overwrites existing credentials (if any)
  int restorePlatformCredentials();
};

#endif /* __PLATFORM_H__ */

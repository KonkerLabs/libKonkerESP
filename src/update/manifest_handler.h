#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include "globals.h"
#include "json_helper.h"

struct fw_info
{
  char version[6];
  char seqNumber[14];
  unsigned int size;
  char deviceType[16];
  time_t expirationDate;
  char author[10];
  char * signature;
  char * keyClaims;
  char ckSum[36];
};

class ManifestHandler
{
private:
  bool valid;
  fw_info newFwInfo;

public:
  ManifestHandler();

  //from update_rpi3
  // compareVersions()
  int updateFwInfo(fw_info newInfo);
  bool checkDependencies();
  bool checkDevice(char * deviceType);
  bool checkMemory(int fwSize);
  bool checkMinVersion(char * reqVersion);
  bool checkPermissions(char * author);
  bool checkSeqNumber(char * newSeqNumber);
  bool checkSignature();
  bool checkVendor();
  bool checkVersion(char * newVersion);
  bool checkVersionList();
  bool checkChecksum();

  StaticJsonDocument * parseManifest(String * manifest);
  int validateManiest();
  int applyManifest();
};

#endif /* __MANIFEST_H__ */

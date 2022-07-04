#ifndef __MANIFEST_H__
#define __MANIFEST_H__

#include "globals.h"
#include "json_helper.h"

#define INFO_LOADED 0xABCDu

// 36 bytes
struct fw_info_t
{
  uint16_t loaded;
  char version[9]; //mem
  char deviceID[9]; //mem
  char seqNumber[16]; //mem
};

// [MJ] all optional fields here?
struct manifest_info_t
{
  fw_info_t essentialInfo;
  unsigned int size;
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
  manifest_info_t newFwInfo;
  fw_info_t currentFwInfo;
  DynamicJsonDocument manifestJson;

  // returns true if new > old
  // version format must be XX.XX.XX
  bool compareVersions(const char * newVersion, const char * oldVersion);
  bool checkVersion(const char * newVersion);
  bool checkDevice(const char * deviceID);
  bool checkSeqNumber(const char * newSeqNumber);
  bool checkKeyClaims(const char * key_claims);
  bool checkSignature(const char * signature);
  bool checkVendor(const char * vendor);
  bool checkMemory(const int size);
  bool checkPermissions(const char * author);
  bool checkMinVersion(const char * reqVersion);
  bool checkVersionList(const JsonArray versionList);
  bool checkDependencies();

  bool validateRequiredFields();
  bool validateOptionalFields();

  //from update_rpi3
  void updateFwInfo();
  bool saveNewFwInfo();

public:
  ManifestHandler();
  ~ManifestHandler();

  bool startHandler(fw_info_t * currentInfo);

  bool parseManifest(const char * manifest);
  bool validateManifest();
  bool checkChecksum(String md5recv);
  int applyManifest();

  bool updateCurrentFwInfo(fw_info_t *);

  char * getCurrentVersion();
  char * getNewVersion();
  char * getMd5();
};

#endif /* __MANIFEST_H__ */

#include "manifest_handler.h"

ManifestHandler::ManifestHandler() : manifestJson(1024)
{
  this->valid = false;
  memset(&newFwInfo, 0, sizeof(fw_info_t));
  memset(&currentFwInfo, 0, sizeof(fw_info_t));
  // jsonHelper.saveCurrentFwInfo();
}

ManifestHandler::~ManifestHandler()
{
  Log.trace("[MNFT] Unloading ManifestHandler\n");
  // free(currentFwInfo.signature);
  // free(currentFwInfo.keyClaims);
}

bool ManifestHandler::startHandler(fw_info_t * currentInfo)
{
  DynamicJsonDocument currentFwInfoJson(1024);
  Log.trace("[MNFT] Loading current FW info\n");

  if(currentInfo->loaded == INFO_LOADED)
  {
    Log.trace("[MNFT] Already loaded from flash\n");
    memcpy(&currentFwInfo, currentInfo, sizeof(fw_info_t));
    return true;
  }

  if(jsonHelper.loadCurrentFwInfo(&currentFwInfoJson))
  {
    strcpy(currentFwInfo.version, currentFwInfoJson["version"]);
    strcpy(currentFwInfo.deviceID, currentFwInfoJson["device"]);
    strcpy(currentFwInfo.seqNumber, currentFwInfoJson["sequence_number"]);
    currentFwInfo.loaded = INFO_LOADED;

    Log.trace("[MNFT] Information read: %s / %s / %s / %X\n", currentFwInfo.version, currentFwInfo.deviceID, currentFwInfo.seqNumber, currentFwInfo.loaded);

    memcpy(currentInfo, &currentFwInfo, sizeof(fw_info_t));
    return true;
  }

  return false;
}

bool ManifestHandler::parseManifest(const char *manifest)
{
  Log.trace("[MNFT] Received = %s\n", manifest);
  // Log.trace("[MNFT] JSON memory usage: %d; size: %d\n", this->manifestJson.memoryUsage(), this->manifestJson.size());
  DeserializationError err = deserializeJson(this->manifestJson, manifest);

  if (err)
  {
    Log.notice("[MNFT] Failed to desirialize json document. Code = %s\n", err.c_str());
    return false;
  }

  return true;
}

bool ManifestHandler::validateManifest()
{
  if (currentFwInfo.loaded != INFO_LOADED)
  {
    Log.trace("[MNFT] Current FW info not loaded! Please call ManifestHandler::startHandler\n");
  }

  // Log.trace("[MNFT] JSON memory usage: %d; size: %d\n", this->manifestJson.memoryUsage(), this->manifestJson.size());
  bool ret = validateRequiredFields();
  ret = ret && validateOptionalFields();

  // obs: this does not free memory, just clear fields
  manifestJson.clear();

  return ret;
}

int ManifestHandler::applyManifest()
{
  int ret = 0;
  ret = this->saveNewFwInfo();
  // TODO check addtional steps and perform them

  if(ret)
  {
    this->updateFwInfo();
  }

  return ret;
}

bool ManifestHandler::saveNewFwInfo()
{
  DynamicJsonDocument newFwInfoJson(1024);
  char newFwInfoBuffer[1024];

  Log.trace("[MNFT] Saving new firmware information to memory\n");
  newFwInfoJson["version"] = this->newFwInfo.essentialInfo.version;
  newFwInfoJson["device"] = this->newFwInfo.essentialInfo.deviceID;
  newFwInfoJson["sequence_number"] = this->newFwInfo.essentialInfo.seqNumber;

  serializeJson(newFwInfoJson, newFwInfoBuffer);
  return jsonHelper.saveCurrentFwInfo(newFwInfoBuffer);
}

void ManifestHandler::updateFwInfo()
{
  // free(currentFwInfo.signature);
  // free(currentFwInfo.keyClaims);

  memcpy(&currentFwInfo, &newFwInfo.essentialInfo, sizeof(fw_info_t));
}

bool ManifestHandler::updateCurrentFwInfo(fw_info_t * info)
{
  DynamicJsonDocument infoJson(1024);
  char infoBuffer[1024];

  Log.trace("[MNFT] Updating current firmware information in memory\n");
  infoJson["version"] = info->version;
  infoJson["device"] = info->deviceID;
  infoJson["sequence_number"] = info->seqNumber;

  serializeJson(infoJson, infoBuffer);
  Log.trace("[MNFT] Information: %s\n", infoBuffer);
  return jsonHelper.saveCurrentFwInfo(infoBuffer);
}

char * ManifestHandler::getCurrentVersion()
{
  return currentFwInfo.version;
}

char * ManifestHandler::getNewVersion()
{
  return newFwInfo.essentialInfo.version;
}

char * ManifestHandler::getMd5()
{
  return newFwInfo.ckSum;
}

bool ManifestHandler::validateRequiredFields()
{
  this->valid = true;
  Log.trace("[MNFT] Validating required fields\n");

  if (manifestJson.containsKey("version"))
  {
    if (checkVersion(manifestJson["version"]))
    {
      strcpy(newFwInfo.essentialInfo.version, manifestJson["version"]);
      Log.trace("[MNFT] New version: %s\n", newFwInfo.essentialInfo.version);
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Older version! Current = %s\n", currentFwInfo.version);
      this->valid = false;
    }
  }
  else
  {
    Log.error("[MNFT] Missing required key: version\n");
    this->valid = false;
  }

  if (manifestJson.containsKey("device"))
  {
    if (checkDevice(manifestJson["device"]))
    {
      strcpy(newFwInfo.essentialInfo.deviceID, manifestJson["device"]);
      Log.trace("[MNFT] Update for device: %s\n", newFwInfo.essentialInfo.deviceID);
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Update for different device! Current = %s\n", currentFwInfo.deviceID);
      this->valid = false;
    }
  }
  else
  {
    Log.error("[MNFT] Missing required key: device\n");
    this->valid = false;
  }

  if (manifestJson.containsKey("sequence_number"))
  {
    if (checkSeqNumber(manifestJson["sequence_number"]))
    {
      strcpy(newFwInfo.essentialInfo.seqNumber, manifestJson["sequence_number"]);
      Log.trace("[MNFT] New sequence number: %s\n", newFwInfo.essentialInfo.seqNumber);
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Older version! Current = %s\n", currentFwInfo.seqNumber);
      this->valid = false;
    }
  }
  else
  {
    Log.error("[MNFT] Missing required key: sequence_number\n");
    this->valid = false;
  }

  if (manifestJson.containsKey("key_claims"))
  {
    if (checkKeyClaims(manifestJson["key_claims"]))
    {
      Log.notice("[MNFT] Received key claims!\n");
      newFwInfo.keyClaims = nullptr; //malloc(manifestJson["key_claims"]); ???
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Invalid key claims!\n");
      this->valid = false;
    }
  }
  else
  {
    Log.error("[MNFT] Missing required key: key_claims\n");
    this->valid = false;
  }

  if (manifestJson.containsKey("digital_signature"))
  {
    if (checkSignature(manifestJson["digital_signature"]))
    {
      char temp[32];
      strcpy(temp, manifestJson["digital_signature"]);
      Log.trace("[MNFT] Signature: %s\n", temp);
      newFwInfo.signature = nullptr; // (char *) malloc(sizeof(manifestJson["digital_signature"]));
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Invalid signature!\n");
      this->valid = false;
    }
  }
  else
  {
    Log.error("[MNFT] Missing required key: digital_signature\n");
    this->valid = false;
  }

  if (manifestJson.containsKey("checksum"))
  {
    // this is checked later
    strcpy(newFwInfo.ckSum, manifestJson["checksum"]);
    Log.trace("[MNFT] Checksum received: %s\n", newFwInfo.ckSum);
    this->valid = this->valid && true;
  }
  else
  {
    Log.error("[MNFT] Missing required key: checksum\n");
    this->valid = false;
  }

  return this->valid;
}

bool ManifestHandler::validateOptionalFields()
{
  Log.trace("[MNFT] Validating optional fields\n");

  if (manifestJson.containsKey("vendor_id"))
  {
    if (checkVendor(manifestJson["vendor_id"]))
    {
      Log.trace("[MNFT] Valid vendor ID\n");
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Invalid vendor!\n");
      this->valid = false;
    }
  }

  if (manifestJson.containsKey("author"))
  {
    if (checkPermissions(manifestJson["author"]))
    {
      Log.trace("[MNFT] Author with correct permissions\n");
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Author not authorized!\n");
      this->valid = false;
    }
  }

  if (manifestJson.containsKey("size"))
  {
    if (checkMemory(manifestJson["size"].as<int>()))
    {
      Log.trace("[MNFT] Enough memory to update\n");
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Not enough memory to update!\n");
      this->valid = false;
    }
  }

  if (manifestJson.containsKey("required_version"))
  {
    if (checkMinVersion(manifestJson["required_version"]))
    {
      Log.trace("[MNFT] Required version met\n");
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Required version not met!\n");
      this->valid = false;
    }
  }

  if (manifestJson.containsKey("required_version_list"))
  {
    if (checkVersionList(manifestJson["required_version_list"].as<JsonArray>()))
    {
      Log.trace("[MNFT] Current version in required versions list\n");
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Version not in required list!\n");
      this->valid = false;
    }
  }

  if (manifestJson.containsKey("dependencies"))
  {
    if (checkDependencies()) //manifestJson["dependencies"]))
    {
      Log.trace("[MNFT] Dependencies met\n");
      this->valid = this->valid && true;
    }
    else
    {
      Log.notice("[MNFT] Dependencies not met!\n");
      this->valid = false;
    }
  }

  return this->valid;
}

bool ManifestHandler::compareVersions(const char * newVersion, const char * oldVersion)
{
  String newStr(newVersion);
  String oldStr(oldVersion);

  // fist number
  int newIdx = newStr.indexOf('.');
  int oldIdx = oldStr.indexOf('.');
  int newInt = newStr.substring(0, newIdx).toInt();
  int oldInt = oldStr.substring(0, oldIdx).toInt();
  if (newInt > oldInt)
  {
    return true;
  }
  if (newInt == oldInt)
  {
    // second number
    newInt = newStr.substring(newIdx+1, newStr.indexOf('.', newIdx+1)).toInt();
    oldInt = oldStr.substring(oldIdx+1, oldStr.indexOf('.', oldIdx+1)).toInt();
    newIdx = newStr.indexOf('.', newIdx+1);
    oldIdx = oldStr.indexOf('.', oldIdx+1);
    if (newInt > oldInt)
    {
      return true;
    }
    if (newInt == oldInt)
    {
      //third number
      newInt = newStr.substring(newIdx+1).toInt();
      oldInt = oldStr.substring(oldIdx+1).toInt();
      if (newInt > oldInt)
      {
        return true;
      }
    }
  }
  return false;
}

bool ManifestHandler::checkVersion(const char *newVersion)
{
  return compareVersions(newVersion, currentFwInfo.version);
}

bool ManifestHandler::checkDevice(const char *deviceID)
{
  return strcmp(currentFwInfo.deviceID, deviceID) == 0 ? true : false;
}

bool ManifestHandler::checkSeqNumber(const char *newSeqNumber)
{
  return strcmp(currentFwInfo.seqNumber, newSeqNumber) < 0 ? true : false;
}

bool ManifestHandler::checkKeyClaims(const char *key_claims)
{
  return true;
}

bool ManifestHandler::checkSignature(const char *signature)
{
  // TODO check signature
  return true;
}

bool ManifestHandler::checkChecksum(String md5recv)
{
  Log.trace("[MNFT] Comparing checksums\n");
  Log.trace("[MNFT] Manifest: %s\n", newFwInfo.ckSum);
  Log.trace("[MNFT] Received: %s\n", md5recv.c_str());
  return md5recv.equals(String(newFwInfo.ckSum));
}

bool ManifestHandler::checkVendor(const char * vendor)
{
  // Check this with platform?
  return true;
}

bool ManifestHandler::checkPermissions(const char * author)
{
  // Check this with platform?
  return true;
}

bool ManifestHandler::checkMemory(const int size)
{
  int freeSpace = ESP.getFreeSketchSpace();

  if(freeSpace < size)
    return false;

  return true;
}

bool ManifestHandler::checkMinVersion(const char * reqVersion)
{
  return compareVersions(currentFwInfo.version, reqVersion);
}

bool ManifestHandler::checkVersionList(const JsonArray versionList)
{
  for(JsonVariant value : versionList)
  {
    char version[9];
    strcpy(version, value);
    if(strcmp(version, currentFwInfo.version) == 0)
    {
      return true;
    }
  }
  return false;
}

bool ManifestHandler::checkDependencies()
{
  return true;
}

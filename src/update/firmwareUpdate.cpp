/*
  Firmware update class. Part of Konker FW.
  @author Maria Júlia Berriel de Sousa
*/

#include "firmwareUpdate.h"

/**
 * Constructor
 */
ESPHTTPKonkerUpdate::ESPHTTPKonkerUpdate(Protocol *client, String currentVersion) : _fwEndpoint("/firmware/"), _manifestEndpoint("/_update")
{
  _httpProtocol = client;
  _currentVersion = currentVersion;
  _newVersion = "";
  _deviceState = RUNNING;
  _last_time_update_check = 0;
}

ESPHTTPKonkerUpdate::ESPHTTPKonkerUpdate() : _fwEndpoint("/firmware/"), _manifestEndpoint("/_update")
{
  _currentVersion = "";
  _newVersion = "";
  _deviceState = RUNNING;
  _last_time_update_check = 0;
}

/**
 * Destructor
 */
ESPHTTPKonkerUpdate::~ESPHTTPKonkerUpdate()
{
  if (manifest != nullptr)
  {
    delete manifest;
  }
  _currentVersion = "";
}

void ESPHTTPKonkerUpdate::setProtocol(Protocol *client)
{
  _httpProtocol = client;
  _fwEndpoint += _httpProtocol->getUser();
  _manifestEndpoint = "sub/" + _httpProtocol->getUser() + _manifestEndpoint;
}

void ESPHTTPKonkerUpdate::setFWchannel(String id)
{
  _fwEndpoint += id;
  _manifestEndpoint = "sub/" + id + _manifestEndpoint;
}

/**
 * Perform FW update
 * @param version char*
 * @return success t_httpUpdate_return
 */
t_httpUpdate_return ESPHTTPKonkerUpdate::update(String newVersion)
{
  HTTPClient * pHttp;
  void * pHttpVoid = nullptr;
  t_httpUpdate_return ret = t_httpUpdate_return::HTTP_UPDATE_NO_UPDATES;

  if (_httpProtocol->checkConnection())
  {
    Log.trace("[UPDT] Fetching binary at: %s/binary\n", _fwEndpoint.c_str());
    // TODO
    _httpProtocol->getClient(pHttpVoid);
    pHttp = static_cast<HTTPClient *>(pHttpVoid);
    if(!pHttp)
    {
      pHttp = new HTTPClient;
    }
    pHttp->setURL(_fwEndpoint + String("/binary "));

    ret = this->handleUpdate(*pHttp, _currentVersion, false);

    Log.trace("[UPDT] Return code: %s\n", this->getLastErrorString().c_str());
  }

  return ret;
}

/**
 * Send FW update confirmation to platform
 * @param newVersion char*
 * @return none
 */
void ESPHTTPKonkerUpdate::updateSucessCallBack(const char newVersion[16])
{
  if(!_httpProtocol->checkConnection())
  {
    Log.trace("[UPDT] Cannot send confirmation\n");
    return;
  }

  Log.trace("[UPDT] Update ok, sending confirmation\n");

  //_client.addHeader("Content-Type", "application/json");
  //_client.addHeader("Accept", "application/json");

  // TODO change to REBOOTING
  String smsg=String("{\"version\": \"" + String(newVersion) + "\",\"status\":\"UPDATED\"}");
  int retCode = _httpProtocol->send(_fwEndpoint.c_str(), String(smsg));


  Log.trace("Confirmantion sent: %s; Body: %s; httpCode: %d\n", _fwEndpoint.c_str(), smsg.c_str(), retCode);

  if (!retCode){
    Serial.println("[UPDT callback] Failed\n\n");
  }else{
    Serial.println("[UPDT callback] Success\n\n");
  }
}

/**
 * Perform update and show the result
 * @param UPDATE_SUCCESS_CALLBACK_SIGNATURE
 * @return none
 */
void ESPHTTPKonkerUpdate::runUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE)
{
  Log.trace("UPDATING....\n");
  _deviceState = UPDATING;

  ESP8266HTTPUpdate::rebootOnUpdate(false);
  t_httpUpdate_return ret = this->update(_newVersion);

  switch(ret)
  {
    case HTTP_UPDATE_FAILED:
      Log.trace("[UPDT] FW update failed\n\n");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Log.trace("[UPDT] No update\n\n");
      break;
    case HTTP_UPDATE_OK:
      Log.trace("[UPDT] Complete!\n");
      (this->*updateSucessCallBack_t)(_newVersion.c_str());
      ESP.restart();
      break;
  }
}

/**
 * Wrapper for performUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE)
 * @param none
 * @return none
 */
void ESPHTTPKonkerUpdate::performUpdate() //[rpi3] apply_manifest
{
  this->runUpdate(&ESPHTTPKonkerUpdate::updateSucessCallBack);
}

/**
 * Return true if it's the first boot of a new FW
 * @param none
 * @return bool
 */
bool ESPHTTPKonkerUpdate::checkFirstBoot()
{
  //TODO Code
  return false;
}

/**
 * Send confirmation that first boot happened after FW update
 * @param newVersion char*
 * @return none
 */
void ESPHTTPKonkerUpdate::sendUpdateConfirmation(String newVersion)
{
  //TODO Code (similar to callback)
}

/**
 * Validate manifest and populate manifest.newFwInfo
 * @return true if manifest is valid
 */
bool ESPHTTPKonkerUpdate::validateUpdate()
{
  char version[16];

  int ret = manifest->validateManiest();

  if(ret)
  {
    Log.trace("[UPDT] Valid manifest\n", version);
    return true;
  }
  else
  {
    Log.trace("[UPDT] Invalid manifest\n");
    return false;
  }

  // [MJ] put htis after update complete
  // delete manifest;
}

/**
 * Return true if there is a new FW update
 * @param recvVersion char*
 * @return bool
 */
int ESPHTTPKonkerUpdate::querryPlatform()
{
  String retPayload;

  Log.trace("[UPDT] Checking for updates...\n");

  if(!_httpProtocol->checkConnection())
  {
    Log.trace("[UPDT] No connection to platform\n");
    return 1;
  }

  Log.trace("[UPDT] Checking update at: %s\n", _manifestEndpoint.c_str());

  int retCode = _httpProtocol->request(&retPayload, _manifestEndpoint);

  if(!retCode ||
      retPayload.equals("[]") ||
      retPayload.equals("Resource not found for incoming device"))
  {
    Log.trace("[UPDT] No new FW version\n");
    return 1;
  }
  else
  {
    Log.trace("[UPDT] New version exist\n");

    Log.trace("[UPDT] Payload received = %s\n\n", retPayload.c_str());
    this->manifest = new ManifestHandler();
    if(!this->manifest->startHandler())
    {
      Log.notice("[UPDT] Failed to load current FW info from flash!");
    }
    if(this->manifest->parseManifest(retPayload.c_str()))
    {
      return 0;
    }
    return 2;
  }

  return retCode;
}

/**
 * Check if there is an update
 * @param callback function*
 * @return bool
 */
bool ESPHTTPKonkerUpdate::checkForUpdate()
{
  if (_last_time_update_check != 0)
  {
    //throtle this call at maximum 1 per minute
    if ((millis() - _last_time_update_check) < 6500)
    {
      //Serial.println("checkForUpdates maximum calls is 1/minute. Please wait more to call again");
      return false;
    }
  }

  int hasManifest = this->querryPlatform();
  if (hasManifest == 0) //[rpi3] get_manifest
  {
    this->sendStatusMessage(MSG_MANIFEST_RECEIVED); // [MJ] this is probably causing some kind of memory overflow and overwriting manifestJson hmmmm
    _last_time_update_check = millis();
    if(this->validateUpdate()) //[rpi3] parse_manifest
    {
      this->sendStatusMessage(MSG_MANIFEST_CORRECT);
      return true;
    }
    this->sendExceptionMessage(EXPT_MANIFEST_INCORRECT);
    return false;
  }
  else if(hasManifest == 2)
  {
    this->sendExceptionMessage(EXPT_COULD_NOT_GET_MAN);
  }
  _last_time_update_check = millis();
  return false;
}

void ESPHTTPKonkerUpdate::sendStatusMessage(int msgIndex)
{
  const char * messages[6] =
  {
    "Manifest received",
    "Manifest correct",
    "Firmware received",
    "Checksum OK",
    "Update done",
    "Update correct"
  };

  Log.trace("[UPDT] MESSAGE: %s\n", messages[msgIndex]);

  String smsg=String("{\"update_stage\": \"" + String(messages[msgIndex]) + "\"}");
  int retCode = _httpProtocol->send("_update", smsg);

  Log.trace("[UPDT] Message sent to: _update; Body: %s; httpCode: %d\n", _manifestEndpoint.c_str(), smsg.c_str(), retCode);

  if (retCode == 1)
  {
    Log.trace("[UPDT] Success sending message\n");
  }
  else
  {
    Log.trace("[UPDT] Failed to send message\n");
  }
}

void ESPHTTPKonkerUpdate::sendExceptionMessage(int exptIndex)
{
  const char * messages[6] =
  {
    "Could not get manifest",
    "Manifest incorrect",
    "Did not receive firmware",
    "Checksum NOK",
    "Update failed",
    "New firmware started incorrectly"
  };

  Log.trace("[UPDT] MESSAGE: %s\n", messages[exptIndex]);

  String smsg=String("{\"update_exception\": \"" + String(messages[exptIndex]) + "\"}");
  int retCode = _httpProtocol->send("_update", smsg);

  Log.trace("[UPDT] Exception sent to: _update; Body: %s; httpCode: %d\n", _manifestEndpoint.c_str(), smsg.c_str(), retCode);

  if (retCode == 1)
  {
    Log.trace("[UPDT] Success sending exception\n");
  }
  else
  {
    Log.trace("[UPDT] Failed to send exception\n");
  }
}

/*
  Firmware update class. Part of Konker FW.
  @author Maria Júlia Berriel de Sousa
*/

#include "firmwareUpdate.h"

/**
 * Constructor
 */
ESPHTTPKonkerUpdate::ESPHTTPKonkerUpdate(Protocol *client, String currentVersion) : _fwEndpoint("/firmware/")
{
  _httpProtocol = client;
  _currentVersion = currentVersion;
  _newVersion = "";
  _deviceState = RUNNING;
  _last_time_update_check = 0;
}

ESPHTTPKonkerUpdate::ESPHTTPKonkerUpdate() : _fwEndpoint("/firmware/")
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
  _currentVersion = "";
}

void ESPHTTPKonkerUpdate::setProtocol(Protocol *client)
{
  _httpProtocol = client;
  _fwEndpoint += _httpProtocol->getUser();
}

void ESPHTTPKonkerUpdate::setFWchannel(String id)
{
  _fwEndpoint += id;
}

/**
 * Perform FW update
 * @param version char*
 * @return success t_httpUpdate_return
 */
t_httpUpdate_return ESPHTTPKonkerUpdate::update(String newVersion)
{
  HTTPClient * pHttp;
  void * pHttpVoid;
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

  this->updateVersion(newVersion);
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
 * Get new version from plaftform
 * @param strPayload char*
 * @param recvVersion char*
 * @return none
 */
String ESPHTTPKonkerUpdate::getVersionFromPayload(String strPayload)
{
  char version[16];
  //TODO fix
  //if(parse_JSON_item(strPayload, "version", version))
  if(true)
  {
    Log.trace("[UPDT] New version = %s\n", version);
    return String(version);
  }
  else
  {
    Log.trace("[UPDT] Failed to parse version\n");
    return "";
  }
}

/**
 * Update running version
 * @param newVersion char*
 * @return none
 */
void ESPHTTPKonkerUpdate::updateVersion(String newVersion)
{
  if(newVersion != "")
  {
    _currentVersion = newVersion;
  }
  // TODO save new version to memory
  // TODO update first boot flag in memory
}

/**
 * Return true if there is a new FW update
 * @param recvVersion char*
 * @return bool
 */
bool ESPHTTPKonkerUpdate::querryPlatform(String recvVersion)
{
  String retPayload;

  Log.trace("[UPDT] Checking for updates...\n");

  if(!_httpProtocol->checkConnection())
  {
    Log.trace("[UPDT] No connection to platform\n");
    return false;
  }

  Log.trace("[UPDT] Checking update at: %s\n", _fwEndpoint.c_str());

  int retCode = _httpProtocol->request(&retPayload, _fwEndpoint);

  if(!retCode)
  {
    Log.trace("[UPDT] No new FW version\n");
    recvVersion = ""; // [MJ] Se update falha, string da versão preenchida com vazio
  }
  else
  {
    Log.trace("[UPDT] New version exist\n\n");

    Log.trace("[UPDT] trPayload=%s", retPayload.c_str());
    if (retPayload != "[]")
    {
      recvVersion = this->getVersionFromPayload(retPayload);
    }
    // retCode = this->parseManifest(&retPayload);
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

  char recvVersion[16];
  if (this->querryPlatform(recvVersion)) //[rpi3] get_manifest
  {
    _last_time_update_check = millis();
    if(this->validateUpdate()) //[rpi3] parse_manifet
    {
      this->sendStatusMessage();
      return true;
    }
    this->sendExceptionMessage();
    return false;
  }
  _last_time_update_check = millis();
  return false;
}

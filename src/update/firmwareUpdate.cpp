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
    _client = client;
    _currentVersion = currentVersion;
    _newVersion = "";
    _deviceState = RUNNING;
    _last_time_update_check = 0;
}

ESPHTTPKonkerUpdate::ESPHTTPKonkerUpdate() : _fwEndpoint("/firmware/")
{
    //_client.begin();
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
    _client = client;
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
    HTTPClient *http;
    t_httpUpdate_return ret = t_httpUpdate_return::HTTP_UPDATE_NO_UPDATES;

    if (_client->checkConnection())
    {
        Serial.print("Fetching binary at: " + _fwEndpoint + "/binary");
        _client->getClient(http);
        if(!http)
        {
            http = new HTTPClient;
        }
        http->setURL(_fwEndpoint + String("/binary "));

        ret = ESP8266HTTPUpdate::handleUpdate(*http, _currentVersion, false);

        Serial.println("Return code: " + ESP8266HTTPUpdate::getLastErrorString());
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
    if(!_client->checkConnection())
    {
        Serial.println("[Update] Cannot send confirmation");
        return;
    }

    Serial.println("[Update] Update ok, sending confirmation.");

    //_client.addHeader("Content-Type", "application/json");
    //_client.addHeader("Accept", "application/json");

    // TODO change to REBOOTING
    String smsg=String("{\"version\": \"" + String(newVersion) + "\",\"status\":\"UPDATED\"}");
    int retCode = _client->send(_fwEndpoint.c_str(), String(smsg));


    Serial.println("Confirmantion sent: " + _fwEndpoint  + "; Body: " + smsg + "; httpcode: " + String(retCode));

    if (!retCode){
        Serial.println("[Update callback] Failed");
    }else{
        Serial.println("[Update callback] Success");
    }
    Serial.println("");

    this->updateVersion(newVersion);
}

/**
 * Check if there is an update
 * @param callback function*
 * @return bool
 */
bool ESPHTTPKonkerUpdate::checkForUpdate()
{
    if (_last_time_update_check != 0){
        //throtle this call at maximum 1 per minute
        if ((millis() - _last_time_update_check) < 6500){
            //Serial.println("checkForUpdates maximum calls is 1/minute. Please wait more to call again");
            return false;
        }
    }

    char recvVersion[16];
    if (this->querryPlatform(recvVersion))
    {
        if(String(recvVersion).indexOf(String(_currentVersion))>=0 || String(recvVersion)=="") // [MJ] necessary???
        {
            _newVersion = recvVersion;
        }
        _last_time_update_check = millis();
        return true;
    }
    _last_time_update_check = millis();
    return false;
}

/**
 * Perform update and show the result
 * @param UPDATE_SUCCESS_CALLBACK_SIGNATURE
 * @return none
 */
void ESPHTTPKonkerUpdate::runUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE)
{
    Serial.println("UPDATING....");
    _deviceState = UPDATING;

    ESP8266HTTPUpdate::rebootOnUpdate(false);
    t_httpUpdate_return ret = this->update(_newVersion);

    switch(ret)
    {
        case HTTP_UPDATE_FAILED:
            Serial.println("[Update] FW update failed.");
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[Update] No update.");
            break;
        case HTTP_UPDATE_OK:
            // Serial.println("[Update] Not sending confirmation!!! D:D:D:D:");
            (this->*updateSucessCallBack_t)(_newVersion.c_str());
            ESP.restart();
            break;
    }
    Serial.println("");
}

/**
 * Wrapper for performUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE)
 * @param none
 * @return none
 */
void ESPHTTPKonkerUpdate::performUpdate()
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
        Serial.println("New version = " + String(version));
        return String(version);
    }
    else
    {
        Serial.println("Failed to parse version");
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

    Serial.println("Checking for updates...");

    if(!_client->checkConnection())
    {
        Serial.println("No connection to platform");
        return false;
    }
    // _client.addHeader("Content-Type", "application/json");
    // _client.setTimeout(2000);
    int retCode = _client->receive(&retPayload);

    Serial.println("Checking update at: " + _fwEndpoint);

    if(!retCode)
    {
        Serial.println("[Update] No new FW version");
        Serial.println("");
        recvVersion = ""; // [MJ] Se update falha, string da versão preenchida com vazio
    }
    else
    {
        Serial.println("[Update] New version exist");
        Serial.println("");

        Serial.println("strPayload=" + retPayload);
        if (retPayload != "[]")
        {
            recvVersion = this->getVersionFromPayload(retPayload);
        }
    }

    return retCode;
}

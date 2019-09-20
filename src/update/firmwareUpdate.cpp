/*
    Firmware update class. Part of Konker FW.
    @author Maria Júlia Berriel de Sousa
*/

#include "firmwareUpdate.h"

/**
 * Constructor
 */
ESPHTTPKonkerUpdate::ESPHTTPKonkerUpdate(HTTPClient client, String currentVersion) : _fwEndpoint("/firmware/")
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

/**
 * Perform FW update
 * @param version char*
 * @return success t_httpUpdate_return
 */
t_httpUpdate_return ESPHTTPKonkerUpdate::update(String newVersion)
{
    t_httpUpdate_return ret = t_httpUpdate_return::HTTP_UPDATE_NO_UPDATES;
    if (_client.connected())
    {
        //client.begin(_device.getHost(), _device.getPort(), _device.getUri());

        Serial.print("Fetching binary at: ");
        //Serial.println(_device.getHost() + ":" + String(_connections.port) + _connections.uri);

        //Serial.println("Authorizing...");
        //client.setAuthorization(_credentials.device_login, _credentials.device_pass);
        //Serial.println("Authorization successfull");
        ret = ESP8266HTTPUpdate::handleUpdate(_client, _currentVersion, false);

        Serial.println("Return code: " + ESP8266HTTPUpdate::getLastErrorString());
    }

    return ret;
}

/**
 * Send FW update confirmation to platform
 * @param newVersion char*
 * @return none
 */
void ESPHTTPKonkerUpdate::updateSucessCallBack(char newVersion[16])
{
    if(!_client.connected())
    {
        Serial.println("[Update] Cannot send confirmation");
        return;
    }

    Serial.println("[Update] Update ok, sending confirmation.");
    bool subCode=0;

    //String fwUpdateURL= "http://" + String(_connections.host) + String (":") + String(_connections.port) + String("/registry-data/firmware/") + String(_credentials.device_login);
    //HTTPClient http;  //Declare an object of class HTTPClient
    //http.begin(fwUpdateURL);  //Specify request destination
    _client.addHeader("Content-Type", "application/json");
    _client.addHeader("Accept", "application/json");
    //http.setAuthorization(_credentials.device_login, _credentials.device_pass);

    // TODO change to REBOOTING
    String smsg=String("{\"version\": \"" + String(newVersion) + "\",\"status\":\"UPDATED\"}");
    int httpCode = _client.sendRequest("PUT", String(smsg));


    //Serial.println("Confirmantion send: " + fwUpdateURL  + "; Body: " + smsg + "; httpcode: " + String(httpCode));

    //_client.clear();   //Close connection

    //subCode = interpretHTTPCode(httpCode);
    subCode = 0;

    if (!subCode){
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
void ESPHTTPKonkerUpdate::runUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE cb)
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
            (this->*cb)(_newVersion.c_str());
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
    bool subCode=0;
    char buffer[100];
    char bffPort[6];

    Serial.println("Checking for updates...");

    //String sPort=(String)_device.getPort;
    //sPort.toCharArray(bffPort, 6);
    //if (String(_device.getHost()).indexOf("http://", 0) <= 0)
    /* {
        // Add 'http://' if not in URL
        strcpy (buffer,"http://");
    } */
    //strcpy (buffer,_device.getHost());
    strcat (buffer,":");
    strcat (buffer,bffPort);
    strcat (buffer, "/registry-data"); // [MJ] Apenas para plataforma rodando localmente
    strcat (buffer,"/firmware/");
    //strcat (buffer,_device.getLogin());

    //HTTPClient http;  //Declare an object of class HTTPClient
    if(!_client.connected())
    {
        Serial.println("No connection");
        return false;
    }
    _client.addHeader("Content-Type", "application/json");
    _client.setTimeout(2000);
    //http.setAuthorization(_credentials.device_login, _credentials.device_pass);
    //http.begin((String)buffer);  //Specify request destination
    int httpCode = _client.GET();

    Serial.println("Checking update at: " + String(buffer));

    //TODO do not depend on HTTP codes to confirm update
    //subCode = interpretHTTPCode(httpCode);

    //if (!subCode)
    if(httpCode == 404)
    {
        Serial.println("[Update] No new FW version");
        Serial.println("");
        recvVersion = ""; // [MJ] Se update falha, string da versão preenchida com vazio
    }
    else
    {
        Serial.println("[Update] New version exist");
        Serial.println("");

        String strPayload = _client.getString();
        Serial.println("strPayload=" + strPayload);
        if (strPayload != "[]")
        {
            recvVersion = this->getVersionFromPayload(strPayload);
        }
    }
    //_client.clear();   //Close connection

    return subCode;
}

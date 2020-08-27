#include "konker.h"

#define MAX_NAME_SIZE    10

// ---------------------------------------------------------------------------//

KonkerDevice::KonkerDevice() : deviceWifi(), deviceMonitor(&this->deviceWifi), webServer(80), deviceUpdate()
{
  if (DEBUG_LEVEL>0 && !Serial)
  {
    Serial.begin(115200);
    while (!Serial) delay(100);
  }

  // Third parameter is showLevel
  Log.begin(DEBUG_LEVEL, &Serial, true);

  Log.trace("BUILD: %s\n", BUILD_ID);
  this->chipID = ESP.getChipId();

  this->defaultConnectionType = ConnectionType::MQTT;

  deviceMonitor.restoreHealthInfo();
  deviceNTP.startNTP();

  // Turn LED on when booting device
  pinMode(_STATUS_LED, OUTPUT);
	digitalWrite(_STATUS_LED, LOW);

  delay(1000);
}


KonkerDevice::~KonkerDevice()
{
  deviceMonitor.saveHealthInfo();
  // delete currentProtocol;
}

void KonkerDevice::restartDevice()
{
  // save stuff to memory
  deviceMonitor.saveHealthInfo();

  delay(3000);
#ifndef ESP32
  ESP.reset();
#else
  ESP.restart();,
#endif
}

// TODO what is this
void KonkerDevice::resetALL()
{
  deviceWifi.disconnectClientWifi();

  // formatFileSystem();
  Log.trace("Full reset done! FileSystem formated!");
  Log.trace("You must remove this device from Konker plataform if registred, and redo factory configuration.");

  delay(5000);
  restartDevice();
}

void KonkerDevice::loop()
{
  deviceNTP.updateNTP();
  if (this->currentProtocol == nullptr)
  {
    Log.error("Protocol not set! Please call KonkerDevice::setDefaultConnectionType at setup!");
  }
  else
  {
    if(this->currentProtocol->protocolLoop())
    {
      Log.trace("\n\n!!!!! Restart here !!!!!!\n\n");
      restartDevice();
    }
  }
  if (deviceUpdate.checkForUpdate())
  {
    deviceUpdate.performUpdate();
  }
  deviceMonitor.healthUpdate(this->avgLoopDuration);
  this->avgLoopDuration = 0;
  this->loopCount = 1;
  delay(100);
}

void KonkerDevice::addWifi(String ssid, String password)
{
  deviceWifi.setConfig(ssid, password);
}

void KonkerDevice::clearWifi(String ssid)
{
  deviceWifi.removeConfig(ssid);
}

bool KonkerDevice::connectWifi()
{
  int res = deviceWifi.tryConnectClientWifi();
  if (res)
  {
    // Turn LED off
    digitalWrite(_STATUS_LED, HIGH);
    Log.trace("Device connected to WiFi\n");
    Log.trace("Local IP: %s\n", deviceWifi.getLocalIP().toString().c_str());
  }
  else
  {
    Log.trace("Failed to connected to WiFi\n");
  }
  return res;
}

bool KonkerDevice::checkWifiConnection()
{
  return deviceWifi.checkConnectionStatus();
}

void KonkerDevice::setDefaultConnectionType(ConnectionType c)
{
  this->defaultConnectionType = c;
  sendBuffer.setConnectionType(this->defaultConnectionType);

  Protocol* newConnection;

	switch (this->defaultConnectionType)
  {
		case ConnectionType::HTTP:
			newConnection = new HTTPProtocol();
			break;
		case ConnectionType::HTTPS:
			newConnection = new HTTPSProtocol();
			break;
		case ConnectionType::MQTT:
			newConnection = new MQTTProtocol();
			break;
		case ConnectionType::MQTTS:
			newConnection = new MQTTSProtocol();
			break;
		case ConnectionType::TCP:
			newConnection = new TCPProtocol();
			break;
		case ConnectionType::UDP:
			// newConnection = new UDPProtocol();
      newConnection = new HTTPProtocol();
			break;
		default:
			newConnection = nullptr;
	}

  if (newConnection != nullptr)
  {
    this->currentProtocol = newConnection;
  }

  this->httpProtocol = new HTTPProtocol();
}

void KonkerDevice::setFallbackConnectionType(ConnectionType c)
{
  this->fallbackConnectionType = c;
}

// handle connection to the server used to send and receive data for this device
void KonkerDevice::startConnection(bool afterReconnect)
{
  stopConnection();

	if (checkProtocol())
  {
    if(afterReconnect)
    {
      this->currentProtocol->increaseConnFail();
    }
    else
    {
      if(!this->currentProtocol->isCredentialSet())
      {
        String deviceid, user, password;
        int ret = this->getCredentialsForPlatform(&deviceid, &user, &password);
        if(ret == 2)
        {
          Log.trace("Credentials received from gateway: %s/%s/%s\n", deviceid.c_str(), user.c_str(), password.c_str());
          this->setPlatformCredentials(deviceid, user, password);
        }
        else if (ret == 0)
        {
          Log.error("No platform credentials!!! Restarting...\n\n");
          this->restartDevice();
        }
      }

      if (this->currentProtocol->isConnectionOriented())
      {
        deviceMonitor.setProtocol(this->currentProtocol, this->httpProtocol);
        // update the ESP update client with this new connection
        deviceUpdate.setProtocol(this->httpProtocol);
      }
    }

    this->currentProtocol->connect();
    this->httpProtocol->connect();

    // at this point, device connections are initialized
    if(!afterReconnect)
    {
      deviceUpdate.checkFirstBoot();
    }
	}
}

void KonkerDevice::stopConnection()
{
	if (this->currentProtocol != nullptr)
  {
		if (this->currentProtocol->checkConnection())
    {
			this->currentProtocol->disconnect();
		}
	}
}

int KonkerDevice::checkPlatformConnection()
{
	if (this->currentProtocol != nullptr)
  {
		return this->currentProtocol->checkConnection();
	}
	return NOT_CONNECTED;
}

// calculate moving average of main loop duration
void KonkerDevice::loopDuration(unsigned int duration)
{
  this->avgLoopDuration = this->avgLoopDuration + ((duration - this->avgLoopDuration)/this->loopCount);
  this->loopCount++;
}

void KonkerDevice::getCurrentTime(char *timestamp)
{
  deviceNTP.getTimeNTP(timestamp);
}

bool KonkerDevice::checkProtocol()
{
  if (this->currentProtocol == nullptr)
  {
    Log.error("Protocol not set! Please call KonkerDevice::setDefaultConnectionType first!");
    return false;
  }

  return true;
}

void KonkerDevice::setServer(String host, int port)
{
  if (checkProtocol())
  {
    this->currentProtocol->setConnection(host, port);
    this->httpProtocol->setConnection("data.prod.konkerlabs.net", 80);
  }
}

void KonkerDevice::setServer(String host, int port, int httpPort)
{
  if (checkProtocol())
  {
    this->currentProtocol->setConnection(host, port);
    this->httpProtocol->setConnection(host, httpPort);
  }
}

void KonkerDevice::setChipID(String deviceID)
{
#ifndef ESP32
  this->chipID = deviceID + "_" + ESP.getChipId();
#else
  this->chipID = deviceID + "_" + ESP.getEfuseMac();
#endif
}

void KonkerDevice::setDeviceIds(String id)
{
  this->deviceID = id;
  setChipID(this->deviceID);
}

String KonkerDevice::getDeviceId()
{
  return this->deviceID;
}

void KonkerDevice::setPlatformCredentials(String userid, String password)
{
  this->setDeviceIds(DEFAULT_NAME);

  if (checkProtocol())
  {
    this->currentProtocol->setPlatformCredentials(userid, password);
    this->httpProtocol->setPlatformCredentials(userid, password);
  }
}

void KonkerDevice::setPlatformCredentials(String deviceID, String userid, String password)
{
  this->setDeviceIds(deviceID);

  if (checkProtocol())
  {
    this->currentProtocol->setPlatformCredentials(userid, password);
    this->httpProtocol->setPlatformCredentials(userid, password);
  }
}

bool KonkerDevice::savePlatformCredentials()
{
  if(checkProtocol())
  {
    return this->currentProtocol->savePlatformCredentials();
  }

  return false;
}

bool KonkerDevice::saveWifiCredentials()
{
  return this->deviceWifi.saveWifiCredentials();
}

bool KonkerDevice::saveAllCredentials()
{
  bool ret = true;

  Log.trace("Saving credentials to memory\n");

  ret = ret & saveWifiCredentials();
  ret = ret & savePlatformCredentials();

  if (!ret)
  {
    Log.warning("Could not store credentials in memory!\n");
  }

  return ret;
}

bool KonkerDevice::restorePlatformCredentials()
{
  if(checkProtocol())
  {
    return this->currentProtocol->restorePlatformCredentials();
  }

  return false;
}

bool KonkerDevice::restoreWifiCredentials()
{
  return this->deviceWifi.restoreWifiCredentials();
}

bool KonkerDevice::restoreAllCredentials()
{
  bool ret = true;

  Log.trace("Restoring credentials from memory\n");

  ret = ret & restoreWifiCredentials();
  ret = ret & restorePlatformCredentials();

  if (!ret)
  {
    Log.warning("Could not restore credentials from memory!\n");
  }

  return ret;
}

// returns true if recovered credentials from server, false otherwise
int KonkerDevice::getCredentialsForPlatform(String * deviceid, String * user, String * password)
{
  String response;

  Log.trace("Trying to recover credentials from memmory\n");
  if(this->restorePlatformCredentials())
  {
    Log.trace("Device credentials recovered from memory\n");
    // TODO store deviceid in eeprom
    this->setDeviceIds(DEFAULT_NAME);
    return 1;
  }

  Log.trace("Trying to recover credentials from server for device %s\n", this->chipID.c_str());
  if(this->httpProtocol->getPlatformCredentials(&response, this->chipID))
  {
    Log.trace("Credentials received from server\n");
    DynamicJsonDocument responseJson(256);
    DeserializationError err = deserializeJson(responseJson, response);
    if (err)
    {
      Log.notice("[JSON] Failed to desirialize json document. Code = %s\n", err.c_str());
      return 0;
    }
    *deviceid = responseJson["device_id"].as<String>();
    *user = responseJson["username"].as<String>();
    *password = responseJson["password"].as<String>();
    return 2;
  }

  return 0;
}

int KonkerDevice::storeData(String channel, String payload)
{
  Log.trace("Storing data in buffer\n");
  // sendBuffer.printBufferStatus();
  return sendBuffer.collectData(channel, payload);
}

BufferElement KonkerDevice::recoverData()
{
  Log.trace("Rocovering data from buffer\n");
  // sendBuffer.printBufferStatus();
  return sendBuffer.consumeData();
}

int KonkerDevice::sendData(String channel, String payload)
{
  int res = 0;

  res = this->currentProtocol->send(channel.c_str(), payload);
  if(res == CONNECTED)
  {
    Log.trace("Payload sent to platform\n");
  }

  return res;
}

int KonkerDevice::sendData()
{
  int res = 0;
  int dataPosition;
  BufferElement data;

  if(sendBuffer.isEmpty())
  {
    Log.trace("Empty buffer, nothing to send\n");
    return 0;
  }

  dataPosition = sendBuffer.getData(&data);
  if(dataPosition >= 0)
  {
    res = this->currentProtocol->send(data.channel, String(data.payload));
    if(res)
    {
      Log.trace("Payload sent to platform\n");
      sendBuffer.removeData(dataPosition);
    }
    else
    {
      Log.warning("Could not send payload to platform, try again later\n");
      sendBuffer.incrementRetries(dataPosition);
    }
  }

  return res;
}

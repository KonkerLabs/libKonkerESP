#include "konker.h"

#define MAX_NAME_SIZE    10

// ---------------------------------------------------------------------------//

KonkerDevice::KonkerDevice() : deviceWifi(), deviceMonitor(&this->deviceWifi), webServer(80)//, update()
{
  if (DEBUG_LEVEL>0 && !Serial)
  {
    Serial.begin(115200);
    while (!Serial) delay(100);
  }

  // Third parameter is showLevel
  Log.begin(DEBUG_LEVEL, &Serial, true);

  Log.trace("BUILD: %s", BUILD_ID);

  this->defaultConnectionType = ConnectionType::MQTT;

  deviceMonitor.restoreHealthInfo();

  // Turn LED on when booting device
  pinMode(_STATUS_LED, OUTPUT);
	digitalWrite(_STATUS_LED, LOW);

  //update.setFWchannel(this->userid);

  delay(1000);
}


KonkerDevice::~KonkerDevice()
{
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
    this->wifi_connection_errors += 1;
  }
  return res;
}

bool KonkerDevice::checkWifiConnection()
{
  return deviceWifi.checkConnectionStatus();
}

// void KonkerDevice::checkForUpdates()
// {
//   update.checkForUpdate();
// }

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
		if (this->currentProtocol->isConnectionOriented())
    {
			deviceMonitor.setProtocol(this->currentProtocol);
		}

		this->currentProtocol->connect();

    if(afterReconnect)
    {
      this->currentProtocol->increaseConnFail();
    }

		// update the ESP update client with this new connection
		// update.setProtocol(newConnection);
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

void KonkerDevice::loopDuration(unsigned int duration)
{
  this->avgLoopDuration = this->avgLoopDuration + ((duration - this->avgLoopDuration)/this->loopCount);
  this->loopCount++;
}

void KonkerDevice::loop()
{
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
  // checkForUpdates();
  deviceMonitor.healthUpdate(this->avgLoopDuration);
  this->avgLoopDuration = 0;
  this->loopCount = 1;
  delay(1000);
}

bool KonkerDevice::checkProtocol()
{
  if (this->currentProtocol == nullptr)
  {
    Log.error("Protocol not set! Please call KonkerDevice::setDefaultConnectionType first!");
    // restart here?
    return false;
  }

  return true;
}

void KonkerDevice::setServer(String host, int port)
{
  if (checkProtocol())
  {
    this->currentProtocol->setConnection(host, port);
  }
}

void KonkerDevice::setChipID(String deviceID)
{
#ifndef ESP32
  this->chipID = deviceID + ESP.getChipId();
#else
  this->chipID = deviceID + ESP.getEfuseMac();
#endif
}

void KonkerDevice::setUniqueID(String id)
{
  this->deviceID = id;
  setChipID(this->deviceID);
}

String KonkerDevice::getUniqueID()
{
  return this->deviceID;
}

void KonkerDevice::setPlatformCredentials(String userid, String password)
{
  this->deviceID = DEFAULT_NAME;
  setChipID(this->deviceID);

  if (checkProtocol())
  {
    this->currentProtocol->setPlatformCredentials(userid, password);
  }
}

void KonkerDevice::setPlatformCredentials(String deviceID, String userid, String password)
{
  this->deviceID = deviceID;
  setChipID(this->deviceID);
  if (checkProtocol())
  {
    this->currentProtocol->setPlatformCredentials(userid, password);
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

// TODO
// int KonkerDevice::testSendHTTP()
// {
//   StaticJsonDocument<512> jsonMSG;
//   char bufferJson[1024];
//   HTTPProtocol httpObj;
//   int conn = 0, res = 0;
//
//   String ssid = deviceWifi.getWifiSSID();
//   String ip = deviceWifi.getLocalIP().toString();
//   int rssi = deviceWifi.getWifiStrenght();
//
//   httpObj.setConnection("data.prod.konkerlabs.net", 80);
//   httpObj.setCredentials(this->userid.c_str(), this->password.c_str());
//
//   conn = httpObj.connect();
//   if (conn)
//   {
//     jsonMSG["ssid"] = ssid;
//     jsonMSG["ip"] = ip;
//     jsonMSG["rssi"] = String(rssi);
//
//     serializeJson(jsonMSG, bufferJson);
//
//     Log.trace("[Health] Sending message: %s\n", bufferJson);
//     res = httpObj.send(this->_health_channel.c_str(), String(bufferJson));
//   }
//
//   return res;
// }

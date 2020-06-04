#include "konker.h"

#define MAX_NAME_SIZE    10

// ---------------------------------------------------------------------------//

KonkerDevice::KonkerDevice() : deviceWifi(), webServer(80)//, update()
{
  if (DEBUG_LEVEL>0 && !Serial)
  {
    Serial.begin(115200);
    while (!Serial) delay(100);
  }

  // Third parameter is showLevel
  Log.begin(DEBUG_LEVEL, &Serial, true);

  Log.trace("BUILD: %s", BUILD_ID);

  // this->defaultConnectionType = ConnectionType::MQTT;

  // Turn LED on when booting device
  pinMode(_STATUS_LED, OUTPUT);
	digitalWrite(_STATUS_LED, LOW);

  //update.setFWchannel(this->userid);

  delay(1000);
}


KonkerDevice::~KonkerDevice()
{
}

// void KonkerDevice::formatFileSystem()
// {
//   return;
// }

void KonkerDevice::resetALL()
{
  deviceWifi.disconnectClientWifi();

  // formatFileSystem();
  Log.trace("Full reset done! FileSystem formated!");
  Log.trace("You must remove this device from Konker plataform if registred, and redo factory configuration.");

  delay(5000);
#ifndef ESP32
  ESP.reset();
#else
  ESP.restart();,
#endif
  delay(1000);
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
    Log.trace("Device connected to WiFi");
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
}

void KonkerDevice::setFallbackConnectionType(ConnectionType c)
{
  this->fallbackConnectionType = c;
}

// handle connection to the server used to send and receive data for this device
void KonkerDevice::startConnection()
{
	if (this->currentProtocol != nullptr)
  {
		stopConnection();
		this->currentProtocol = nullptr;
	}

	Protocol* newConnection;
	bool connectionOriented = true;

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
			connectionOriented = false;
			break;
		default:
			newConnection = nullptr;
			connectionOriented = false;
	}

	if (newConnection != nullptr)
  {
		if (connectionOriented)
    {
			newConnection->setConnection(this->host, this->port);
			newConnection->setCredentials(this->userid.c_str(), this->password.c_str());
		}
		this->currentProtocol = newConnection;
		this->currentProtocol->connect();

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

void KonkerDevice::loop()
{
  this->currentProtocol->protocolLoop();
  // checkForUpdates();
  //healthUpdate(_health_channel);
  delay(1000);
}

void KonkerDevice::setServer(String host, int port)
{
	this->host = host;
	this->port = port;
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
	this->userid = userid;
	this->password = password;
}

void KonkerDevice::setPlatformCredentials(String deviceID, String userid, String password)
{
  this->deviceID = deviceID;
  setChipID(this->deviceID);
	this->userid = userid;
	this->password = password;
}

int KonkerDevice::sendData(String channel, String payload)
{
  int res;

  res = this->currentProtocol->send(channel.c_str(), payload);
  if(res)
  {
    Log.trace("Payload sent to platform\n");
  }

  return res;
}

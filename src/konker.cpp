#include "konker.h"

#include "./protocols/all_protocols.h"

#define MAX_NAME_SIZE    10





// ---------------------------------------------------------------------------//

ConfigWifi::ConfigWifi() {
    for (int x = 0; x < 4; x++) this->ip[x] = 0;
    for (int x = 0; x < 4; x++) this->gateway[x] = 0;
    for (int x = 0; x < 4; x++) this->subnet[x] = 0;
}

void ConfigWifi::setIP (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
  this->ip[0] = first_octet;
  this->ip[1] = second_octet;
  this->ip[2] = third_octet;
  this->ip[3] = fourth_octet;
  this->i=1;
}

void ConfigWifi::setGateway (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
  this->gateway[0] = first_octet;
  this->gateway[1] = second_octet;
  this->gateway[2] = third_octet;
  this->gateway[3] = fourth_octet;
  this->g=1;
}

void ConfigWifi::setSubnet (uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
  this->subnet[0] = first_octet;
  this->subnet[1] = second_octet;
  this->subnet[2] = third_octet;
  this->subnet[3] = fourth_octet;
  this->s=1;
}


bool ConfigWifi::isConfigured() {
  return this->i && this->g && this->s;
}

// ---------------------------------------------------------------------------//

KonkerDevice::KonkerDevice() : webServer(80), update(), wifiFile("/wifi.json")
{
  if (DEBUG_LEVEL>0 && !Serial){
    Serial.begin(115200);
    while (!Serial) delay(100);
  }
  WiFi.persistent(false);
  WiFi.disconnect();
  delay(10);  
  WiFi.setAutoConnect(false);  
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  delay(10);  
  WiFi.setAutoReconnect(true);
  if (DEBUG_LEVEL>0){
    Serial.println("BUILD: " + (String)BUILD_ID);
  }

  delay(1000);
}


KonkerDevice::~KonkerDevice() {

}

void KonkerDevice::formatFileSystem()
{


}

void KonkerDevice::resetALL()
{
    WiFi.mode(WIFI_OFF);
    delay(100);
    
    formatFileSystem();
    if (DEBUG_LEVEL>0){
      Serial.println(F("Full reset done! FileSystem formated!"));
      Serial.println(F("You must remove this device from Konker plataform if registred, and redo factory configuration."));
    }
    

    delay(5000);
    #ifndef ESP32
    ESP.reset();
    #else
    ESP.restart();,
    #endif
    delay(1000);
}

void KonkerDevice::setPlatformCredentials(String userid, String password) {
	this->userid = userid;
	this->password = password;
}

void KonkerDevice::setName(const char * newName)
{
  NAME = newName;

  #ifndef ESP32
  ChipId = NAME + String(ESP.getChipId());
  #else
  ChipId = NAME + (uint32_t)ESP.getEfuseMac();
  #endif
}

void KonkerDevice::addWifi(String ssid, String password)
{

}

void KonkerDevice::clearWifi(String ssid)
{

}

void KonkerDevice::checkForUpdates() {
  update.checkForUpdate();
}

void KonkerDevice::loop() {
    #ifdef pubsubMQTT
    MQTTLoop();
    #endif
    checkForUpdates();
    healthUpdate(_health_channel);
}

// handle connection to the server used to send and receive data for this device
void KonkerDevice::startConnection() {

	if (this->currentProtocol != nullptr) {
		stopConnection();
		delete this->currentProtocol;
		this->currentProtocol = nullptr;
	}

	Protocol* newConnection; 
	bool connectionOriented = true;

	switch (defaultConnectionType) {
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
			connectionOriented = false;
			break;
		default: 
			newConnection = nullptr;
			connectionOriented = false;
	}


	if (newConnection != nullptr) {
		if (connectionOriented) {
			newConnection->setConnection(this->host, this->port);
			newConnection->setCredentials(this->userid.c_str(), this->password.c_str());
		}
		this->currentProtocol = newConnection;
		this->currentProtocol->connect();

		// update the ESP update client with this new connection 

		update.setProtocol(newConnection);
	}
}

void KonkerDevice::stopConnection() {
	if (this->currentProtocol != nullptr) {
		if (this->currentProtocol->checkConnection()) {
			this->currentProtocol->disconnect();
		}
	}


}
int KonkerDevice::checkConnection() {
		if (this->currentProtocol != nullptr) {
			return this->currentProtocol->checkConnection();
		}
		return NOT_CONNECTED;

}

void KonkerDevice::setServer(String host, int port) {
	this->host = host;
	this->port = port;
}


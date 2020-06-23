#include "manage_wifi.h"

WifiManager::WifiManager()
{
  WiFi.persistent(false);
  WiFi.disconnect();
  delay(10);
  WiFi.setAutoConnect(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  delay(10);
  WiFi.setAutoReconnect(true);

  this->wifiFile = "wifi.json";
}

WifiManager::WifiManager(String wifiFile) : WifiManager()
{
  this->wifiFile = wifiFile;
}

String getConnectMessage(int status_code)
{
	switch(status_code)
  {
		case 255: return "WL_NO_SHIELD"; break;
		case 0: return "WL_IDLE_STATUS"; break;
		case 1: return "WL_NO_SSID_AVAIL"; break;
		case 2: return "WL_SCAN_COMPLETED"; break;
		case 3: return "WL_CONNECTED"; break;
		case 4: return "WL_CONNECT_FAILED"; break;
		case 5: return "WL_CONNECTION_LOST"; break;
		case 6: return "WL_DISCONNECTED"; break;
	}
	return "UNDEFINED";
}

bool WifiManager::tryConnect(String ssid, String password)
{
  int counter = 100;

	WiFi.mode(WIFI_OFF);
	delay(500);
	WiFi.mode(WIFI_STA);
	delay(500);
	//check if we have ssid and pass and force those, if not, try with last saved values
  Log.notice("[WiFi] WiFi.begin(%s, %s)\n", ssid.c_str(), password.c_str());
	WiFi.begin(ssid, password);

  // unsigned long wifiStartTime=millis();
	// Serial.println("checking for connection status");
	// Serial.print("wifi timeout = ");
	// Serial.println(__wifiTimout);
	// Serial.print("startTime = ");
	// Serial.println(wifiStartTime);
	// Serial.print("now = ");
	// Serial.println(millis());
  // NO WHILE (millis()-wifiStartTime)<(__wifiTimout*i)

  while (WiFi.status() != WL_CONNECTED && counter > 0)
  {
    delay(500);
    if (DEBUG_LEVEL>0) Serial.print(".");
    counter -= 1;
  }
  if (DEBUG_LEVEL>0) Serial.print("\n");

  int connRes = WiFi.status();
  Log.notice("[WiFi] Connection Status = %d %s\n", connRes, getConnectMessage(connRes).c_str());

  return (connRes == WL_CONNECTED);
}

bool WifiManager::tryConnectSSID(String ssid, String password, int retries)
{
  bool connRes = false;
  Log.notice("[WiFi] Will try to connect %d times\n", retries);
  if (DEBUG_LEVEL>0)
  {
    Serial.print(">>>>>> DEVICE MAC ADDRESS = ");
  	Serial.println(WiFi.macAddress());
  }
  for(int i=0; i < retries; i++)
  {
    connRes = this->tryConnect(ssid, password);
    if(connRes) break;
  }

  return connRes;
}

void WifiManager::setConfig(String ssid, String password)
{
  int index = 0;

  if(this->numWifiCredentials > 0 &&  this->numWifiCredentials < MAX_NUM_WIFI_CRED)
  {
    index = this->numWifiCredentials + 1;
  }

  Log.notice("[WiFi] Setting new WiFi credentials at index %d\n", index);

  this->wifiCredentials[index].SSID = ssid;
  this->wifiCredentials[index].PASSWD = password;

  this->numWifiCredentials += 1;
}

void WifiManager::setConfig(String ssid0, String password0, String ssid1, String password1)
{
  setConfig(ssid0, password0);
  setConfig(ssid1, password1);
}

void WifiManager::setConfig(String ssid0, String password0, String ssid1, String password1, String ssid2, String password2)
{
  setConfig(ssid0, password0);
  setConfig(ssid1, password1);
  setConfig(ssid2, password2);
}

void WifiManager::removeConfig(String ssid)
{
  WifiCredentials temp[MAX_NUM_WIFI_CRED];
  int numTemp = 0;

  for(int i=0; i<this->numWifiCredentials; i++)
  {
    if(this->wifiCredentials[i].SSID == ssid)
    {
      continue;
    }
    temp[numTemp].SSID = this->wifiCredentials[i].SSID;
    temp[numTemp].PASSWD = this->wifiCredentials[i].PASSWD;
    numTemp++;
  }

  memcpy(this->wifiCredentials, temp, sizeof(this->wifiCredentials));
  this->numWifiCredentials = numTemp;
}

bool WifiManager::tryConnectClientWifi()
{
  bool conn = false;

  for (int i=0; i < this->numWifiCredentials; i++)
  {
    Log.notice("[WiFi] Trying to connect to SSID: %s\n", this->wifiCredentials[i].SSID.c_str());

    conn = this->tryConnectSSID(this->wifiCredentials[i].SSID, this->wifiCredentials[i].PASSWD, 2);
  }

  return conn;
}

bool WifiManager::checkConnectionStatus()
{
  return (WiFi.status() == WL_CONNECTED);
}

void WifiManager::disconnectClientWifi()
{
  WiFi.mode(WIFI_OFF);
  delay(100);
}

int WifiManager::getNumberOfWifiCredentials()
{
  return this->numWifiCredentials;
}

IPAddress WifiManager::getLocalIP()
{
  IPAddress ip;

  ip = WiFi.localIP();
  // Log.trace("Local IP address: %s\n", ip.toString().c_str());

  return ip;
}

int WifiManager::getWifiStrenght()
{
  return WiFi.RSSI();
}

String WifiManager::getWifiSSID()
{
  return WiFi.SSID();
}

// bool WifiManager::isConfigured()
// {
//   return this->i && this->g && this->s;
// }
